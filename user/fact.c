#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    // Help
    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        printf("Usage: fact number\n");
        exit(0);
    }

    // Wrong number of parameters
    if (argc != 2) {
        printf("You can only get the factorial of a single number\n");
        exit(0);
    }

    // Detect negative number manually since xv6 atoi() can't
    if (argv[1][0] == '-') {
        printf("Negative numbers not allowed\n");
        exit(0);
    }

    int n = atoi(argv[1]);

    int res = 1;
    for (int i = 1; i <= n; i++) {
        res *= i;
    }

    printf("%d\n", res);
    exit(0);
}
