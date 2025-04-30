#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include "treasure_io.h"   // for TREASURE_FILE and sizeof(Treasure)

static pid_t monitor_pid = 0;
static const char *CMD_FILE = "hub_cmd.txt";

// helper to scan “./” for all hunt dirs and print counts
static void print_hunts(void) {
    DIR *dp = opendir(".");
    if (!dp) { perror("opendir"); return; }
    struct dirent *ent;
    while ((ent = readdir(dp))) {
        if (ent->d_type == DT_DIR
         && ent->d_name[0] != '.'
         && strcmp(ent->d_name, "build") != 0)
        {
            char path[512];
            snprintf(path, sizeof(path),
                     "%s/%s", ent->d_name, TREASURE_FILE);
            struct stat st;
            if (stat(path, &st) == 0) {
                int count = st.st_size / sizeof(Treasure);
                printf("Hunt: %s, treasures: %d\n",
                       ent->d_name, count);
            }
        }
    }
    closedir(dp);
}

// Called in the monitor on SIGUSR1 for list_treasures / view_treasure
static void cmd_handler(int sig) {
    char buf[256] = {0};
    int fd = open(CMD_FILE, O_RDONLY);
    if (fd < 0) return;
    int n = read(fd, buf, sizeof(buf)-1);
    close(fd);
    if (n <= 0) return;
    buf[n] = '\0';

    char *cmd = strtok(buf, " \n");
    if (!cmd) return;

    pid_t c = fork();
    if (c < 0) return;
    if (c == 0) {
        // child => dispatch to build/treasure_manager
        if (strcmp(cmd, "list_treasures") == 0) {
            char *hunt = strtok(NULL, " \n");
            execl("./build/treasure_manager",
                  "treasure_manager", "--list", hunt,
                  (char*)NULL);
        }
        else if (strcmp(cmd, "view_treasure") == 0) {
            char *hunt = strtok(NULL, " \n");
            char *id   = strtok(NULL, " \n");
            execl("./build/treasure_manager",
                  "treasure_manager", "--view",
                  hunt, id, (char*)NULL);
        }
        _exit(1);  // unknown
    }
    waitpid(c, NULL, 0);
}

// reap the monitor itself when it dies
static void handle_sigchld(int sig) {
    int status;
    pid_t p = waitpid(monitor_pid, &status, WNOHANG);
    if (p == monitor_pid) {
        if (WIFEXITED(status))
            printf("[Hub] Monitor exited (status=%d)\n",
                   WEXITSTATUS(status));
        monitor_pid = 0;
    }
}

// start the long‐lived monitor process
static void start_monitor(void) {
    if (monitor_pid) {
        printf("Monitor already running.\n");
        return;
    }
    pid_t p = fork();
    if (p < 0) { perror("fork"); return; }
    if (p == 0) {
        // monitor child
        struct sigaction sa = {0};
        sa.sa_handler = cmd_handler;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGUSR1, &sa, NULL);
        signal(SIGCHLD, SIG_DFL);
        while (1) pause();   // wait for signals
        _exit(0);
    }
    // hub parent
    monitor_pid = p;
    signal(SIGCHLD, handle_sigchld);
    printf("[Hub] Monitor started (PID %d)\n", monitor_pid);
}

int main(void) {
    char line[256];
    printf("[Hub] Ready> ");
    while (fgets(line, sizeof(line), stdin)) {
        if (strncmp(line, "start_monitor", 13) == 0) {
            start_monitor();
        }
        else if (strncmp(line, "stop_monitor", 12) == 0) {
            if (monitor_pid)
                kill(monitor_pid, SIGTERM);
            else
                printf("No monitor running.\n");
        }
        else if (strncmp(line, "list_hunts", 10) == 0) {
            if (!monitor_pid)
                printf("Start monitor first.\n");
            else
                print_hunts();
        }
        else if (strncmp(line, "view_treasure", 13) == 0
              || strncmp(line, "list_treasures", 15) == 0)
        {
            if (!monitor_pid) {
                printf("Start monitor first.\n");
            } else {
                int fd = open(CMD_FILE,
                              O_WRONLY|O_CREAT|O_TRUNC, 0644);
                if (fd >= 0) {
                    write(fd, line, strlen(line));
                    close(fd);
                    kill(monitor_pid, SIGUSR1);
                }
            }
        }
        else if (strncmp(line, "exit", 4) == 0) {
            if (monitor_pid) 
                printf("Stop monitor first.\n");
            else
                break;
        }
        else {
            printf("Unknown command or start monitor first.\n");
        }
        printf("[Hub] Ready> ");
    }
    return 0;
}
