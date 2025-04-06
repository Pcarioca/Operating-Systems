#ifndef TREASURE_IO_H
#define TREASURE_IO_H

#define TREASURE_FILE "treasures.dat"
#define LOG_FILE "logged_hunt"
#define MAX_NAME 32
#define MAX_CLUE 128

typedef struct {
    int id;
    char username[MAX_NAME];
    float latitude;
    float longitude;
    char clue[MAX_CLUE];
    int value;
} Treasure;

void log_operation(const char *hunt_id, const char *operation);
int add_treasure(const char *hunt_id);
int list_treasures(const char *hunt_id);
int view_treasure(const char *hunt_id, int id);
int remove_treasure(const char *hunt_id, int id);
int remove_hunt(const char *hunt_id);

#endif
