# Operating Systems: Treasure Hunt Project

This repository implements a multi-process "Treasure Hunt" system in C, comprising three main components:

1. **`treasure_manager`**: CLI tool to create and manage treasures in individual hunts.
2. **`treasure_hub`**: Central hub CLI that orchestrates commands across hunts, launching child processes with pipes.
3. **`calculate_score`**: Standalone tool that computes per-user score totals for a given hunt.

A `Makefile` automates building all executables into a `build/` directory.


## Directory Structure

```
├── calculate_score.c       # Phase 3: score aggregation tool
├── treasure_hub.c          # Simplified hub (fork+pipe-based)
├── treasure_manager.c      # Main manager for treasures
├── treasure_io.c           # I/O routines and data structures
├── treasure_io.h           # Treasure struct and constants
├── Makefile                # Build rules for all components
└── build/                  # Binaries output directory
```

---

## Building the Project

1. **Create the build directory** and compile everything:

   ```sh
   make clean
   make
   ```
2. Binaries are placed in `build/`:

   * `build/treasure_manager`
   * `build/treasure_hub`
   * `build/calculate_score`

---

## `treasure_manager` Commands

This tool reads and writes a `treasures.dat` file inside each hunt directory.

* **Add a treasure**

  ```sh
  ./build/treasure_manager --add <hunt_dir>
  ```

  * Prompts for:

    * **Treasure ID** (integer)
    * **Username** (string)
    * **Latitude** (float)
    * **Longitude** (float)
    * **Clue** (string)
    * **Value** (integer)
  * Appends a `Treasure` record to `<hunt_dir>/treasures.dat`.

* **List treasures**

  ```sh
  ./build/treasure_manager --list <hunt_dir>
  ```

  * Outputs all treasures with ID, username, latitude, longitude, clue, and value.

* **View a treasure**

  ```sh
  ./build/treasure_manager --view <hunt_dir> <treasure_id>
  ```

  * Displays detailed fields for the specified ID.

* **Remove a treasure**

  ```sh
  ./build/treasure_manager --remove <hunt_dir> <treasure_id>
  ```

  * Deletes that record from the file.

* **Remove an entire hunt**

  ```sh
  ./build/treasure_manager --remove-hunt <hunt_dir>
  ```

  * Deletes the entire directory and `treasures.dat`.

* **Log operations**

  * Any add/remove action is appended to `logged_hunt-<date>.txt`.

---

## `calculate_score` Command

Standalone C program that scans a hunt’s `treasures.dat` and sums values by user.

```sh
./build/calculate_score <hunt_dir>
```

* **Output format:**

  ```
  ```

alice: 50
bob:   75

```

---

## `treasure_hub` Commands

High-level hub that forks child processes and pipes their output back.

- **list_hunts**
```

\[Hub] Ready> list\_hunts
Hunt: hunt1, treasures: 3
Hunt: hunt2, treasures: 5

```
  Lists all subdirectories (excluding `build/`) containing `treasures.dat`, with counts.

- **list_treasures**
```

\[Hub] Ready> list\_treasures \<hunt\_dir>

```
  Invokes `treasure_manager --list` under the hood and prints results.

- **view_treasure**
```

\[Hub] Ready> view\_treasure \<hunt\_dir> <id>

```
  Invokes `treasure_manager --view`.

- **calculate_score**
```

\[Hub] Ready> calculate\_score

```
  Runs `calculate_score` on each hunt directory, printing per-user totals.

- **exit**
```

\[Hub] Ready> exit

````
  Quits the hub.

Unknown commands will show a usage hint.

---

## End-to-End Example

```sh
# Setup sample hunts
date=$(date +"%Y%m%d")
mkdir -p hunt1 hunt2
./build/treasure_manager --add hunt1    # follow prompts for alice, bob
./build/treasure_manager --add hunt2    # prompts for carol, dave

# Start hub
./build/treasure_hub
[Hub] Ready> list_hunts
Hunt: hunt1, treasures: 2
Hunt: hunt2, treasures: 2
[Hub] Ready> list_treasures hunt1
... details ...
[Hub] Ready> view_treasure hunt2 1
... details ...
[Hub] Ready> calculate_score
Scores for hunt1:
alice: 50
bob:   75
Scores for hunt2:
carol: 30
dave:  20
[Hub] Ready> exit
````

---

## Cleaning Up

```sh
make clean
```

Removes `build/`, all generated binaries, `.dat` files, and logs.

---

> Happy hunting! 🎉
