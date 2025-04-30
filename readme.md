To run my code, run the following commands in a linux terminal:

make

./test.sh

The shell script will call the program using different commands and simulate standard output to input, delete, write to file etc.
The log is stored into logged_hunt and the binary output is in treasures.dat. The shell script outputs the standard output also onto exported_treasures.txt

# Treasure Hunt Game System

## Phase 1: File Systems

### Build & Test

```bash
make treasure_manager
./test.sh

Commands

--add <hunt_id>: Add a treasure to hunt directory.

--list <hunt_id>: Show hunt name, file size, last modified time, and list treasures.

--view <hunt_id> <id>: Show details for treasure ID.

--remove <hunt_id> <id>: Remove a treasure by ID.

--remove_hunt <hunt_id>: Remove entire hunt directory and logs.

All operations are logged in <hunt_id>/logged_hunt and a symlink logged_hunt-<hunt_id> is created in the program root.