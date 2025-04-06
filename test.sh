#!/bin/bash

# Clean previous runs
rm -rf game_test logged_hunt-game_test
mkdir -p build

echo " Testing treasure_manager Phase 1"

echo -e "1\nalice\n45.76\n21.23\nUnder the old oak tree\n100" | build/treasure_manager --add game_test


echo -e "2\nbob\n40.42\n23.65\nBehind the waterfall\n250" | build/treasure_manager --add game_test


echo -e "\n List all treasures:"
build/treasure_manager --list game_test


echo -e "\n View treasure ID 1:"
build/treasure_manager --view game_test 1


echo -e "\nRemove treasure ID 1:"
build/treasure_manager --remove game_test 1


echo -e "\n List after removal:"
build/treasure_manager --list game_test


echo -e "\n Removing entire hunt:"
build/treasure_manager --remove_hunt game_test

echo "All basic operations complete"
