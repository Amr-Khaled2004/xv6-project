#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    if(argc == 2 && strcmp(argv[1], "?") == 0) {
        printf("Usage: mv source destination\n");
        exit(0);
    }

    if(argc != 3) {
        printf("Invalid command. Usage: mv source destination\n");
        exit(0);
    }

    // Check if destination already exists
    int fd = open(argv[2], 0);
    if(fd >= 0) {
        close(fd);
        printf("Destination already exists: %s\n", argv[2]);
        exit(0);
    }

    // Try linking source → destination
    if(link(argv[1], argv[2]) < 0) {
        printf("Cannot link %s to %s\n", argv[1], argv[2]);
        exit(0);
    }

    // Remove source
    if(unlink(argv[1]) < 0) {
        printf("Cannot unlink %s\n", argv[1]);
        exit(0);
    }

    exit(0);
}
