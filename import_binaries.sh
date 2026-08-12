#!/bin/bash

set -Eeou pipefail

echo "ARCH: ${ARCH}"

cd "${ROOT}"

echo -e "\n\nImport Libraries:\n"

mkdir -p lib/${ARCH_TRIPLET}/

# Some libraries are downloaded with "dependencies_target" under clickable.json,
# but this does not work well for all libraries, so the rest will be downloaded with "apt download".
# Downloading some libraries with "dependencies_target" updates the docker container with
# "apt-get update", so it is possible to use "apt download" afterwards.

# Copy SDL2 libraries
if [ -f "/usr/lib/${ARCH_TRIPLET}/libSDL2.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libSDL2* lib/${ARCH_TRIPLET}/ || true
fi

# Copy Lua libraries
if [ -f "/usr/lib/${ARCH_TRIPLET}/liblua5.1.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/liblua5.1* lib/${ARCH_TRIPLET}/ || true
fi

# Copy other libraries
if [ -f "/usr/lib/${ARCH_TRIPLET}/libpng16.so.16" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libpng16* lib/${ARCH_TRIPLET}/ || true
fi

if [ -f "/usr/lib/${ARCH_TRIPLET}/libvorbis.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libvorbis* lib/${ARCH_TRIPLET}/ || true
fi

if [ -f "/usr/lib/${ARCH_TRIPLET}/libtheora.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libtheora* lib/${ARCH_TRIPLET}/ || true
fi

if [ -f "/usr/lib/${ARCH_TRIPLET}/libogg.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libogg* lib/${ARCH_TRIPLET}/ || true
fi

if [ -f "/usr/lib/${ARCH_TRIPLET}/libz.so.1" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libz* lib/${ARCH_TRIPLET}/ || true
fi

# Download additional dependencies if needed and extract libraries
for lib in libsdl2-2.0-0 liblua5.1-0 libpng16-16 libvorbis0a libtheora0 libogg0 zlib1g; do
    if ! ls lib/${ARCH_TRIPLET}/lib${lib}*.so* 1> /dev/null 2>&1; then
        echo "Downloading $lib for architecture $ARCH..."
        apt download ${lib}:${ARCH} 2>/dev/null || true
        pkg=$(ls | grep "${lib}" || true)
        if [ -n "$pkg" ] && [ -f "$pkg" ]; then
            echo "Extracting $pkg..."
            dpkg-deb -xv "$pkg" . || true # ignore tar ownership errors
            if [ -d "usr/lib/${ARCH_TRIPLET}" ]; then
                cp -r usr/lib/${ARCH_TRIPLET}/* lib/${ARCH_TRIPLET}/ || true
            fi
            if [ -d "usr/lib" ]; then
                cp -r usr/lib/* lib/ || true
            fi
            rm -f "$pkg"
            rm -rf usr etc
        fi
    fi
done

# Clean up any remaining .deb files
rm -f *.deb

echo -e "\n\nImport Programs:\n"

# Import basic utilities needed by the game
utilities=("coreutils" "diffutils" "grep" "findutils" "sed")
for util in "${utilities[@]}"; do
    if ! [ -f "data/$(basename /usr/bin/$util 2>/dev/null)" ]; then
        echo "Downloading $util for architecture $ARCH..."
        apt download ${util}:${ARCH} 2>/dev/null || true
        pkg=$(ls | grep "${util}" || true)
        if [ -n "$pkg" ] && [ -f "$pkg" ]; then
            echo "Extracting $pkg..."
            dpkg-deb -xv "$pkg" . || true
            if [ -d "usr/bin" ]; then
                for bin in cp ls mkdir mv echo rm tr basename mktemp; do
                    if [ -f "usr/bin/$bin" ]; then
                        cp "usr/bin/$bin" data/ || true
                    fi
                done
            fi
            rm -f "$pkg"
            rm -rf usr etc
        fi
    fi
done

echo -e "\n\nLibrary import completed successfully."

exit 0
