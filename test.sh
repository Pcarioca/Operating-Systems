#!/bin/bash

# --- Phase 1 automated smoke tests ---
rm -rf game_test logged_hunt-game_test
mkdir -p build
make

echo "=== Phase 1: add a treasure ==="
./build/treasure_manager --add game_test <<EOF
1
Andrei
45.76
21.23
Sub copac
100
EOF

echo
echo "=== Phase 1: list treasures ==="
./build/treasure_manager --list game_test

echo
echo "=== Phase 1: view treasure ID 1 ==="
./build/treasure_manager --view game_test 1

# echo
# echo "=== Phase 1: remove treasure ID 1 ==="
# ./build/treasure_manager --remove game_test 1

# echo
# echo "=== Phase 1: list after removal ==="
# ./build/treasure_manager --list game_test

# --- Phase 2 manual tests ---
echo
echo "Now you can exercise Phase 2 by running ./treasure_hub and using:"
echo "  start_monitor"
echo "  list_hunts"
echo "  list_treasures game_test"
echo "  view_treasure game_test 2"
echo "  stop_monitor"
echo "  exit"
