CC       := gcc
CFLAGS   := -Wall -g
BUILD_DIR := build

# targets
MANAGER  := $(BUILD_DIR)/treasure_manager
HUB      := treasure_hub

.PHONY: all clean

all: $(BUILD_DIR) $(MANAGER) $(HUB)

# create the build directory if missing
$(BUILD_DIR):
	mkdir -p $@

# Phase 1: manager in build/
$(MANAGER): treasure_manager.c treasure_io.c treasure_io.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ treasure_manager.c treasure_io.c

# Phase 2: hub in project root
$(HUB): treasure_hub.c
	$(CC) $(CFLAGS) -o $@ treasure_hub.c

clean:
	rm -rf $(BUILD_DIR) $(HUB) exported_treasures.txt game_test logged_hunt-* *.dat hub_cmd.txt
