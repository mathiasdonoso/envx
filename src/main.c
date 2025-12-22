#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/exec.h"
#include "../include/link.h"

void print_usage();
void handle_link(char *src, char *dst);
void handle_exec(char *src, char *argv[]);

void print_usage(void);

void print_usage(void) {
    fprintf(stderr, "Usage: envx <subcommand> <env> <command> [args...]\n");
    fprintf(stderr, "\nSubcommands:\n");
    fprintf(stderr, "  exec  - Load environment and execute command\n");
    fprintf(stderr, "  link  - Create .env symlink\n");
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
