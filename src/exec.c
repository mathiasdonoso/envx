#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/exec.h"

void handle_exec(char *src, char *argv[]) {
    FILE *file = fopen(src, "r");
    if (file == NULL) {
        fprintf(stderr, "unable to open file: %s\n", src);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == 0) {
        // TODO: handle env variables bigger than 256 bytes?
        char line[256];
        int s;
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = '\0';
            if (line[0] == '\0' || line[0] == '#') {
                continue;
            }

            // TODO: trim
            char *equals = strchr(line, '=');
            if (equals != NULL) {
                *equals = '\0';
                char *key = line;
                char *value = equals + 1;
                s = setenv(key, value, 1);
                if (s != 0) {
                    fprintf(stderr, "could not set env variable: %s to %s\n", key, value);
                }
            }
        }

        fclose(file);
        execvp(argv[3], &argv[3]);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // parent
    fclose(file);
    int status;
    waitpid(pid, &status, 0);
    exit(WEXITSTATUS(status));
}

