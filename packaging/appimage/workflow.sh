#!/usr/bin/env -S bash --posix

# Display environmental variables
export -p

# set verbose mode, exit if any commands return a result other than 0
set -ev

if [ -z "$WORKSPACE" ]; then
  echo "This script needs to be called from make-appimage.sh"
  exit 1
fi

if [ -z "$VERSION" ]; then
  echo "VERSION is set from make-appimage.sh or the GitHub workflow."
  exit 1
fi

OUTPUT_DIR="$WORKSPACE/packaging/appimage"
test -d "$OUTPUT_DIR"
APPDIR="$WORKSPACE/packaging/appimage/AppDir"

cd "$WORKSPACE"

if [ -d "$APPDIR" ]; then
  rm -rf "$APPDIR"
fi

DATADIR="$APPDIR/data"

mkdir -p "$DATADIR"

CFLAGS="-DDEFAULT_DATA_PATH=\"~~/../../data/\""
CXXFLAGS="$CFLAGS"
./make.py -j $(nproc) release cflags="${CFLAGS} ${CXXFLAGS}" install_data datadir="${DATADIR}"

cd "$OUTPUT_DIR"

# run the extracted linuxdeploy, which is already in the docker image
"/tools/squashfs-root/AppRun"  \
  -d "$WORKSPACE/packaging/boswars.desktop" \
  --icon-file="$WORKSPACE/packaging/boswars.png" \
  --icon-filename=boswars \
  --executable "$WORKSPACE/fbuild/release/boswars" \
  --appdir "$APPDIR" \
  --output appimage
