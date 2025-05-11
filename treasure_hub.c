// treasure_hub.c
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

// Hub-side: scan "." for hunt directories and print counts
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

// Monitor-side: handle SIGUSR1 for list_treasures / view_treasure
static void monitor_handler(int sig) {
    char buf[512] = {0};
    int fd = open(CMD_FILE, O_RDONLY);
    if (fd < 0) return;
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    close(fd);
    if (n <= 0) return;
    buf[n] = '\0';

    char *cmd = strtok(buf, " \n");
    if (!cmd) return;

    pid_t c = fork();
    if (c < 0) return;
    if (c == 0) {
        // child: dispatch to build/treasure_manager
        if (strcmp(cmd, "list_treasures") == 0) {
            char *hunt = strtok(NULL, " \n");
            if (hunt)
                execl("./build/treasure_manager",
                      "treasure_manager", "--list", hunt,
                      (char*)NULL);
        }
        else if (strcmp(cmd, "view_treasure") == 0) {
            char *hunt = strtok(NULL, " \n");
            char *id   = strtok(NULL, " \n");
            if (hunt && id)
                execl("./build/treasure_manager",
                      "treasure_manager", "--view",
                      hunt, id, (char*)NULL);
        }
        _exit(1);
    }
    waitpid(c, NULL, 0);

    // signal hub that we're done
    unlink(CMD_FILE);
}

// Hub-side: reap the monitor if it dies
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

// Hub-side: start the background monitor process
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
        sa.sa_handler = monitor_handler;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGUSR1, &sa, NULL);
        signal(SIGCHLD, SIG_DFL);
        // live until killed by SIGTERM
        while (1) pause();
        _exit(0);
    }
    // hub parent
    monitor_pid = p;
    signal(SIGCHLD, handle_sigchld);
    printf("[Hub] Monitor started (PID %d)\n", (int)p);
}

int main(void) {
    char line[512];
    printf("[Hub] Ready> ");
    fflush(stdout);

    while (fgets(line, sizeof(line), stdin)) {
        // strip newline
        line[strcspn(line, "\n")] = '\0';
        // parse
        char *saveptr;
        char *cmd = strtok_r(line, " ", &saveptr);
        if (!cmd) {
            printf("[Hub] Ready> ");
            fflush(stdout);
                 continue;
        }

        if (strcmp(cmd, "start_monitor") == 0) {
            start_monitor();
        }
        else if (strcmp(cmd, "stop_monitor") == 0) {
            if (monitor_pid)
                kill(monitor_pid, SIGTERM);
            else
                printf("No monitor running.\n");
        }
        else if (strcmp(cmd, "exit") == 0) {
            if (monitor_pid)
                printf("Stop monitor first.\n");
            else
                break;
        }
        else if (strcmp(cmd, "list_hunts") == 0) {
            if (!monitor_pid)
                printf("Start monitor first.\n");
            else
                print_hunts();
        }
        else if (strcmp(cmd, "list_treasures") == 0
              || strcmp(cmd, "view_treasure") == 0)
        {
            if (!monitor_pid) {
                printf("Start monitor first.\n");
            } else {
                // rebuild full command
                char fullcmd[512];
                if (saveptr)
                    snprintf(fullcmd, sizeof(fullcmd),
                             "%s %s", cmd, saveptr);
                else
                    snprintf(fullcmd, sizeof(fullcmd),
                             "%s", cmd);
                // write it
                int fd = open(CMD_FILE,
                              O_WRONLY|O_CREAT|O_TRUNC, 0644);
                if (fd >= 0) {
                    dprintf(fd, "%s\n", fullcmd);
                    close(fd);
                    kill(monitor_pid, SIGUSR1);
                    // wait for monitor to finish
                    while (access(CMD_FILE, F_OK) == 0)
                         usleep(10000);
                }
            }
        }
        else {
            printf("Unknown command.\n");
        }

        printf("[Hub] Ready> ");
        fflush(stdout);
    }

    return 0;
}
