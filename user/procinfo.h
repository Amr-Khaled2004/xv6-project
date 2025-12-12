#ifndef PROCINFO_H
#define PROCINFO_H
#include "types.h"
#define PROC_NAME_LEN 16

struct procinfo {
  int pid;
  int ppid;
  int state;
  uint64 size;
  char name[PROC_NAME_LEN];
};

#endif
