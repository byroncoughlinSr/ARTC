#!/usr/bin/env bash
#
# Install the ARTC icon and desktop entry for the current user.
#
#   ./install-launcher.sh [--desktop] [path/to/ARTC]
#
# The binary path defaults to the shadow build directory. --desktop also drops
# a copy on ~/Desktop and marks it executable, which is what Plasma requires
# before it will launch an entry placed there.

set -euo pipefail

here=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
data_home=${XDG_DATA_HOME:-$HOME/.local/share}
icon_dir="$data_home/icons/hicolor/scalable/apps"
app_dir="$data_home/applications"

on_desktop=false
binary=""

for arg in "$@"; do
    case "$arg" in
        --desktop) on_desktop=true ;;
        *)         binary=$arg ;;
    esac
done

if [ -z "$binary" ]; then
    binary="$(cd "$here/.." && pwd)/build-ARTC-Desktop-Debug/ARTC"
fi

if [ ! -x "$binary" ]; then
    echo "No ARTC binary at $binary" >&2
    echo "Build it first, or pass its path: $0 /path/to/ARTC" >&2
    exit 1
fi

mkdir -p "$icon_dir" "$app_dir"
install -m 644 "$here/artc.svg" "$icon_dir/artc.svg"
sed "s|@EXEC@|$binary|" "$here/artc.desktop" >"$app_dir/artc.desktop"
chmod 644 "$app_dir/artc.desktop"

echo "Installed $icon_dir/artc.svg"
echo "Installed $app_dir/artc.desktop"

if [ "$on_desktop" = true ]; then
    desktop_dir=$(xdg-user-dir DESKTOP 2>/dev/null || echo "$HOME/Desktop")
    mkdir -p "$desktop_dir"
    cp "$app_dir/artc.desktop" "$desktop_dir/artc.desktop"
    chmod 755 "$desktop_dir/artc.desktop"
    echo "Installed $desktop_dir/artc.desktop"
fi

# Refresh the desktop and icon caches. Each is best-effort: a missing tool
# only means the entry shows up after the next login instead of immediately.
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database "$app_dir" 2>/dev/null || true
fi
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t "$data_home/icons/hicolor" 2>/dev/null || true
fi
for sycoca in kbuildsycoca6 kbuildsycoca5; do
    if command -v "$sycoca" >/dev/null 2>&1; then
        "$sycoca" --noincremental >/dev/null 2>&1 || true
        break
    fi
done
