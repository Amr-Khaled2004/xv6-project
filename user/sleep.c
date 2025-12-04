#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

  if(argc == 2 && strcmp(argv[1], "?") == 0) {
    printf("Usage: sleep number\n");
    exit(0);
  }

  if(argc != 2) {
    printf("Invalid command, you can only use a single number\n");
    exit(0);
  }

  int sleeptime = atoi(argv[1]);

  printf("Starting sleep for %d\n", sleeptime);
  sleep(sleeptime);
  printf("Finished sleeping...\n");

  exit(0);
}
