# ==============================
# Project Configuration
# ==============================

CC       := gcc
CFLAGS   := -Wall -Wextra -pedantic -std=c99
DEBUGFLAGS := -g
RELEASEFLAGS := -O2

SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin
TARGET   := $(BIN_DIR)/femto

# Automatically find all .c files
SOURCES  := $(shell find $(SRC_DIR) -name '*.c')
OBJECTS  := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DEPS     := $(OBJECTS:.o=.d)

# ==============================
# Build Modes
# ==============================

ifeq ($(MODE),release)
    CFLAGS += $(RELEASEFLAGS)
else
    CFLAGS += $(DEBUGFLAGS)
endif

# ==============================
# Default Target
# ==============================

all: $(TARGET)

# ==============================
# Linking
# ==============================

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@

# ==============================
# Compilation
# ==============================

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include auto-generated dependency files
-include $(DEPS)

# ==============================
# Utilities
# ==============================

run: all
	./$(TARGET)

debug:
	$(MAKE) MODE=debug

release:
	$(MAKE) MODE=release

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run clean debug release
