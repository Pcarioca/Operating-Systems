// treasure_hub.c
// Simplified hub without FIFOs, suitable for WSL or any POSIX filesystem.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include "treasure_io.h"

static void print_hunts(void) {
    DIR *dp = opendir(".");
    if (!dp) {
        perror("opendir");
        return;
    }
    struct dirent *ent;
    while ((ent = readdir(dp)) != NULL) {
        if (ent->d_type == DT_DIR
         && ent->d_name[0] != '.'
         && strcmp(ent->d_name, "build") != 0) {
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", ent->d_name, TREASURE_FILE);
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

// Runs argv[0] … argv[n-1], piping its stdout/stderr back to this process.
static int run_command(char *const argv[]) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        // child
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        execvp(argv[0], argv);
        perror("execvp");
        _exit(1);
    }
    // parent
    close(pipefd[1]);
    char buf[512];
    ssize_t n;
    while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
        fwrite(buf, 1, n, stdout);
    }
    close(pipefd[0]);
    int status = 0;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int main(void) {
    char line[512];
    printf("[Hub] Ready> ");
    fflush(stdout);

    while (fgets(line, sizeof(line), stdin)) {
        // strip newline
        line[strcspn(line, "\n")] = '\0';

        // parse
        char *saveptr = NULL;
        char *cmd = strtok_r(line, " ", &saveptr);
        if (!cmd) {
            printf("[Hub] Ready> ");
            fflush(stdout);
            continue;
        }

        if (strcmp(cmd, "exit") == 0) {
            break;
        }
        else if (strcmp(cmd, "list_hunts") == 0) {
            print_hunts();
        }
        else if (strcmp(cmd, "list_treasures") == 0) {
            char *hunt = strtok_r(NULL, " ", &saveptr);
            if (!hunt) {
                printf("Usage: list_treasures <hunt>\n");
            } else {
                char *args[] = {
                    "./build/treasure_manager",
                    "--list",
                    hunt,
                    NULL
                };
                run_command(args);
            }
        }
        else if (strcmp(cmd, "view_treasure") == 0) {
            char *hunt = strtok_r(NULL, " ", &saveptr);
            char *id   = strtok_r(NULL, " ", &saveptr);
            if (!hunt || !id) {
                printf("Usage: view_treasure <hunt> <id>\n");
            } else {
                char *args[] = {
                    "./build/treasure_manager",
                    "--view",
                    hunt,
                    id,
                    NULL
                };
                run_command(args);
            }
        }
        else if (strcmp(cmd, "calculate_score") == 0) {
            DIR *dp = opendir(".");
            if (!dp) {
                perror("opendir");
            } else {
                struct dirent *ent;
                while ((ent = readdir(dp)) != NULL) {
                    if (ent->d_type == DT_DIR
                     && ent->d_name[0] != '.'
                     && strcmp(ent->d_name, "build") != 0) {
                        printf("Scores for %s:\n", ent->d_name);
                        char *args[] = {
                            "./build/calculate_score",
                            ent->d_name,
                            NULL
                        };
                        run_command(args);
                    }
                }
                closedir(dp);
            }
        }
        else {
            printf("Unknown command. Available:\n");
            printf("  list_hunts\n");
            printf("  list_treasures <hunt>\n");
            printf("  view_treasure <hunt> <id>\n");
            printf("  calculate_score\n");
            printf("  exit\n");
        }

        printf("[Hub] Ready> ");
        fflush(stdout);
    }

    return 0;
}
