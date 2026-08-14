#!/bin/bash
# Prepare and build boswars-touch for Clickable.
#
# This mirrors draw-on-document's make_project.sh: a project-prep step that is
# part of the build flow itself (NOT a separate clickable prebuild hook, which
# would run before clickable creates build_dir). It runs from the project root
# and then invokes the engine's own build system (make.py).
#
# make.py resolves its sources relative to the current directory (find('engine',
# ...)), so it MUST be run with CWD = project root, not from clickable's
# build_dir.
set -e

cd "$(dirname "$0")"

echo "Preparing boswars-touch for Clickable..."

# Remove only the engine's own build cache. Do NOT remove build/ or lib/ here:
# build/ is clickable's build dir (created/entered by the builder) and lib/ is
# populated by import_binaries.sh during postbuild.
rm -rf fbuild .deps

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

# Build the engine. make.py creates builddir if missing and outputs the
# 'boswars' binary into it.
echo "Building boswars..."
python3 make.py release builddir="${BUILD_DIR}"

echo "Build complete."
exit 0
