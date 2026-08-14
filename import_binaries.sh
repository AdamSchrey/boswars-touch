#!/bin/bash
# Import the shared libraries and helper programs that boswars-touch needs at
# runtime into the click package. Mirrors the draw-on-document import_binaries.sh:
# libraries are copied from the build container and, where that is not enough,
# downloaded with "apt download" and extracted.
set -Eeou pipefail

echo "ARCH: ${ARCH}"
cd "${ROOT}"

echo -e "\n\nImport Libraries:\n"
mkdir -p lib/${ARCH_TRIPLET}/

# Some libraries are installed via "dependencies_target" in clickable.json,
# but that does not always bundle every needed .so, so the rest are obtained
# with "apt download". Downloading some libraries with "dependencies_target"
# runs "apt-get update" in the container, which makes "apt download" usable.
cp /usr/lib/${ARCH_TRIPLET}/libSDL2* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/liblua5.1.so* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libpng16* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libvorbis* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libtheora* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libogg* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libz.so* lib/${ARCH_TRIPLET}/ || true

# Download any libraries that were not found above and extract them.
for pkg in libsdl2-2.0-0 liblua5.1-0 libpng16-16 libvorbis0a libtheora0 libogg0 zlib1g; do
    if ! ls lib/${ARCH_TRIPLET}/lib${pkg}* 1> /dev/null 2>&1; then
        echo "Downloading $pkg for architecture $ARCH..."
        apt download ${pkg}:${ARCH} 2>/dev/null || true
        deb=$(ls | grep "^${pkg}" || true)
        if [ -n "$deb" ] && [ -f "$deb" ]; then
            echo "Extracting $deb..."
            dpkg-deb -xv "$deb" . || true  # ignore tar ownership errors
            if [ -d "usr/lib/${ARCH_TRIPLET}" ]; then
                cp -r usr/lib/${ARCH_TRIPLET}/* lib/${ARCH_TRIPLET}/ || true
            fi
            rm -f "$deb"
            rm -rf usr etc
        fi
    fi
done

# libc6 is required for dynamic linking of the bundled libraries.
if ! ls lib/${ARCH_TRIPLET}/libc.so* 1> /dev/null 2>&1; then
    echo "Downloading libc6 for architecture $ARCH..."
    apt download libc6:${ARCH} 2>/dev/null || true
    libc=$(ls | grep "^libc6" || true)
    if [ -n "$libc" ] && [ -f "$libc" ]; then
        echo "Extracting $libc..."
        dpkg-deb -xv "$libc" . || true  # ignore tar ownership errors
        cp -r usr/lib/* lib/ || true
        rm -f "$libc"
        rm -rf usr etc
    fi
fi

# Clean up any leftover .deb files.
rm -f *.deb

echo -e "\n\nImport Programs:\n"
# boswars may shell out to basic coreutils programs at runtime; bundle them
# into data/ so they are available inside the confined click package.
for pkg in coreutils grep; do
    echo "Downloading $pkg for architecture $ARCH..."
    apt download ${pkg}:${ARCH} 2>/dev/null || true
    deb=$(ls | grep "^${pkg}" || true)
    if [ -n "$deb" ] && [ -f "$deb" ]; then
        echo "Extracting $deb..."
        dpkg-deb -xv "$deb" . || true  # ignore tar ownership errors
        if [ -d "usr/bin" ]; then
            for bin in cp ls mkdir mv echo rm tr basename mktemp grep; do
                if [ -f "usr/bin/$bin" ]; then
                    cp usr/bin/$bin data/ || true
                fi
            done
        fi
        rm -f "$deb"
        rm -rf usr etc
    fi
done

rm -f *.deb

echo -e "\n\nLibrary and program import completed successfully."
exit 0
