#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

void print_usage();
void handle_link(char *src, char *dst);
void handle_exec(char *src, char *argv[]);

void print_usage() {
    fprintf(stderr, "Usage: envx <subcommand> <env> <command> [args...]\n");
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    char *command = argv[1];
    char *env = argv[2];

    char *filename = malloc(strlen(".env.") + strlen(env) + 1);
    if (filename == NULL) {
        fprintf(stderr, "error allocating memory");
        exit(EXIT_FAILURE);
    }

    strcpy(filename, ".env.");
    strcat(filename, env);

    if (access(filename, F_OK) != 0) {
        fprintf(stderr, "could not found file: %s\n", filename);
        free(filename);
        exit(EXIT_FAILURE);
    }

    if (strcmp(command, "exec") == 0) {
        if (argc < 4) {
            print_usage();
            free(filename);
            exit(EXIT_FAILURE);
        }

        handle_exec(filename, argv);
    } else if (strcmp(command, "link") == 0) {
        handle_link(filename, ".env");
    }

    free(filename);
    exit(EXIT_SUCCESS);
}

// void handle_exec(int argc, char **argv, char *src);
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

            // Skip empty lines and comments
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

void handle_link(char *src, char *dst) {
    if (access(dst, F_OK) == 0) {
        struct stat buf;
        if (lstat(dst, &buf) != 0) {
            fprintf(stderr, "error reading file: %s\n", dst);
            exit(EXIT_FAILURE);
        }

        // TODO: maybe ask the user to delete the regular file?
        if (S_ISREG(buf.st_mode)) {
            fprintf(stderr, "%s is a regular file, please delete it before using envx\n", dst);
            exit(EXIT_FAILURE);
        }

        if (S_ISLNK(buf.st_mode)) {
            if (remove(dst) != 0) {
                fprintf(stderr, "couldn't remove file: %s\n", dst);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (symlink(src, dst) != 0) {
        fprintf(stderr, "Couldn't create a symlink for: %s\n", src);
        exit(EXIT_FAILURE);
    }
}
