#!/bin/bash
# Prepare and build boswars-touch for Clickable.
#
# This script is the "build" command of clickable's custom builder (see
# clickable.json). It runs from the project root and invokes the engine's own
# build system (make.py). It is NOT meant to be run manually; use
#   CLICKABLE_FRAMEWORK=... CLICKABLE_ARCH=... clickable
#
# make.py resolves its sources relative to the current directory (find('engine',
# ...)), so it MUST be run with CWD = project root, not from clickable's
# build_dir.
#
# Clickable's custom builder runs on an amd64 host image that cross-compiles to
# the target arch (e.g. clickable/amd64-ut24.04-1.x-arm64). The target
# libraries live under /usr/lib/${ARCH_TRIPLET}/ and pkg-config .pc files under
# /usr/lib/${ARCH_TRIPLET}/pkgconfig/, but make.py calls plain "g++" and
# "pkg-config" by default, which search the host (amd64) paths. So point make.py
# at the cross-compiler and configure pkg-config for the target architecture.
# Clickable always exports ARCH_TRIPLET and BUILD_DIR.
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

# Cross-compile: use the target-arch compiler and tell pkg-config where the
# target .pc files are, so make.py's library detection finds the target libs.
export PKG_CONFIG_LIBDIR="/usr/lib/${ARCH_TRIPLET}/pkgconfig:/usr/share/pkgconfig"

echo "Building boswars (arch=${ARCH}, triplet=${ARCH_TRIPLET}, cc=${ARCH_TRIPLET}-g++)..."
# make.py creates builddir if missing and outputs the 'boswars' binary into it.
# cc:        the cross-compiler
# ldflags:   -L so the linker finds -lz, -lpng, etc. under the target lib dir
python3 make.py release \
    builddir="${BUILD_DIR}" \
    cc="${ARCH_TRIPLET}-g++" \
    ldflags="-L/usr/lib/${ARCH_TRIPLET}"

echo "Build complete."
exit 0
