#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *filename);

int main(int argc, char *argv[]) {
    if(argc == 2 && strcmp(argv[1], "?") == 0) {
        printf("Usage: find directory filename\n");
        exit(0);
    }

    if(argc != 3) {
        printf("Invalid command. Usage: find directory filename\n");
        exit(0);
    }

    find(argv[1], argv[2]);
    exit(0);
}

void find(char *path, char *filename) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0) {
        printf("Cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0) {
        printf("Cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
    case T_FILE:
        if(strcmp(path + strlen(path) - strlen(filename), filename) == 0)
            printf("%s\n", path);
        break;
    case T_DIR:
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0) continue;
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
            memmove(p, de.name, strlen(de.name));
            p[strlen(de.name)] = 0;
            find(buf, filename);
        }
        break;
    }
    close(fd);
}
