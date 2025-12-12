#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "timestruct.h"

#ifndef BOOT_EPOCH
// Fallback if Makefile didn't supply -DBOOT_EPOCH
#define BOOT_EPOCH 0
#endif

extern int kbd_intr_count;
extern int syscall_count;


uint64
sys_kbdint(void)
{
  return kbd_intr_count;
}
uint64
sys_countsyscall(void)
{
  return syscall_count;
}


// Park-Miller minimal standard random generator


extern uint ticks;   // system uptime ticks (declared in timer.c)

// LCG state (static global in kernel)
static unsigned long rand_seed = 1;

// Linear congruential generator
static unsigned long
lcg_next(void)
{
    // LCG: new = old * a + c
    // a = 1103515245, c = 12345 (classic ANSI C parameters)
    rand_seed = rand_seed * 1103515245UL + 12345UL;
    // return a non-negative 31-bit value
    return (rand_seed >> 1) & 0x7fffffffUL;
}

uint64
sys_urand(void)
{
    // Seed on first call with ticks (so different runs produce different sequences)
    if (rand_seed == 1) {
        // Use ticks to get some entropy; ensure it's non-zero
        if (ticks != 0)
            rand_seed = ticks;
        else
            rand_seed = 1103515245UL ^ 12345UL; // fallback seed
    }

    return (uint64)lcg_next();
}
static inline uint64
sbi_call_alt(uint64 ext, uint64 func, uint64 arg0, uint64 arg1, uint64 arg2)
{
    register uint64 a0 asm("a0") = arg0;
    register uint64 a1 asm("a1") = arg1;
    register uint64 a2 asm("a2") = arg2;
    register uint64 a6 asm("a6") = func;
    register uint64 a7 asm("a7") = ext;
    asm volatile("ecall"
                 : "+r"(a0)
                 : "r"(a1), "r"(a2), "r"(a6), "r"(a7)
                 : "memory");
    return a0;
}


uint64 sys_shutdown(void) {
    printf("xv6: System shutdown initiated\n");
    printf("Goodbye!\n");

    // Method 1: Standard QEMU virt machine poweroff
    // Write magic value 0x5555 to test device at 0x100000
    volatile uint32 *poweroff = (volatile uint32*)TEST;
    *poweroff = 0x5555;

    // Memory barrier to ensure write completes
    asm volatile("fence" ::: "memory");

    // Short delay
    for(volatile int i = 0; i < 10000; i++);

    // If still running, print message
    printf("xv6: Shutdown command sent to QEMU\n");
    printf("xv6: If QEMU doesn't exit, press Ctrl-A X\n");

    // Halt CPU
    for(;;) {
        asm volatile("wfi");  // Wait for interrupt
    }

    return 0;
}
static uint64 read_mtime(void) {
    // CLINT_MTIME is memory-mapped at 0x2000000 + 0xBFF8
    volatile uint64 *mtime_ptr = (volatile uint64*)CLINT_MTIME;
    return *mtime_ptr;
}

// Convert UNIX timestamp to datetime with timezone
static void unix_to_datetime(uint64 unix_time, struct timestruct *dt) {
    // Constants
    const int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Egypt is UTC+2 (7200 seconds)
    uint64 local_time = unix_time + 7200;

    // Calculate days and seconds
    uint64 total_days = local_time / 86400;
    uint64 remaining_seconds = local_time % 86400;

    // Calculate time of day
    dt->hour = remaining_seconds / 3600;
    dt->minute = (remaining_seconds % 3600) / 60;
    dt->second = remaining_seconds % 60;

    // Calculate weekday (1970-01-01 was Thursday)
    dt->weekday = (total_days + 4) % 7;

    // Calculate year
    int year = 1970;

    while (1) {
        // Check leap year
        int is_leap = (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
        int days_in_year = is_leap ? 366 : 365;

        if (total_days < days_in_year) {
            break;
        }

        total_days -= days_in_year;
        year++;
    }

    dt->year = year;

    // Calculate month
    int month = 0;
    int is_leap = (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);

    while (month < 12) {
        int days_in_month = days_per_month[month];
        if (month == 1 && is_leap) {  // February
            days_in_month = 29;
        }

        if (total_days < days_in_month) {
            break;
        }

        total_days -= days_in_month;
        month++;
    }

    dt->month = month + 1;  // 1-based
    dt->day = total_days + 1;  // 1-based
}

uint64 sys_datetime(void) {
    struct timestruct dt;
    uint64 addr;

    // Get user buffer address
    argaddr(0, &addr);

    if (addr == 0) {
        return -1;
    }

    // Read mtime register as per requirements
    uint64 mtime_cycles = read_mtime();

    // Convert cycles to seconds
    // QEMU virt machine: mtime runs at 10 MHz (10,000,000 cycles/sec)
    uint64 seconds_since_boot = mtime_cycles / 10000000;

    // Calculate current UNIX time
    uint64 current_unix_time = BOOT_EPOCH + seconds_since_boot;

    // Convert to datetime (Egypt time UTC+2)
    unix_to_datetime(current_unix_time, &dt);

    // Copy to user space
    if (copyout(myproc()->pagetable, addr, (char *)&dt, sizeof(dt)) < 0) {
        return -1;
    }

    return 0;
}
