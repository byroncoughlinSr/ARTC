# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

ARTC (Ancestral Relation Tree Creation) is a Qt Widgets / C++ desktop application that
builds a pedigree chart in a MySQL database. The design documents in `documents/` describe
a larger goal — ingest a 23andMe relatives file, infer how each match connects, emit a
GEDCOM — and **none of that is implemented**. What exists is account registration and
sign-in, a "new host" dialog, and an algorithm that generates a skeleton pedigree tree of
placeholder ancestors.

The project was dormant from 2019 until 2026. Treat it as a prototype under active
reconstruction, not a mature codebase.

## Build and run

**qmake + Qt 6.4.0.** Use 6.4.0, not the 6.3.1 also installed under
`/usr/local/share/QT/`: only 6.4.0 has a working MySQL driver here (see below). There is no
CMake, no CI, and **no test suite or linter** — don't look for one, and don't claim to have
run tests.

```bash
cd build-ARTC-Desktop-Debug
/usr/local/share/QT/6.4.0/gcc_64/bin/qmake -o Makefile ../ARTC/ARTC.pro \
    -spec linux-g++ CONFIG+=debug
make -j$(nproc)
./ARTC
```

New `.cpp`/`.h`/`.ui` files must be added to `SOURCES`/`HEADERS`/`FORMS` in `ARTC/ARTC.pro`
by hand. `ARTC.pro` declares `QT += core gui sql widgets` and `LIBS += -lcrypto`.

### Known environment blocker: the MySQL driver will not load

`libqsqlmysql.so` in **both** installed Qt versions is linked against
`libmysqlclient.so.21`, and Ubuntu 26.04 ships `libmysqlclient24`. The plugin therefore
fails to `dlopen`, and the application cannot reach the database at all. The schema tooling
is unaffected — it shells out to the `mysql` client.

`QSqlDatabase::isDriverAvailable("QMYSQL")` returns **true** even then, because it only
reports that Qt knows the driver's name. `createConnection()` checks `db.isValid()` after
`addDatabase()` instead, which is the only reliable signal. Diagnose with
`QT_DEBUG_PLUGINS=1`.

## Database

MySQL, database `dbArtc`. Connection settings come from the `[connector_artc]` section of
`config.ini`, resolved from `$ARTC_CONFIG`, then beside the executable, then one directory
up. The same file, and the same section names, are read by `mysql-test.py` as a
`mysql.connector` option file — keep them compatible.

The schema lives in `database/` and is applied with `database/scripts/db_updater.py`; see
`database/README.md`. **Add a feature there rather than hand-writing DDL against a live
database.**

| Table        | Notes |
| ------------ | ----- |
| `tblPerson`  | Every individual. Self-referencing adjacency list via `fatherId`/`motherId`. `gender` and `birthdate` are written but never read back. |
| `tblAccount` | Sign-in accounts. `personId` is null until the holder creates a host. |
| `tblChild`   | The inverse parent→child edge, written by `addChild()` and **never read**. Duplicates `tblPerson.fatherId`/`motherId`, and does not record which parent it was. |
| `tblHost`    | Written by `setHost()`, which is **dead code** — nothing calls it. |

`documents/tblRelativeConstant.ods` defines a further table precomputing every pedigree
slot. The code never touches it; the design doc calls the same idea `tblPedigreeConstant` /
`tblPedigreeTemplate`.

## Architecture

`MainWindow` is a screen router: a `QStackedWidget` holding Home → Sign In → Register →
Workspace. The menu bar, tool bar and status bar belong to the workspace and stay hidden
until sign-in succeeds. Screens are built in C++, not `.ui` files — they are
stylesheet-driven, which Designer previews poorly.

```
main.cpp  → applies styles/artc.qss → MainWindow

HomeWidget ──┬─ signInRequested  → SignInWidget
             └─ registerRequested → RegisterWidget
                                      │ registrationSubmitted
                                      ▼
                       MainWindow::handleRegistration
                            ensureConnected() → AccountRepository::create
                                      │
SignInWidget ── signInRequested ─────►│
                       MainWindow::attemptSignIn
                            ensureConnected() → AccountRepository::authenticate
                                      ▼
                                 Workspace page
                                      │  File → New Host
                                      ▼
                       Hostdlg → DatabaseHelper → Pedigree::createPedigree
```

**Never position widgets at fixed pixel coordinates.** The application stylesheet pads
inputs, so a hardcoded 24px-tall `QDateEdit` clips its own text. Every screen and dialog
uses layouts and sizes itself from its content. `mainwindow.ui` is the only remaining `.ui`
file, and it carries no positioned widgets.

**Word-wrapped labels must be `WrappingLabel`** (`uitheme.h`), not `QLabel`. A wrapped
QLabel reports about one line as its minimum size, so the fixed-width cards collapse around
it and clip the remaining lines. `WrappingLabel` pins its minimum height to
`heightForWidth()` at the card's content width. Correcting `minimumSizeHint()` alone is not
enough — a layout that has already sized the widget will not revisit it.

**Two database layers, deliberately not merged.** `AccountRepository` (accounts) uses
prepared statements throughout. `DatabaseHelper` (pedigree) is legacy and builds half its
queries by concatenation. Do not extend the concatenation style onto new code.

The connection is opened once, lazily, with the application's own account from
`config.ini`; end users are authenticated against `tblAccount`, not against MySQL. Signing
out clears the session and leaves the connection open.

Password hashing is Argon2id via OpenSSL's KDF (`PasswordHasher`), stored in PHC format
(`$argon2id$v=19$m=65536,t=3,p=1$…`) so the cost travels with the hash and can be raised
later. `needsRehash()` drives a transparent upgrade on the next successful sign-in.

### The placeholder-ancestor scheme

Unknown ancestors are inserted as real `tblPerson` rows whose *name encodes their position
in the tree*. `Pedigree::createSide()` builds:

```
firstName = <side><GF|GM><generation><sequence>-<rootId>   e.g. "FGF11-5"
lastName  = <rootId>                                        e.g. "5"
```

The parent of a just-created node is then found by looking that name back up
(`getFatherId(firstName, lastName)`) — which is why names, not IDs, are the lookup key.

`createSide()` is a breadth-first walk over two hand-rolled linked lists, running to
`MAX_GENERATION = 7` (`pedigree.h`). Each node inserts two ancestors, so one call writes
~126 placeholders per side, ~252 per host, each costing 3 INSERTs and 2 SELECTs, with no
batching or transaction.

The code comment says "up to five generations", the constant is 7, and the design doc says
"up to 4th cousin". Decide which is intended before touching this.

## Known defects

Pre-existing; mention them rather than silently working around them.

- **SQL injection in `DatabaseHelper`.** `addPerson`, `getPersonId`, `getFatherId`,
  `getMotherId`, `addMother` and `addFather` concatenate input into SQL. A surname with an
  apostrophe breaks the app. The other half of the same file already uses `prepare()` +
  `bindValue()` — follow that, and prefer converting the string-built queries to adding
  more.
- **Ambiguous lookups.** `getPersonId()` matches on name alone and its `while
  (query.next())` loop keeps the *last* row, so relatives sharing a name collapse into one.
- **Uninitialized returns.** The `getXId` functions declare `int id;` and return it unset
  when nothing matches.
- **Sequence formatting mismatch.** `createSide()` emits `QString::number(sequence)` ("1"),
  but `tblRelativeConstant` and the design doc specify zero-padded two digits ("01").
- `Person::Individual` initializes `QString` members with `= NULL`; `DatabaseHelper` is
  copied by value in `mainwindow.cpp` and `pedigree.cpp`, forcing a copy of its
  `QSqlQuery` member. Both warn under Qt 6.

## Repository notes

- `build-ARTC-Desktop-Debug/` and `config.ini` are **not** tracked. Build output was
  committed until 2026 and had to be purged: the tracked `ui_*.h` were generated by uic
  5.9.5 and the `.o` files built against Qt 5, which broke the build rather than helping
  it.
- `documents/spec.md` is a stub reading "This is a test". The real specification is
  `documents/ARTC.odt` (requirements) and `documents/ARTC design.odt` (design), with
  diagrams in the `.odg` files.
- `packaging/` holds the application icon and desktop entry; `install-launcher.sh`
  installs both for the current user.
