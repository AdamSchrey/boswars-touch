#!/bin/bash
# Launch boswars inside the click package. The binary and the read-only game
# data are both installed next to this script; -d points the engine at them.
# User data goes to $HOME/.local/share/boswars-touch/ (STRATAGUS_HOME_PATH),
# the confined app-data dir, so no writable-working-directory hack is needed.
case "$0" in
    */*) DIR="${0%/*}" ;;
    *)    DIR="." ;;
esac
"$DIR/boswars" -d "$DIR"
exit 0
