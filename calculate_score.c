#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "treasure_io.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hunt_id>\n", argv[0]);
        return 1;
    }
    const char *hunt_id = argv[1];
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, TREASURE_FILE);
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("Open treasure file");
        return 1;
    }

    typedef struct { char username[MAX_NAME]; int score; } Score;
    int capacity = 8, count = 0;
    Score *scores = malloc(capacity * sizeof(Score));
    if (!scores) { perror("malloc"); close(fd); return 1; }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        int idx = -1;
        for (int i = 0; i < count; i++) {
            if (strcmp(scores[i].username, t.username) == 0) { idx = i; break; }
        }
        if (idx < 0) {
            if (count == capacity) {
                capacity *= 2;
                scores = realloc(scores, capacity * sizeof(Score));
                if (!scores) { perror("realloc"); close(fd); return 1; }
            }
            strcpy(scores[count].username, t.username);
            scores[count].score = t.value;
            count++;
        } else {
            scores[idx].score += t.value;
        }
    }
    close(fd);

    for (int i = 0; i < count; i++) {
        printf("%s: %d\n", scores[i].username, scores[i].score);
    }
    free(scores);
    return 0;
}