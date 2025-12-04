#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char buf[512];

int main(int argc, char *argv[]) {
    if(argc == 2 && strcmp(argv[1], "?") == 0) {
        printf("Usage: cp source destination\n");
        exit(0);
    }

    if(argc != 3) {
        printf("Invalid command. Usage: cp source destination\n");
        exit(0);
    }

    int fd1 = open(argv[1], 0);
    if(fd1 < 0) {
        printf("Cannot open %s\n", argv[1]);
        exit(0);
    }

    int fd2 = open(argv[2], O_CREATE | O_WRONLY);
    if(fd2 < 0) {
        printf("Cannot create %s\n", argv[2]);
        close(fd1);
        exit(0);
    }

    int n;
    while((n = read(fd1, buf, sizeof(buf))) > 0) {
        if(write(fd2, buf, n) != n) {
            printf("Write error\n");
            close(fd1); close(fd2);
            exit(0);
        }
    }

    close(fd1);
    close(fd2);
    exit(0);
}
