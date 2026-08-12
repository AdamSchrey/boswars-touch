#!/bin/bash

# Clean up previous build files
rm -f *.o
rm -f Makefile
rm -f *.pro
rm -f moc_*
rm -f .qmake*
rm -rf .clickable lib build

# Clean and recreate data directory
rm -rf data

# Create data directory structure
mkdir -p data/campaigns
mkdir -p data/doc
mkdir -p data/graphics
mkdir -p data/intro
mkdir -p data/languages
mkdir -p data/maps
mkdir -p data/patches
mkdir -p data/scripts
mkdir -p data/sounds
mkdir -p data/units

# Copy all data files to data directory
echo "Copying data files..."

# Copy campaigns
if [ -d "campaigns" ]; then
    cp -r campaigns data/ 2>/dev/null || true
fi

# Copy documentation
if [ -d "doc" ]; then
    cp -r doc data/ 2>/dev/null || true
fi

# Copy graphics
if [ -d "graphics" ]; then
    cp -r graphics data/ 2>/dev/null || true
fi

# Copy intro
if [ -d "intro" ]; then
    cp -r intro data/ 2>/dev/null || true
fi

# Copy languages
if [ -d "languages" ]; then
    cp -r languages data/ 2>/dev/null || true
fi

# Copy maps
if [ -d "maps" ]; then
    cp -r maps data/ 2>/dev/null || true
fi

# Copy patches
if [ -d "patches" ]; then
    cp -r patches data/ 2>/dev/null || true
fi

# Copy scripts
if [ -d "scripts" ]; then
    cp -r scripts data/ 2>/dev/null || true
fi

# Copy sounds
if [ -d "sounds" ]; then
    cp -r sounds data/ 2>/dev/null || true
fi

# Copy units
if [ -d "units" ]; then
    cp -r units data/ 2>/dev/null || true
fi

# Copy root files to data directory
echo "Copying root files..."
for file in *.txt *.md CHANGELOG* COPYRIGHT* README* INSTALL*; do
    if [ -f "$file" ]; then
        cp "$file" data/ 2>/dev/null || true
    fi
done

echo "Data files copied successfully."

exit 0
