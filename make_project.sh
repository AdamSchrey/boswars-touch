#!/bin/bash
# Prepare and build boswars-touch for Clickable.
#
# This mirrors draw-on-document's make_project.sh: a project-prep step that is
# part of the build flow itself (NOT a separate clickable prebuild hook). It
# runs from the project root and then invokes the engine's own build system
# (make.py).
#
# make.py resolves its sources relative to the current directory (find('engine',
# ...)), so it MUST be run with CWD = project root, not from clickable's
# build_dir.
#
# Clickable's "custom" builder runs on an amd64 host image that cross-compiles
# to the target arch (e.g. clickable/amd64-ut24.04-1.x-arm64). The target
# libraries live under /usr/lib/${ARCH_TRIPLET}/ and pkg-config .pc files under
# /usr/lib/${ARCH_TRIPLET}/pkgconfig/, but make.py calls plain "g++" and
# "pkg-config" by default, which search the host (amd64) paths. So, when the
# cross-compiler is available, point make.py at it and configure pkg-config for
# the target architecture.
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

# Determine the build directory. Clickable exports BUILD_DIR; fall back to the
# engine's default for direct (non-clickable) invocations.
BUILDDIR="${BUILD_DIR:-fbuild/release}"

# Cross-compilation: when building for a foreign architecture (the normal
# clickable case), use the cross-compiler and tell pkg-config where the target
# .pc files are, so make.py's library detection finds the target libraries.
# ARCH_TRIPLET is exported by clickable; if absent, build natively.
MAKE_ARGS=("release" "builddir=${BUILDDIR}")
if [ -n "${ARCH_TRIPLET:-}" ]; then
    CROSS_CC="${ARCH_TRIPLET}-g++"
    if command -v "${CROSS_CC}" >/dev/null 2>&1; then
        export PKG_CONFIG_LIBDIR="/usr/lib/${ARCH_TRIPLET}/pkgconfig:/usr/share/pkgconfig"
        MAKE_ARGS+=("cc=${CROSS_CC}" "ldflags=-L/usr/lib/${ARCH_TRIPLET}")
        echo "Building boswars (cross-compile: arch=${ARCH}, triplet=${ARCH_TRIPLET}, cc=${CROSS_CC})..."
    else
        echo "Building boswars (triplet=${ARCH_TRIPLET} but ${CROSS_CC} not found; native toolchain)..."
    fi
else
    echo "Building boswars (native)..."
fi

# make.py creates builddir if missing and outputs the 'boswars' binary into it.
python3 make.py "${MAKE_ARGS[@]}"

echo "Build complete."
exit 0
