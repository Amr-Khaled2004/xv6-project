/*#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"*/


/*int main() {

    int ppid=getppid();
    printf("parent PID of current process: %d\n",ppid);
    exit(0);
}*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Define pstat struct to match kernel (if not already in a header)
// Based on what you told me exists in proc.h
struct pstat {
    int pid;
    int ppid;
    int state;
    int size;       // This might be uint64 in your kernel, adjust if needed
    char name[16];
};

// Helper function to convert state number to string
static const char* state_to_str(int state) {
    switch(state) {
        case 0: return "UNUSED";
        case 1: return "USED";
        case 2: return "SLEEPING";
        case 3: return "RUNNABLE";
        case 4: return "RUNNING";
        case 5: return "ZOMBIE";
        default: return "UNKNOWN";
    }
}

int main(int argc, char *argv[]) {
    struct pstat procs[64];  // Buffer for up to 64 processes
    int max_procs = 64;
    int i, actual_procs;

    // Check for help flag (as per requirements)
    if(argc > 1) {
        if(strcmp(argv[1], "?") == 0) {
            printf("Usage: ps\n");
            printf("Display information about all running processes\n");
            printf("Output format: PID PPID STATE SIZE NAME\n");
            exit(0);
        } else {
            printf("ps: too many arguments\n");
            printf("Usage: ps\n");
            exit(1);
        }
    }

    // Call getptable system call with pstat buffer
    if(getptable(max_procs, (char*)procs) == 0) {
        printf("ps: failed to get process table\n");
        exit(1);
    }

    // Count how many valid processes we got
    // Look for first entry with pid=0 and state=0 (empty slot)
    for(actual_procs = 0; actual_procs < max_procs; actual_procs++) {
        if(procs[actual_procs].pid == 0 && procs[actual_procs].state == 0) {
            break;
        }
    }

    // Print header EXACTLY as specified: "PID PPID STATE SIZE NAME"
    printf("PID PPID STATE SIZE NAME\n");

    // Print each process
    for(i = 0; i < actual_procs; i++) {
        printf("%d %d %s %d %s\n",
               procs[i].pid,
               procs[i].ppid,
               state_to_str(procs[i].state),
               procs[i].size,
               procs[i].name);
    }

    exit(0);
}
