# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

ARTC (Ancestral Relation Tree Creation) is a Qt Widgets / C++ desktop application that
builds a pedigree chart in a MySQL database. The design documents in `documents/` describe
a larger goal — ingest a 23andMe relatives file, infer connections, emit a GEDCOM — but
**none of that is implemented**. What exists is: a login dialog, a "new host" dialog, and
an algorithm that generates a skeleton pedigree tree of placeholder ancestors.

The project has been dormant since 2019 (last commit "pedigree tree"). Treat everything
below as a description of a half-finished prototype, not a maintained codebase.

## Build and run

Built with **qmake + Qt 6.3.1** (Qt 6.4.0 is also installed under `/usr/local/share/QT/`).
There is no CMake, no CI, and **no test suite or linter of any kind** — don't look for one
and don't claim to have run tests.

The committed shadow build directory reproduces what Qt Creator does:

```bash
cd /development/ARTC/build-ARTC-Desktop-Debug
/usr/local/share/QT/6.3.1/gcc_64/bin/qmake -o Makefile ../ARTC/ARTC.pro \
    -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug
make -j$(nproc)
./ARTC
```

`ARTC.pro` declares `QT += core gui sql widgets`. Any new `.cpp`/`.h`/`.ui` must be added
to `SOURCES`/`HEADERS`/`FORMS` in `ARTC/ARTC.pro` by hand.

Running the app immediately opens a modal login dialog and tries to connect; with no
reachable database the app shows a `QMessageBox` error and then continues in a broken
state.

### The Python side is unrelated to the app

`mysql-test.py` + `venv/` (Python 3.10, `mysql-connector-python` 8.0.32) is a standalone
connectivity probe that runs `SHOW DATABASES`. It reads `config.ini` (`localhost`, user
`artc`), which is **not** where the C++ app connects. Changing `config.ini` has no effect
on the application.

```bash
source venv/bin/activate && python mysql-test.py
```

## Database

MySQL, database **`dbArtc`**, host **hardcoded to `172.28.1.5`** in
`DatabaseHelper::createConnection()` (`ARTC/databasehelper.cpp:25`). Only the username and
password come from the login dialog. The design document names a third host
(`192.168.0.22`) — all three addresses in this repo disagree.

**There is no DDL anywhere in the repo** — no `.sql` file, no migration, no dump. If you
need the schema, reconstruct it from the queries in `databasehelper.cpp`:

- **`tblPerson`** — `ID` (PK, auto-increment), `firstName`, `middleName`, `lastName`,
  `gender`, `birthdate`, `fatherId`, `motherId`. Self-referencing adjacency list: each row
  points at its two parents. `gender` and `birthdate` are written but never read back.
- **`tblChild`** — `(parentId, childId)`. The inverse edge, written by `addChild()` and
  **never read**. It duplicates what `tblPerson.fatherId`/`motherId` already holds, and has
  no column recording whether the parent is the father or the mother.
- **`tblHost`** — `(personId)`. Written by `setHost()`, which is **dead code** — nothing
  calls it.

`documents/tblRelativeConstant.ods` defines a fourth table
(`pedConstant`, `pedParentChar`, `pedGrandparentChar`, `pedCousinGeneration`,
`pedSequence`, `pedFatherId`, `pedMotherId`) precomputing every pedigree slot. The code
never touches it. The design doc calls the same idea `tblPedigreeConstant` /
`tblPedigreeTemplate`.

## Architecture

Six classes, no layering — the UI classes call `DatabaseHelper` directly.

```
main.cpp → MainWindow ctor
             ├─ Login (modal)  → username/password
             └─ DatabaseHelper(user, pass).createConnection()

File → New Host  (MainWindow::on_action_New_Host_triggered)
             ├─ Hostdlg (modal) → Person::Individual (name, birthdate, sex)
             ├─ addPerson(host); getPersonId(host)
             ├─ addPerson(FATHER/<hostId>), addPerson(MOTHER/<hostId>)
             ├─ getFatherId/getMotherId by name → addParents(host, f, m)
             └─ Pedigree::createPedigree(host, dbHelper)
                     ├─ createSide('F', fatherId, dh)
                     └─ createSide('M', motherId, dh)
```

`Person` is not really a class — it's a namespace for the `Person::Individual` POD struct.
`person.cpp` is an empty constructor.

### The placeholder-ancestor scheme

Ancestors that aren't known yet are inserted as real `tblPerson` rows whose *name encodes
their position in the tree*. `Pedigree::createSide()` builds:

```
firstName = <side><GF|GM><generation><sequence>-<rootId>   e.g. "FGF11-5"
lastName  = <rootId>                                        e.g. "5"
```

The parent of a just-created node is then found by looking that name back up
(`getFatherId(firstName, lastName)`). This is why names are the lookup key rather than IDs.

`createSide()` is a breadth-first walk using two hand-rolled linked lists (`head`/`tail`
for the current generation, `head1`/`tail1` for the next, swapped by `copyLinkList()`),
running to `MAX_GENERATION = 7` (`pedigree.h`). Each node inserts two ancestors, so one
call writes ~126 placeholder people per side, ~252 per host, each costing 3 INSERTs and
2 SELECTs. There is no batching or transaction.

Note the code comment says "up to five generations" while the constant is 7, and the design
doc says "up to 4th cousin" — the three don't agree. Decide which is intended before
touching this.

## Known defects to be aware of

These are pre-existing; mention them rather than silently working around them.

- **SQL injection.** `addPerson`, `getPersonId`, `getFatherId`, `getMotherId`, `addMother`
  and `addFather` concatenate user input into SQL strings. A surname with an apostrophe
  breaks the app. The *other half* of the same file (`setHost`, `addParents`, `addChild`,
  `getPerson`) already uses `prepare()` + `bindValue()` — follow that pattern, and prefer
  converting the string-built queries over adding new ones.
- **Ambiguous lookups.** `getPersonId()` matches on first+middle+last name only, and the
  `while (query.next())` loops keep the *last* row, so two relatives sharing a name
  silently collapse into one.
- **Uninitialized returns.** `getPersonId`, `getFatherId`, `getMotherId`,
  `getChildsFatherId`, `getChildsMotherId` declare `int id;` and return it unset when the
  query matches nothing.
- **Sequence formatting mismatch.** `createSide()` emits `QString::number(sequence)` ("1"),
  but `tblRelativeConstant` and the design doc specify a zero-padded two-digit sequence
  ("01"). Generated names will not match the constant table if it is ever wired up.
- `Person::Individual` initializes `QString` members with `= NULL`, which warns under Qt 6.

## Repository hygiene

- **`build-ARTC-Desktop-Debug/` is committed to git** — 22 of the 57 tracked files are
  object files, the `Makefile`, moc/uic output, and the `ARTC` binary. There is **no
  `.gitignore`**. Most of the working-tree churn you will see (`M`/`D` on `.o` files, on
  `Makefile`, on `ARTC.pro.user`) is build noise, not real changes. Do not commit build
  output; adding a `.gitignore` and untracking that directory would be a genuine
  improvement.
- `config.ini` is tracked and contains a password (`artc`/`artc`).
- `documents/spec.md` is a stub containing the text "This is a test" — the real
  specification is `documents/ARTC.odt` (requirements) and `documents/ARTC design.odt`
  (design), with diagrams in the `.odg` files.
