#include "treasure_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

void log_operation(const char *hunt_id, const char *operation) {
    char path[128];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, LOG_FILE);
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (fd == -1) { perror("Log open failed"); return; }

    dprintf(fd, "%s: %s\n", __TIME__, operation);
    close(fd);
}

int add_treasure(const char *hunt_id) {
    mkdir(hunt_id, 0755);

    char path[128];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, TREASURE_FILE);
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (fd == -1) { perror("Open treasure file"); return 1; }

    Treasure t;
    printf("Enter Treasure ID: "); scanf("%d", &t.id);
    printf("Enter Username: "); scanf("%s", t.username);
    printf("Enter Latitude: "); scanf("%f", &t.latitude);
    printf("Enter Longitude: "); scanf("%f", &t.longitude);
    printf("Enter Clue: "); getchar(); fgets(t.clue, MAX_CLUE, stdin);
    t.clue[strcspn(t.clue, "\n")] = 0;
    printf("Enter Value: "); scanf("%d", &t.value);

    write(fd, &t, sizeof(Treasure));
    close(fd);

    log_operation(hunt_id, "Added treasure");

    char linkname[64];
    snprintf(linkname, sizeof(linkname), "logged_hunt-%s", hunt_id);
    symlink(path, linkname);

    return 0;
}

int list_treasures(const char *hunt_id) {
    char path[128];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, TREASURE_FILE);

    struct stat st;
    if (stat(path, &st) == -1) { perror("Stat failed"); return 1; }

    printf("Hunt: %s\nSize: %ld bytes\nLast modified: %s",
           hunt_id, st.st_size, ctime(&st.st_mtime)); // this will print metadata on the screen!!!!

    int fd = open(path, O_RDONLY);
    if (fd == -1) { perror("Open failed"); return 1; }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %d | User: %s | GPS: (%.3f, %.3f) | Clue: %s | Value: %d\n",
               t.id, t.username, t.latitude, t.longitude, t.clue, t.value);
    }

    close(fd);
    log_operation(hunt_id, "Listed treasures");
    return 0;
}

int view_treasure(const char *hunt_id, int id) {
    char path[128];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, TREASURE_FILE);
    int fd = open(path, O_RDONLY);
    if (fd == -1) { perror("Open failed"); return 1; }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.id == id) {
            printf("ID: %d | User: %s | GPS: (%.3f, %.3f) | Clue: %s | Value: %d\n",
                   t.id, t.username, t.latitude, t.longitude, t.clue, t.value);
            close(fd);
            log_operation(hunt_id, "Viewed treasure");
            return 0;
        }
    }

    printf("Treasure not found.\n");
    close(fd);
    return 1;
}

int remove_treasure(const char *hunt_id, int id) {
    char path[128], temp_path[128];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, TREASURE_FILE);
    snprintf(temp_path, sizeof(temp_path), "%s/temp.dat", hunt_id);

    int fd_in = open(path, O_RDONLY);
    int fd_out = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd_in == -1 || fd_out == -1) { perror("Open"); return 1; }

    Treasure t;
    while (read(fd_in, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.id != id)
            write(fd_out, &t, sizeof(Treasure));
    }

    close(fd_in);
    close(fd_out);

    rename(temp_path, path);
    log_operation(hunt_id, "Removed a treasure");
    return 0;
}

int remove_hunt(const char *hunt_id) {
    char path[128];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, TREASURE_FILE);
    unlink(path);

    snprintf(path, sizeof(path), "%s/%s", hunt_id, LOG_FILE);
    unlink(path);

    rmdir(hunt_id);

    char linkname[64];
    snprintf(linkname, sizeof(linkname), "logged_hunt-%s", hunt_id);
    unlink(linkname);

    printf("Removed hunt %s\n", hunt_id);
    return 0;
}
