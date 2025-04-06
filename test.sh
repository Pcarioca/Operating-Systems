#!/bin/bash

# sterge orice test anterior
rm -rf game_test logged_hunt-game_test
mkdir -p build

echo " Testing treasure_manager Phase 1"

# Adaug comoara 1
echo -e "1\nAndrei\n45.76\n21.23\nSub copac\n100" | build/treasure_manager --add game_test

# Adaug comoara 2
echo -e "2\nPaul\n40.42\n23.65\nDupă cascada\n250" | build/treasure_manager --add game_test

# Afiez toate comorile
echo -e "\n List all treasures:"
build/treasure_manager --list game_test

# Salvez toate comorile intr-un fis
echo -e "\n Put all treasures in file exported_treasures.txt:"
build/treasure_manager --list game_test > exported_treasures.txt

# Vizualizez comoara cu ID 1
echo -e "\n View treasure ID 1:"
build/treasure_manager --view game_test 1

# Elimin comoara cu ID 1
echo -e "\nRemove treasure ID 1:"
build/treasure_manager --remove game_test 1

# Afisez comorile după stergere
echo -e "\n List after removal:"
build/treasure_manager --list game_test

# sterge
echo -e "\n Removing entire hunt:"
build/treasure_manager --remove_hunt game_test

echo "All basic operations complete"
