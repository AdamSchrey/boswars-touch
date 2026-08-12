# Makefile for boswars-touch (adapted for Clickable and Lua 5.1)

TARGET = boswars
BUILD_DIR = build/${ARCH_TRIPLET}/app
SRC_DIR = engine/stratagus
INCLUDES = -I$(SRC_DIR)/include -Iengine/include -Iengine/guichan/include
CXXFLAGS = -Wall -fsigned-char -D_GNU_SOURCE=1 -D_REENTRANT -O2
LDFLAGS = -lSDL2 -llua5.1 -lz -lpng -lvorbis -ltheora -logg

# Find source files
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

release: all

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean release
