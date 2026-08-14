#!/bin/bash
# Launch boswars inside the click package.
# The binary and the game data (scripts/, maps/, ...) are both installed into
# the same directory as this script, so run from here and point the engine at
# the current directory via -d (see DEFAULT_DATA_PATH in engine/include/stratagus.h).
cd "$(dirname "$0")"
./boswars -d .
exit 0
