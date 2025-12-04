#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // Check for help
    if (argc > 1) {
        if (strcmp(argv[1], "?") == 0) {
            printf("Usage: shutdown\n");
            printf("Shut down the xv6 operating system\n");
            exit(0);
        } else {
            printf("shutdown: too many arguments\n");
            printf("Usage: shutdown\n");
            exit(1);
        }
    }

    printf("Initiating system shutdown...\n");
    shutdown();

    // Should not reach here
    printf("shutdown: failed\n");
    exit(1);
}
