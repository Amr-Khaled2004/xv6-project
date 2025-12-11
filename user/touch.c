#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if(argc == 2 && strcmp(argv[1], "?") == 0) {
        printf("Usage: touch filename\n");
        exit(0);
    }

    if(argc != 2) {
        printf("Invalid command. Usage: touch filename\n");
        exit(0);
    }

    // First try opening WITHOUT O_CREATE to check if file exists
    int fd = open(argv[1], O_RDONLY);
    if(fd >= 0) {
        // File already exists
        printf("touch: file %s already exists\n", argv[1]);
        close(fd);
        exit(0);
    }

    // File does NOT exist, so create it
    fd = open(argv[1], O_CREATE | O_RDWR);
    if(fd < 0) {
        printf("touch: cannot create file %s\n", argv[1]);
        exit(0);
    }

    close(fd);
    exit(0);
}
