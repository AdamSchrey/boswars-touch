#!/bin/bash

# Script to import the necessary libraries for boswars-touch
# Uses Lua 5.1 to ensure compatibility with existing scripts

set -Eeou pipefail

echo "ARCH: ${ARCH}"
echo "ROOT: ${ROOT}"

cd "${ROOT}"

echo -e "\n\nImport Libraries:\n"

# Create lib directory for the architecture
mkdir -p lib/${ARCH_TRIPLET}/

# Lua 5.1 (important for compatibility with existing scripts)
echo "Importing Lua 5.1..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/liblua5.1.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/liblua5.1.so* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/liblua5.1.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/liblua5.1.so* lib/${ARCH_TRIPLET}/
else
    echo "ERROR: liblua5.1.so not found! Trying to download via apt..."
    apt download liblua5.1-0:${ARCH} 2>/dev/null || {
        echo "ERROR: liblua5.1-0 could not be downloaded!"
        exit 1
    }
    lua_deb=$(ls | grep "liblua5.1-0" | head -1)
    if [ -n "$lua_deb" ]; then
        dpkg-deb -xv "$lua_deb" . || true
        cp -r usr/lib/* lib/
        rm -f "$lua_deb"
        rm -rf usr
    else
        echo "ERROR: liblua5.1-0 deb package not found!"
        exit 1
    fi
fi

# SDL2
echo "Importing SDL2..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libSDL2-2.0.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libSDL2* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libSDL2-2.0.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/libSDL2* lib/${ARCH_TRIPLET}/
else
    echo "ERROR: libSDL2 not found! Trying to download via apt..."
    apt download libsdl2-2.0-0:${ARCH} 2>/dev/null || {
        echo "ERROR: libsdl2-2.0-0 could not be downloaded!"
        exit 1
    }
    sdl_deb=$(ls | grep "libsdl2-2.0-0" | head -1)
    if [ -n "$sdl_deb" ]; then
        dpkg-deb -xv "$sdl_deb" . || true
        cp -r usr/lib/* lib/
        rm -f "$sdl_deb"
        rm -rf usr
    else
        echo "ERROR: libsdl2-2.0-0 deb package not found!"
        exit 1
    fi
fi

# zlib
echo "Importing zlib..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libz.so.1" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libz.so* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libz.so.1" ]; then
    cp /usr/lib/aarch64-linux-gnu/libz.so* lib/${ARCH_TRIPLET}/
else
    echo "ERROR: libz.so not found! Trying to download via apt..."
    apt download zlib1g:${ARCH} 2>/dev/null || {
        echo "ERROR: zlib1g could not be downloaded!"
        exit 1
    }
    zlib_deb=$(ls | grep "zlib1g" | head -1)
    if [ -n "$zlib_deb" ]; then
        dpkg-deb -xv "$zlib_deb" . || true
        cp -r usr/lib/* lib/
        rm -f "$zlib_deb"
        rm -rf usr
    else
        echo "ERROR: zlib1g deb package not found!"
        exit 1
    fi
fi

# libpng
echo "Importing libpng..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libpng16.so.16" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libpng* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libpng16.so.16" ]; then
    cp /usr/lib/aarch64-linux-gnu/libpng* lib/${ARCH_TRIPLET}/
else
    echo "WARN: libpng not found! This is optional."
fi

# libvorbis
echo "Importing libvorbis..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libvorbis.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libvorbis* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libvorbis.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/libvorbis* lib/${ARCH_TRIPLET}/
else
    echo "WARN: libvorbis not found! This is optional."
fi

# libtheora
echo "Importing libtheora..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libtheora.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libtheora* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libtheora.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/libtheora* lib/${ARCH_TRIPLET}/
else
    echo "WARN: libtheora not found! This is optional."
fi

# libogg
echo "Importing libogg..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libogg.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libogg* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libogg.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/libogg* lib/${ARCH_TRIPLET}/
else
    echo "WARN: libogg not found! This is optional."
fi

# libc (important for dynamic linking)
echo "Importing libc6..."
apt download libc6:${ARCH} 2>/dev/null || {
    echo "WARN: libc6 could not be downloaded, trying with apt-get..."
    apt-get download libc6:${ARCH} 2>/dev/null || {
        echo "WARN: libc6 could not be downloaded. This might cause problems."
    }
}
libc_deb=$(ls | grep "libc6" | head -1)
if [ -n "$libc_deb" ]; then
    dpkg-deb -xv "$libc_deb" . || true
    cp -r usr/lib/* lib/
    rm -f "$libc_deb"
    rm -rf usr
fi

echo -e "\n\nLibraries imported. Done!"

exit 0
