#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define BASE_DIR "./hunts"
#define LOG_FILE "logged_hunt"
#define RECORD_SIZE 128

typedef struct {
    char id[10];
    char username[50];
    float latitude;
    float longitude;
    char clue[50];
    int value;
} Treasure;

int main(void){

    printf("Test\n");

    int fdesc = open("LOG_FILE", O_CREAT);

    write(fdesc, "idk", 3);

    int cl = close(fdesc);

    return 0;
}