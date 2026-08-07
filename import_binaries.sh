#!/bin/bash

# Skript zum Importieren der notwendigen Bibliotheken für boswars-touch
# Verwendet Lua 5.1, um Kompatibilität mit den bestehenden Skripten zu gewährleisten

set -Eeou pipefail

echo "ARCH: ${ARCH}"
echo "ROOT: ${ROOT}"

cd "${ROOT}"

echo -e "\n\nImport Libraries:\n"

# Erstelle lib-Verzeichnis für die Architektur
mkdir -p lib/${ARCH_TRIPLET}/

# Lua 5.1 (wichtig für Kompatibilität mit den bestehenden Skripten)
echo "Importiere Lua 5.1..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/liblua5.1.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/liblua5.1.so* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/liblua5.1.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/liblua5.1.so* lib/${ARCH_TRIPLET}/
else
    echo "FEHLER: liblua5.1.so nicht gefunden! Versuche, es über apt herunterzuladen..."
    apt download liblua5.1-0:${ARCH} 2>/dev/null || {
        echo "FEHLER: liblua5.1-0 konnte nicht heruntergeladen werden!"
        exit 1
    }
    lua_deb=$(ls | grep "liblua5.1-0" | head -1)
    if [ -n "$lua_deb" ]; then
        dpkg-deb -xv "$lua_deb" . || true
        cp -r usr/lib/* lib/
        rm -f "$lua_deb"
        rm -rf usr
    else
        echo "FEHLER: liblua5.1-0 Deb-Paket nicht gefunden!"
        exit 1
    fi
fi

# SDL2
echo "Importiere SDL2..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libSDL2-2.0.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libSDL2* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libSDL2-2.0.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/libSDL2* lib/${ARCH_TRIPLET}/
else
    echo "FEHLER: libSDL2 nicht gefunden! Versuche, es über apt herunterzuladen..."
    apt download libsdl2-2.0-0:${ARCH} 2>/dev/null || {
        echo "FEHLER: libsdl2-2.0-0 konnte nicht heruntergeladen werden!"
        exit 1
    }
    sdl_deb=$(ls | grep "libsdl2-2.0-0" | head -1)
    if [ -n "$sdl_deb" ]; then
        dpkg-deb -xv "$sdl_deb" . || true
        cp -r usr/lib/* lib/
        rm -f "$sdl_deb"
        rm -rf usr
    else
        echo "FEHLER: libsdl2-2.0-0 Deb-Paket nicht gefunden!"
        exit 1
    fi
fi

# zlib
echo "Importiere zlib..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libz.so.1" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libz.so* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libz.so.1" ]; then
    cp /usr/lib/aarch64-linux-gnu/libz.so* lib/${ARCH_TRIPLET}/
else
    echo "FEHLER: libz.so nicht gefunden! Versuche, es über apt herunterzuladen..."
    apt download zlib1g:${ARCH} 2>/dev/null || {
        echo "FEHLER: zlib1g konnte nicht heruntergeladen werden!"
        exit 1
    }
    zlib_deb=$(ls | grep "zlib1g" | head -1)
    if [ -n "$zlib_deb" ]; then
        dpkg-deb -xv "$zlib_deb" . || true
        cp -r usr/lib/* lib/
        rm -f "$zlib_deb"
        rm -rf usr
    else
        echo "FEHLER: zlib1g Deb-Paket nicht gefunden!"
        exit 1
    fi
fi

# libpng
echo "Importiere libpng..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libpng16.so.16" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libpng* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libpng16.so.16" ]; then
    cp /usr/lib/aarch64-linux-gnu/libpng* lib/${ARCH_TRIPLET}/
else
    echo "WARN: libpng nicht gefunden! Dies ist optional."
fi

# libvorbis
echo "Importiere libvorbis..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libvorbis.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libvorbis* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libvorbis.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/libvorbis* lib/${ARCH_TRIPLET}/
else
    echo "WARN: libvorbis nicht gefunden! Dies ist optional."
fi

# libtheora
echo "Importiere libtheora..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libtheora.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libtheora* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libtheora.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/libtheora* lib/${ARCH_TRIPLET}/
else
    echo "WARN: libtheora nicht gefunden! Dies ist optional."
fi

# libogg
echo "Importiere libogg..."
if [ -f "/usr/lib/${ARCH_TRIPLET}/libogg.so.0" ]; then
    cp /usr/lib/${ARCH_TRIPLET}/libogg* lib/${ARCH_TRIPLET}/
elif [ -f "/usr/lib/aarch64-linux-gnu/libogg.so.0" ]; then
    cp /usr/lib/aarch64-linux-gnu/libogg* lib/${ARCH_TRIPLET}/
else
    echo "WARN: libogg nicht gefunden! Dies ist optional."
fi

# libc (wichtig für dynamische Verknüpfung)
echo "Importiere libc6..."
apt download libc6:${ARCH} 2>/dev/null || {
    echo "WARN: libc6 konnte nicht heruntergeladen werden, versuche es mit apt-get..."
    apt-get download libc6:${ARCH} 2>/dev/null || {
        echo "WARN: libc6 konnte nicht heruntergeladen werden. Dies könnte zu Problemen führen."
    }
}
libc_deb=$(ls | grep "libc6" | head -1)
if [ -n "$libc_deb" ]; then
    dpkg-deb -xv "$libc_deb" . || true
    cp -r usr/lib/* lib/
    rm -f "$libc_deb"
    rm -rf usr
fi

echo -e "\n\nBibliotheken importiert. Fertig!"

exit 0
