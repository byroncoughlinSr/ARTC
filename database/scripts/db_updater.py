#!/usr/bin/env python3
"""
Deploy the ARTC schema, one feature at a time, in dependency order.

Each feature is a file in deploy/ with a matching file in revert/ and verify/.
A feature declares what it needs with a header comment:

    -- Deploy artc:child to mysql
    -- requires: person
    -- idempotent

Applied features are recorded in a deploy_state table along with the sha1 of
the file that was applied, so a changed file is visible in `status`.

    ./db_updater.py status      # what is deployed, and what has drifted
    ./db_updater.py deploy      # apply everything outstanding
    ./db_updater.py deploy host # apply one feature and its requirements
    ./db_updater.py verify      # check the deployed shape
    ./db_updater.py revert host # drop one feature and anything needing it

Adapted from the SenseCraft server's db_updater.py. Two things differ, both
because of MySQL rather than by preference:

  * MySQL commits implicitly on DDL, so a deploy file cannot be one atomic
    transaction the way the PostgreSQL originals are. Feature files are
    written to be idempotent instead, and state is recorded per feature, so
    an interrupted run is resumed by running deploy again.
  * The versioned-file and migration machinery (feature-1.sql, feature-1M.sql,
    deploy/attic) is not reproduced. This schema has no deployed history to
    migrate from yet; add it when there is one.
"""

import argparse
import hashlib
import re
import sys
from pathlib import Path

from utils import app_connection, mysql_command, read_config

REQUIRES_PATTERN = re.compile(r"^--\s+requires:\s+(\w+)\s*$")
IDEMPOTENT_PATTERN = re.compile(r"^--\s+idempotent\s*$")

DEPLOY_DIR = Path(__file__).resolve().parent.parent / "deploy"
REVERT_DIR = DEPLOY_DIR.parent / "revert"
VERIFY_DIR = DEPLOY_DIR.parent / "verify"

STATE_TABLE = """CREATE TABLE IF NOT EXISTS deploy_state (
    feature     VARCHAR(64) NOT NULL,
    sha1sum     CHAR(40) NOT NULL,
    deployed_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (feature)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4"""


class Feature:
    """One deploy file, its declared requirements, and its current checksum."""

    def __init__(self, name, path):
        self.name = name
        self.path = path
        self.requires = set()
        self.idempotent = False
        text = path.read_text()
        for line in text.splitlines():
            match = REQUIRES_PATTERN.match(line)
            if match:
                self.requires.add(match.group(1))
            if IDEMPOTENT_PATTERN.match(line):
                self.idempotent = True
        self.sha1sum = hashlib.sha1(text.encode()).hexdigest()


def read_features():
    """Load every deploy file and check its declared requirements resolve."""
    features = {}
    for path in sorted(DEPLOY_DIR.glob("*.sql")):
        features[path.stem] = Feature(path.stem, path)
    for feature in features.values():
        missing = feature.requires - set(features)
        if missing:
            raise SystemExit(
                f"{feature.path.name} requires unknown feature(s): {', '.join(sorted(missing))}"
            )
    return features


def ordered(features, names):
    """Depth-first topological order: every requirement before its dependant."""
    result, visiting = [], set()

    def visit(name, trail):
        if name in result:
            return
        if name in visiting:
            raise SystemExit(f"requirement cycle: {' -> '.join(trail + [name])}")
        visiting.add(name)
        for requirement in sorted(features[name].requires):
            visit(requirement, trail + [name])
        visiting.discard(name)
        result.append(name)

    for name in names:
        visit(name, [])
    return result


def dependants(features, name):
    """Every feature that would break if `name` were dropped, `name` last."""
    result = []
    for other in features:
        if name in features[other].requires and other not in result:
            result.extend(d for d in dependants(features, other) if d not in result)
    if name not in result:
        result.append(name)
    return result


def read_state(conn):
    """The deploy_state table as {feature: sha1sum}."""
    output = mysql_command(sql="SELECT feature, sha1sum FROM deploy_state", **conn)
    state = {}
    for line in output.splitlines():
        if line.strip():
            feature, sha1sum = line.split("\t")[:2]
            state[feature] = sha1sum
    return state


def init(conn):
    mysql_command(sql=STATE_TABLE, **conn)
    print("deploy_state ready")


def status(conn, features):
    state = read_state(conn)
    for name in ordered(features, sorted(features)):
        feature = features[name]
        if name not in state:
            mark, note = "missing", ""
        elif state[name] == feature.sha1sum:
            mark, note = "ok", ""
        elif feature.idempotent:
            mark, note = "changed", "(idempotent: deploy will re-apply)"
        else:
            mark, note = "CHANGED", "(not idempotent: revert then deploy)"
        print(f"  {mark:8} {name} {note}".rstrip())
    for name in sorted(set(state) - set(features)):
        print(f"  {'orphan':8} {name} (deployed, but no file in deploy/)")


def deploy(conn, features, names, dry_run=False):
    state = read_state(conn)
    targets = ordered(features, names or sorted(features))
    applied = 0
    for name in targets:
        feature = features[name]
        if state.get(name) == feature.sha1sum:
            continue
        if name in state and not feature.idempotent:
            print(f"  skip     {name}: file changed and it is not idempotent; "
                  f"revert it first", file=sys.stderr)
            continue
        print(f"  deploy   {name}")
        applied += 1
        if dry_run:
            continue
        mysql_command(sqlfile=feature.path, **conn)
        mysql_command(
            sql="INSERT INTO deploy_state (feature, sha1sum) "
                f"VALUES ('{name}', '{feature.sha1sum}') "
                "ON DUPLICATE KEY UPDATE sha1sum = VALUES(sha1sum), "
                "deployed_at = CURRENT_TIMESTAMP",
            **conn,
        )
    if not applied:
        print("nothing to do")
    else:
        print(f"{applied} feature(s) {'would be deployed' if dry_run else 'deployed'}")


def revert(conn, features, names, dry_run=False):
    state = read_state(conn)
    targets = []
    for name in names:
        for dependant in dependants(features, name):
            if dependant not in targets:
                targets.append(dependant)
    for name in targets:
        if name not in state:
            continue
        path = REVERT_DIR / f"{name}.sql"
        if not path.exists():
            raise SystemExit(f"no revert file for {name}")
        print(f"  revert   {name}")
        if dry_run:
            continue
        mysql_command(sqlfile=path, **conn)
        mysql_command(sql=f"DELETE FROM deploy_state WHERE feature = '{name}'", **conn)


def verify(conn, features, names):
    failures = 0
    for name in ordered(features, names or sorted(features)):
        path = VERIFY_DIR / f"{name}.sql"
        if not path.exists():
            print(f"  {'no file':8} {name}")
            continue
        try:
            mysql_command(sqlfile=path, **conn)
            print(f"  {'ok':8} {name}")
        except RuntimeError as error:
            failures += 1
            print(f"  {'FAILED':8} {name}: {error}", file=sys.stderr)
    return 1 if failures else 0


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("verb", choices=("init", "status", "deploy", "revert", "verify"))
    parser.add_argument("features", nargs="*", help="features to act on; default all")
    parser.add_argument("-n", "--dry-run", action="store_true",
                        help="report what would run, without running it")
    parser.add_argument("-d", "--debug", action="store_true", help="echo commands")
    args = parser.parse_args()

    config = read_config()
    conn = app_connection(config)
    if not conn.get("user"):
        raise SystemExit("config.ini has no account. Run ./initial_setup.py first.")
    conn["debug"] = args.debug

    features = read_features()
    unknown = set(args.features) - set(features)
    if unknown:
        raise SystemExit(f"unknown feature(s): {', '.join(sorted(unknown))}")

    if args.verb == "init":
        init(conn)
        return 0

    try:
        read_state(conn)
    except RuntimeError:
        raise SystemExit("No deploy_state table. Run ./db_updater.py init first.")

    if args.verb == "status":
        status(conn, features)
    elif args.verb == "deploy":
        deploy(conn, features, args.features, args.dry_run)
    elif args.verb == "revert":
        if not args.features:
            raise SystemExit("revert needs at least one feature named explicitly")
        revert(conn, features, args.features, args.dry_run)
    elif args.verb == "verify":
        return verify(conn, features, args.features)
    return 0


if __name__ == "__main__":
    sys.exit(main())
