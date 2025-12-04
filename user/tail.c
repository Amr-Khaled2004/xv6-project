#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define BSIZE 512

char buf[BSIZE];

int
main(int argc, char *argv[])
{
    int fd;
    int nlines = 10;  // default lines count

    // Help message
    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        printf("Usage: tail [-n num] file\n");
        exit(0);
    }

    int argi = 1;

    // Parse -n argument
    if (argc > 2 && strcmp(argv[1], "-n") == 0) {
        nlines = atoi(argv[2]);
        argi = 3;
    }

    if (argi >= argc) {
        printf("tail: missing filename\n");
        exit(1);
    }

    // Open file
    fd = open(argv[argi], O_RDONLY);
    if (fd < 0) {
        printf("tail: cannot open %s\n", argv[argi]);
        exit(1);
    }

    // PASS 1: count total lines
    int total = 0;
    int n;

    while ((n = read(fd, buf, BSIZE)) > 0) {
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n')
                total++;
        }
    }

    close(fd);

    // Reopen file
    fd = open(argv[argi], O_RDONLY);
    if (fd < 0) exit(1);

    // Compute how many lines to skip
    int skip = total - nlines;
    if (skip < 0) skip = 0;

    int passed = 0;

    // PASS 2: skip lines then print
    while ((n = read(fd, buf, BSIZE)) > 0) {
        for (int i = 0; i < n; i++) {

            // Start printing only AFTER skipping
            if (passed >= skip)
                write(1, &buf[i], 1);

            // Count newline AFTER printing/write logic
            if (buf[i] == '\n')
                passed++;
        }
    }

    close(fd);
    exit(0);
}
