#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "./../include/link.h"

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
