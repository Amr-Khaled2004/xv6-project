#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    uint64 ticks;
    int seconds;
    int minutes;
    int hours;
    int days;

    // Check for help flag
    if (argc > 1) {
        if (strcmp(argv[1], "?") == 0) {
            printf("Usage: uptime\n");
            printf("Display system uptime in ticks and human-readable format\n");
            exit(0);
        } else {
            printf("uptime: too many arguments\n");
            printf("Usage: uptime\n");
            exit(1);
        }
    }

    // Call system call
    ticks = uptime();

    // Convert ticks to human-readable format
    // ticks are at 100Hz (100 ticks per second) in xv6
    seconds = ticks / 100;
    minutes = seconds / 60;
    hours = minutes / 60;
    days = hours / 24;

    // Calculate remainders
    seconds = seconds % 60;
    minutes = minutes % 60;
    hours = hours % 24;

    // Display uptime information
    printf("System uptime:\n");
    printf("  Ticks: %d\n", (int)ticks);

    printf("  Time: ");

    if (days > 0) {
        printf("%d day%s, ", days, (days == 1) ? "" : "s");
    }

    if (hours > 0 || days > 0) {
        printf("%d hour%s, ", hours, (hours == 1) ? "" : "s");
    }

    if (minutes > 0 || hours > 0 || days > 0) {
        printf("%d minute%s, ", minutes, (minutes == 1) ? "" : "s");
    }

    printf("%d second%s\n", seconds, (seconds == 1) ? "" : "s");

    exit(0);
}
