#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "timestruct.h"

// Helper function to print weekday name
static const char* weekday_name(int weekday) {
    static const char* names[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };

    if (weekday >= 0 && weekday < 7) {
        return names[weekday];
    }
    return "Unknown";
}

// Helper function to print month name
static const char* month_name(int month) {
    static const char* names[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    if (month >= 1 && month <= 12) {
        return names[month - 1];
    }
    return "Unknown";
}

int main(int argc, char *argv[]) {
    struct timestruct ts;

    // Check for help flag
    if (argc > 1) {
        if (strcmp(argv[1], "?") == 0) {
            printf("Usage: datetime\n");
            printf("Display current date and time\n");
            exit(0);
        } else {
            printf("datetime: too many arguments\n");
            printf("Usage: datetime\n");
            exit(1);
        }
    }

    // Call system call
    if (datetime(&ts) < 0) {
        printf("datetime: failed to get date/time\n");
        exit(1);
    }

    // Format and print date/time
    printf("%s, %s %d, %d ",
           weekday_name(ts.weekday),
           month_name(ts.month),
           ts.day,
           ts.year);

    // Print time with manual zero-padding
    // xv6's printf might not support %02d format specifier
    if (ts.hour < 10) printf("0");
    printf("%d:", ts.hour);
    if (ts.minute < 10) printf("0");
    printf("%d:", ts.minute);
    if (ts.second < 10) printf("0");
    printf("%d\n", ts.second);

    exit(0);
}
