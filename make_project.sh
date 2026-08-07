#!/bin/bash

# Skript zum Vorbereiten des Projekts für Clickable
# Dies kopiert die notwendigen Dateien und bereitet das Build-System vor

set -e

echo "Bereite boswars-touch für Clickable vor..."

# Erstelle Build-Verzeichnis
mkdir -p build

# Kopiere die Engine-Quelldateien in das Build-Verzeichnis
cp -r engine build/

# Erstelle ein einfaches Makefile für das Build-System
cat > build/Makefile << 'EOF'
# Einfaches Makefile für boswars-touch
TARGET = boswars
BUILD_DIR = .
SRC_DIR = engine/stratagus
INCLUDES = -I$(SRC_DIR)/include -Iengine/include -Iengine/guichan/include
CXXFLAGS = -Wall -fsigned-char -D_GNU_SOURCE=1 -D_REENTRANT -O2
LDFLAGS = -lSDL2 -llua5.1 -lz -lpng -lvorbis -ltheora -logg

# Quelldateien finden
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

echo "Makefile erstellt."

# Erstelle die notwendigen Konfigurationsdateien für Clickable
echo "Erstelle manifest.json..."
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

echo "Erstelle boswars-touch.desktop..."
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

echo "Erstelle boswars-touch.apparmor..."
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

echo "Erstelle start.sh..."
cat > data/start.sh << 'EOF'
#!/bin/bash

# Setze das Arbeitsverzeichnis auf das Datenverzeichnis
cd "$(dirname "$0")"

# Exportiere LD_LIBRARY_PATH, um die mitgelieferten Bibliotheken zu finden
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:+$LD_LIBRARY_PATH:}$(dirname "$0")/../lib/${ARCH_TRIPLET}"

# Führe das Spiel aus
./boswars

exit 0
EOF

chmod +x data/start.sh

# Kopiere ein Icon (falls nicht vorhanden)
if [ ! -f "data/boswars-touch.png" ]; then
    if [ -f "data/graphics/general/dejavusansbold14.png" ]; then
        cp data/graphics/general/dejavusansbold14.png data/boswars-touch.png
    fi
fi

echo "Projekt vorbereitet für Clickable."
exit 0
