# ARTC database

The schema for `dbArtc`, deployed one feature at a time in dependency order.

The layout follows the SenseCraft server's approach (`deploy/`, `revert/`,
`verify/`, plus `initial_setup.py` and `db_updater.py`), adapted from
PostgreSQL to MySQL.

## First time

```bash
cd database/scripts
./initial_setup.py          # create the database and the application account
./db_updater.py init        # create the deploy_state table
./db_updater.py deploy      # apply the schema
```

`initial_setup.py` needs administrative access to MySQL. On a stock Ubuntu
install the root account authenticates through the unix socket, so the default
is `sudo`; the script must therefore be run from a terminal that can prompt.
Use `-u root -p <password> --no-sudo` to connect with a password instead.

It writes the host, port, database, user and generated password into
`../config.ini` (mode 600). That is the same file, and the same
`[connector_artc]` section, that the Qt application reads through `QSettings`
and that `mysql-test.py` reads as a `mysql.connector` option file.

## Day to day

```bash
./db_updater.py status          # what is deployed, and what has drifted
./db_updater.py deploy          # apply everything outstanding
./db_updater.py deploy host     # apply one feature and its requirements
./db_updater.py deploy -n       # dry run
./db_updater.py verify          # check the deployed shape
./db_updater.py revert host     # drop a feature and anything that needs it
```

## Features

| Feature   | Table        | Requires |
| --------- | ------------ | -------- |
| `person`  | `tblPerson`  | —        |
| `account` | `tblAccount` | `person` |
| `child`   | `tblChild`   | `person` |
| `host`    | `tblHost`    | `person` |

`tblPerson` is a self-referencing adjacency list: `fatherId` and `motherId`
point back at `tblPerson.ID`. `middleName`, `gender`, `birthdate`, `fatherId`
and `motherId` are all nullable, because `DatabaseHelper::addMother` and
`addFather` insert placeholders carrying only a first and last name.

`tblAccount` backs registration and sign-in. Its `passwordHash` holds a
PHC-format Argon2id string that carries its own cost parameters, so the work
factor can be raised later without invalidating existing hashes. `personId` is
null until the account holder creates a host.

`tblChild` and `tblHost` are created because the application writes to them,
not because anything reads them back — see the schema notes in `CLAUDE.md`.

## Adding a feature

Create `deploy/<name>.sql`, `revert/<name>.sql` and `verify/<name>.sql`. The
deploy file declares its dependencies and whether it can be re-applied:

```sql
-- Deploy artc:<name> to mysql
-- requires: person
-- idempotent
```

`db_updater.py` topologically sorts on `requires`, so ordering is derived
rather than declared, and reverting a feature reverts its dependants first.

The tables the design documents describe but the code does not yet use —
`tblDNA` for the 23andMe relatives export, and `tblRelativeConstant` from
`documents/tblRelativeConstant.ods` — belong here as new features when they
are implemented.

## How this differs from the SenseCraft original

Both differences are forced by MySQL or by this project's stage, not chosen:

* **Deploys are not atomic.** MySQL commits implicitly on DDL, so a deploy
  file cannot be wrapped in a transaction the way the PostgreSQL originals
  are. Feature files are written to be idempotent instead (`CREATE TABLE IF
  NOT EXISTS`), and state is recorded per feature, so an interrupted run is
  resumed by running `deploy` again.
* **No versioned files or migrations.** The original supports
  `feature-1.sql`, `feature-1M.sql` and a `deploy/attic` for migrating a
  deployed schema forward. There is no deployed history here to migrate from
  yet, so that machinery is left out until there is.

There is also one database rather than the original's production/development/
test trio, because `config.ini` has a single `[connector_artc]` section that
the application and `mysql-test.py` both depend on. `--database` overrides the
name if a second one is wanted.
