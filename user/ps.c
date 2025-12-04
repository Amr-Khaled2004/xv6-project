#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"


int main() {

    int ppid=getppid();
    printf("parent PID of current process: %d\n",ppid);
    exit(0);
}
