# Makefile for Operating-Systems Phase 3

CC       := gcc
CFLAGS   := -Wall -g

.PHONY: all clean

# Default target: build everything
all: build/calculate_score build/treasure_manager treasure_hub

# Ensure build directory exists
build:
	mkdir -p build

# Build calculate_score (link treasure_io.c)
build/calculate_score: calculate_score.c treasure_io.h treasure_io.c | build
	$(CC) $(CFLAGS) -o $@ calculate_score.c treasure_io.c

# Build treasure_manager (link treasure_io.c)
build/treasure_manager: treasure_manager.c treasure_io.h treasure_io.c | build
	$(CC) $(CFLAGS) -o $@ treasure_manager.c treasure_io.c

# Build the hub executable
treasure_hub: treasure_hub.c | build
	$(CC) $(CFLAGS) -o $@ treasure_hub.c

# Clean up all generated files
clean:
	rm -rf build \
	       treasure_hub \
	       exported_treasures.txt \
	       game_test \
	       logged_hunt-* \
	       *.dat \
	       hub_cmd.txt