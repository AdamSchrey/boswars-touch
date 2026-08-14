#!/bin/bash
# Build command for clickable's custom builder (see clickable.json).
# Run via: CLICKABLE_FRAMEWORK=... CLICKABLE_ARCH=... clickable
set -e

cd "$(dirname "$0")"

echo "Preparing boswars-touch for Clickable..."

# Do NOT remove build/ or lib/: build/ is clickable's build dir, lib/ is
# populated by import_binaries.sh below and consumed by install_lib.
rm -rf fbuild .deps

for f in data/manifest.json data/boswars-touch.desktop \
         data/boswars-touch.apparmor data/start.sh; do
    [ -f "$f" ] || { echo "ERROR: required packaging file '$f' is missing." >&2; exit 1; }
done
chmod +x data/start.sh

echo "Project prepared for Clickable."

# make.py must run with CWD = project root (find('engine', ...) is relative).
# Cross-compile with the target-arch toolchain: make.py defaults to plain
# g++/pkg-config which search host paths, not /usr/lib/${ARCH_TRIPLET}/.
export PKG_CONFIG_LIBDIR="/usr/lib/${ARCH_TRIPLET}/pkgconfig:/usr/share/pkgconfig"

echo "Building boswars (arch=${ARCH}, triplet=${ARCH_TRIPLET})..."
python3 make.py release \
    builddir="${BUILD_DIR}" \
    cc="${ARCH_TRIPLET}-g++" \
    ldflags="-L/usr/lib/${ARCH_TRIPLET}"

echo "Build complete."

# Must run during the build, not as a clickable postbuild hook: clickable
# processes install_lib BEFORE postbuild, so lib/ must already exist.
bash "${ROOT}/import_binaries.sh"

echo "Import complete."
exit 0
