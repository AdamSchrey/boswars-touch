#!/bin/bash
# Import runtime shared libraries and helper programs into the click package.
set -Eeou pipefail

echo "ARCH: ${ARCH}"
cd "${ROOT}"

echo -e "\n\nImport Libraries:\n"
mkdir -p lib/${ARCH_TRIPLET}/

cp /usr/lib/${ARCH_TRIPLET}/libSDL2* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/liblua5.1.so* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libpng16* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libvorbis* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libtheora* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libogg* lib/${ARCH_TRIPLET}/ || true
cp /usr/lib/${ARCH_TRIPLET}/libz.so* lib/${ARCH_TRIPLET}/ || true

# Download any libraries not found above and extract them.
for pkg in libsdl2-2.0-0 liblua5.1-0 libpng16-16 libvorbis0a libtheora0 libogg0 zlib1g; do
    if ! ls lib/${ARCH_TRIPLET}/lib${pkg}* 1> /dev/null 2>&1; then
        echo "Downloading $pkg for architecture $ARCH..."
        apt download ${pkg}:${ARCH} 2>/dev/null || true
        deb=$(ls | grep "^${pkg}" || true)
        if [ -n "$deb" ] && [ -f "$deb" ]; then
            echo "Extracting $deb..."
            dpkg-deb -xv "$deb" . || true
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
        dpkg-deb -xv "$libc" . || true
        cp -r usr/lib/* lib/ || true
        rm -f "$libc"
        rm -rf usr etc
    fi
fi

rm -f *.deb

echo -e "\n\nImport Programs:\n"
# Bundle basic coreutils so they are available inside the confined click.
for pkg in coreutils grep; do
    echo "Downloading $pkg for architecture $ARCH..."
    apt download ${pkg}:${ARCH} 2>/dev/null || true
    deb=$(ls | grep "^${pkg}" || true)
    if [ -n "$deb" ] && [ -f "$deb" ]; then
        echo "Extracting $deb..."
        dpkg-deb -xv "$deb" . || true
        if [ -d "usr/bin" ]; then
            for bin in cp ls mkdir mv echo rm tr basename mktemp grep; do
                [ -f "usr/bin/$bin" ] && cp usr/bin/$bin data/ || true
            done
        fi
        rm -f "$deb"
        rm -rf usr etc
    fi
done

rm -f *.deb

echo -e "\n\nLibrary and program import completed successfully."
exit 0
