#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"

extern int getptable(int nproc, char *buffer);

extern struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;
  argint(0, &n);
  addr = myproc()->sz;

  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;

  acquire(&tickslock);
  ticks0 = ticks;

  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }

  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;
  argint(0, &pid);
  return kill(pid);
}

uint64
sys_uptime(void)
{
  uint xticks;
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_getppid(void)
{
  struct proc *p = myproc();
  if (p->parent)
    return p->parent->pid;
  return -1;
}

// -------------------------------
// YOUR SYSCALL: setpriority()
// -------------------------------
uint64
sys_setpriority(void)
{
  int pid, newprio;

  argint(0, &pid);
  argint(1, &newprio);

  return setpriority(pid, newprio);
}

// -------------------------------
// AMR'S SYSCALL: getptable()
// -------------------------------
uint64
sys_getptable(void)
{
  int nproc;
  uint64 buffer;

  argint(0, &nproc);
  argaddr(1, &buffer);

  return getptable(nproc, (char*)buffer);
}
