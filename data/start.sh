#!/bin/bash
# Launch boswars inside the click package.
#
# The binary and the read-only game data (scripts/, maps/, ...) are both
# installed into the same directory as this script. The engine is pointed at
# that directory via -d (see DEFAULT_DATA_PATH in engine/include/stratagus.h).
#
# boswars writes all mutable state (saves, logs, patches, preferences) to
# $HOME/.boswars/ via its own CreateUserDirectories(), so it does NOT need a
# writable working directory and no cache-directory hack is required here.
#
# Determine the script's directory using only bash builtins, so that no extra
# binaries (cd/dirname) need to be bundled just to locate the install dir.
case "$0" in
    */*) DIR="${0%/*}" ;;     # launched with a path, absolute or relative
    *)    DIR="." ;;          # launched by bare name from its own directory
esac
"$DIR/boswars" -d "$DIR"
exit 0
