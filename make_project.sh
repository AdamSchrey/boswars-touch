#!/bin/bash
# Prepare boswars-touch for a Clickable build.
# Mirrors the draw-on-document make_project.sh: clean stale build output and
# make sure the data/ directory that ships the click package is in place.
set -e

cd "$(dirname "$0")"

echo "Preparing boswars-touch for Clickable..."

# Remove stale build artifacts (engine uses fabricate, output goes to fbuild/).
rm -rf fbuild build lib .deps

# The game assets and click packaging files (manifest.json, .desktop,
# .apparmor, start.sh) are committed under data/, so nothing needs to be
# generated here. Verify the expected files are present.
for f in data/manifest.json data/boswars-touch.desktop \
         data/boswars-touch.apparmor data/start.sh; do
    if [ ! -f "$f" ]; then
        echo "ERROR: required packaging file '$f' is missing." >&2
        exit 1
    fi
done

# Ensure the launcher is executable in the working tree.
chmod +x data/start.sh

echo "Project prepared for Clickable."
exit 0
