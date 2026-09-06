#!/usr/bin/env python3
"""
Shared helpers for the ARTC database scripts.

Modelled on the SenseCraft server scripts, adapted to MySQL: connections go
through the mysql client rather than psql, and the password is passed in the
environment so it never appears in the process list.
"""

import os
from configparser import ConfigParser
from pathlib import Path
from subprocess import run

# Both the Qt application (via QSettings) and mysql-test.py (via
# mysql.connector option files) read this section, so the name is fixed.
CONFIG_SECTION = "connector_artc"
DEFAULT_DATABASE = "dbArtc"


def repo_root() -> Path:
    """The ARTC checkout root, two levels above this file."""
    return Path(__file__).resolve().parents[2]


def config_path() -> Path:
    """config.ini, which lives at the checkout root beside the .pro file."""
    return repo_root() / "config.ini"


def read_config() -> ConfigParser:
    """Load config.ini, returning an empty parser if it does not exist yet."""
    parser = ConfigParser()
    path = config_path()
    if path.exists():
        parser.read(path)
    if not parser.has_section(CONFIG_SECTION):
        parser.add_section(CONFIG_SECTION)
    return parser


def write_config(parser: ConfigParser) -> Path:
    """Write config.ini back, readable only by its owner: it holds a password."""
    path = config_path()
    with open(path, "w") as handle:
        parser.write(handle)
    os.chmod(path, 0o600)
    return path


def app_connection(parser: ConfigParser = None) -> dict:
    """Connection arguments for the application's own database account."""
    parser = parser or read_config()
    section = parser[CONFIG_SECTION]
    return dict(
        user=section.get("user"),
        password=section.get("password"),
        host=section.get("host", "localhost"),
        port=int(section.get("port", 3306)),
        database=section.get("database", DEFAULT_DATABASE),
    )


def mysql_command(
    sql=None,
    sqlfile=None,
    user=None,
    password=None,
    host="localhost",
    port=3306,
    database=None,
    sudo=False,
    debug=False,
):
    """
    Run one statement, or one file, through the mysql client.

    Pass sudo=True to connect as the operating system's root account, which is
    how a stock Ubuntu MySQL authenticates an administrator (auth_socket).

    Returns stdout. Raises RuntimeError carrying the client's stderr on failure.
    """
    if sudo:
        conn = ["sudo", "mysql"]
    else:
        conn = ["mysql", "-u", user]
        if host and host not in ("localhost", ""):
            conn.extend(["-h", host, "--protocol=TCP"])

    if port and int(port) != 3306:
        conn.extend(["-P", str(port)])
    conn.extend(["--batch", "--raw", "--skip-column-names"])
    if database:
        conn.append(database)

    stdin = None
    if sql:
        conn.extend(["-e", sql])
    elif sqlfile:
        stdin = Path(sqlfile).read_text()
    else:
        raise ValueError("mysql_command needs either sql or sqlfile")

    # MYSQL_PWD keeps the password out of the process list, unlike -p<password>.
    env = dict(os.environ)
    if password and not sudo:
        env["MYSQL_PWD"] = password
    else:
        env.pop("MYSQL_PWD", None)

    if debug:
        print("  $", " ".join(conn if not sql else conn[:-1] + ["-e", sql[:80]]))

    result = run(conn, input=stdin, capture_output=True, encoding="utf-8", env=env)
    if result.returncode != 0:
        raise RuntimeError((result.stderr or result.stdout).strip())
    return result.stdout
