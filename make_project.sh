#!/bin/bash

# Script to prepare the project for Clickable
# This copies the necessary files and prepares the build system

set -e

echo "Preparing boswars-touch for Clickable..."

# Create build directory
mkdir -p build

# Copy the engine source files to the build directory
cp -r engine build/

# Create a simple Makefile for the build system
cat > build/Makefile << 'EOF'
# Simple Makefile for boswars-touch
TARGET = boswars
BUILD_DIR = .
SRC_DIR = engine/stratagus
INCLUDES = -I$(SRC_DIR)/include -Iengine/include -Iengine/guichan/include
CXXFLAGS = -Wall -fsigned-char -D_GNU_SOURCE=1 -D_REENTRANT -O2
LDFLAGS = -lSDL2 -llua5.1 -lz -lpng -lvorbis -ltheora -logg

# Find source files
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

release: all

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean release
EOF

echo "Makefile created."

# Create the necessary configuration files for Clickable
echo "Creating manifest.json..."
cat > data/manifest.json << 'EOF'
{
    "architecture": "@CLICK_ARCH@",
    "description": "Bos Wars is a real-time strategy game based on the Stratagus engine.",
    "framework": "@CLICK_FRAMEWORK@",
    "hooks": {
        "boswars-touch": {
            "apparmor": "boswars-touch.apparmor",
            "desktop": "boswars-touch.desktop"
        }
    },
    "maintainer": "Adam Schrey <ut-phablet-things@mail.de>",
    "name": "boswars-touch",
    "title": "Bos Wars Touch",
    "version": "2.10.0"
}
EOF

echo "Creating boswars-touch.desktop..."
cat > data/boswars-touch.desktop << 'EOF'
[Desktop Entry]
Name=Bos Wars Touch
Exec=start.sh
X-Ubuntu-Touch=true
Terminal=false
Type=Application
Icon=boswars-touch.png
Comment=Bos Wars is a real-time strategy game based on the Stratagus engine.
StartupNotify=false
EOF

echo "Creating boswars-touch.apparmor..."
cat > data/boswars-touch.apparmor << 'EOF'
{
    "policy_groups": [
        "networking",
        "audio",
        "pulseaudio"
    ],
    "policy_version": 16.04,
    "name": "boswars-touch"
}
EOF

echo "Creating start.sh..."
cat > data/start.sh << 'EOF'
#!/bin/bash

# Set working directory to the data directory
cd "$(dirname "$0")"

# Export LD_LIBRARY_PATH to find the included libraries
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:+$LD_LIBRARY_PATH:}$(dirname "$0")/../lib/${ARCH_TRIPLET}"

# Run the game
./boswars

exit 0
EOF

chmod +x data/start.sh

# Copy an icon (if not present)
if [ ! -f "data/boswars-touch.png" ]; then
    if [ -f "data/graphics/general/dejavusansbold14.png" ]; then
        cp data/graphics/general/dejavusansbold14.png data/boswars-touch.png
    fi
fi

echo "Project prepared for Clickable."
exit 0
