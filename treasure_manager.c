#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "treasure_io.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s --command hunt_id [id]\n", argv[0]);
        return 1;
    }


    const char *cmd = argv[1];
    const char *hunt_id = argv[2];

    if (strcmp(cmd, "--add") == 0) return add_treasure(hunt_id);
    else if (strcmp(cmd, "--list") == 0) return list_treasures(hunt_id);
    else if (strcmp(cmd, "--view") == 0 && argc == 4) return view_treasure(hunt_id, atoi(argv[3]));
    else if (strcmp(cmd, "--remove") == 0 && argc == 4) return remove_treasure(hunt_id, atoi(argv[3]));
    else if (strcmp(cmd, "--remove_hunt") == 0) return remove_hunt(hunt_id);
    else if (strcmp(cmd, "--log") == 0) {
        log_operation(hunt_id, argv[3]);
        return 0;
    }
    else if (strcmp(cmd, "--help") == 0) {
        printf("Usage: %s --command hunt_id [id]\n", argv[0]);
        printf("Commands:\n");
        printf("  --add hunt_id\n");
        printf("  --list hunt_id\n");
        printf("  --view hunt_id id\n");
        printf("  --remove hunt_id id\n");
        printf("  --remove_hunt hunt_id\n");
        printf("  --log hunt_id operation\n");
        return 0;
    }
    else if (strcmp(cmd, "--version") == 0) {
        printf("Treasure Manager version 1.0\n");
        return 0;
    }
    else if (strcmp(cmd, "--test") == 0) {
        printf("Testing mode activated.\n");
        
        return 0;
    }

    else {
        fprintf(stderr, "Unknown or badly used command.\n");
        return -1;
    }
}
