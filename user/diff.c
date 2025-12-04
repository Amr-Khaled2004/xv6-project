#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXLINE 512

// Read one line into buf, return length, 0 on EOF
int readline(int fd, char *buf) {
    int i = 0;
    char c;

    while (read(fd, &c, 1) == 1) {
        if (c == '\n') {
            buf[i++] = c;
            break;
        }
        if (i < MAXLINE - 1)
            buf[i++] = c;
    }

    buf[i] = '\0';
    return i;
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        printf("Usage: diff file1 file2\n");
        exit(0);
    }

    if (argc != 3) {
        printf("diff: requires two files\n");
        exit(1);
    }

    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) {
        printf("diff: cannot open %s\n", argv[1]);
        exit(1);
    }

    int fd2 = open(argv[2], O_RDONLY);
    if (fd2 < 0) {
        printf("diff: cannot open %s\n", argv[2]);
        close(fd1);
        exit(1);
    }

    char line1[MAXLINE];
    char line2[MAXLINE];

    int len1, len2;
    int line_no = 1;
    int differences = 0;

    while (1) {
        len1 = readline(fd1, line1);
        len2 = readline(fd2, line2);

        if (len1 == 0 && len2 == 0)
            break;

        if (len1 > 0 && len2 > 0) {
            if (strcmp(line1, line2) != 0) {
                differences = 1;
                printf("Line %d differs:\n", line_no);
                printf("< %s", line1);
                printf("> %s", line2);
                printf("\n");
            }
        } else if (len1 > 0 && len2 == 0) {
            differences = 1;
            printf("Line %d only in %s:\n", line_no, argv[1]);
            printf("< %s\n", line1);
        } else if (len2 > 0 && len1 == 0) {
            differences = 1;
            printf("Line %d only in %s:\n", line_no, argv[2]);
            printf("> %s\n", line2);
        }

        line_no++;
    }

    close(fd1);
    close(fd2);

    if (!differences)
        printf("Files are identical\n");

    exit(0);
}
