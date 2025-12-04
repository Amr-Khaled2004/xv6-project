#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int c = countsyscall();
  printf("Total system calls since boot: %d\n", c);
  exit(0);
}
