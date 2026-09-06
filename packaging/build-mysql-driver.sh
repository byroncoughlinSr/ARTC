#!/usr/bin/env bash
#
# Build Qt's MySQL SQL driver plugin against the system MySQL client library.
#
#   ./build-mysql-driver.sh [qt-version]      # default 6.4.0
#
# Why this is needed: the plugin shipped with the Qt installer is linked
# against libmysqlclient.so.21 (MySQL 8.0). Ubuntu 26.04 ships
# libmysqlclient24, and no package provides soname 21, so the shipped plugin
# cannot be loaded and QMYSQL is unavailable. Rebuilding it from the Qt
# sources against the installed client library fixes that.
#
# Prerequisites (one apt install):
#   sudo apt install libmysqlclient-dev cmake ninja-build
#
# The plugin directory under /usr/local/share/QT is user-owned, so installing
# the result needs no privileges.

set -euo pipefail

QT_VERSION=${1:-6.4.0}
QT_ROOT=/usr/local/share/QT/$QT_VERSION
QT_BIN=$QT_ROOT/gcc_64/bin
PLUGIN_DIR=$QT_ROOT/gcc_64/plugins/sqldrivers
SRC=$QT_ROOT/Src/qtbase/src/plugins/sqldrivers

fail() { echo "error: $*" >&2; exit 1; }

[ -x "$QT_BIN/qt-cmake" ] || fail "no qt-cmake for Qt $QT_VERSION at $QT_BIN"
[ -d "$SRC" ] || fail "no sqldrivers sources at $SRC (re-run the Qt installer with sources)"
[ -f /usr/include/mysql/mysql.h ] || fail "mysql.h not found. Run: sudo apt install libmysqlclient-dev"
command -v cmake >/dev/null || fail "cmake not found. Run: sudo apt install cmake"
command -v ninja >/dev/null || fail "ninja not found. Run: sudo apt install ninja-build"

client_lib=$(ls /usr/lib/*/libmysqlclient.so 2>/dev/null | head -1)
[ -n "$client_lib" ] || fail "libmysqlclient.so not found. Run: sudo apt install libmysqlclient-dev"

build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT

echo "Building the MySQL driver for Qt $QT_VERSION against $client_lib"
"$QT_BIN/qt-cmake" -G Ninja -S "$SRC" -B "$build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DMySQL_INCLUDE_DIR=/usr/include/mysql \
    -DMySQL_LIBRARY="$client_lib" >/dev/null

cmake --build "$build" --target QMYSQLDriverPlugin >/dev/null

plugin=$(find "$build" -name libqsqlmysql.so | head -1)
[ -n "$plugin" ] || fail "the build produced no libqsqlmysql.so"

if [ -f "$PLUGIN_DIR/libqsqlmysql.so" ] && [ ! -f "$PLUGIN_DIR/libqsqlmysql.so.shipped" ]; then
    cp "$PLUGIN_DIR/libqsqlmysql.so" "$PLUGIN_DIR/libqsqlmysql.so.shipped"
    echo "Kept the shipped plugin as libqsqlmysql.so.shipped"
fi

install -m 755 "$plugin" "$PLUGIN_DIR/libqsqlmysql.so"
echo "Installed $PLUGIN_DIR/libqsqlmysql.so"
echo "  now links: $(objdump -p "$PLUGIN_DIR/libqsqlmysql.so" | awk '/NEEDED/ && /mysql/ {print $2}')"
