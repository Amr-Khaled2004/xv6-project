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
#define CLINT 0x02000000L
#define CLINT_MTIME (CLINT + 0xBFF8)
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


// Helper function to convert UNIX timestamp to timestruct
static void unix_to_timestruct(uint64 unix_time, struct timestruct *ts) {
    // Constants for time calculations
    const int days_per_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Calculate total days since epoch
    uint64 total_days = unix_time / 86400;
    uint64 remaining_seconds = unix_time % 86400;

    // Calculate time of day
    ts->hour = remaining_seconds / 3600;
    ts->minute = (remaining_seconds % 3600) / 60;
    ts->second = remaining_seconds % 60;

    // Calculate weekday (1970-01-01 was Thursday)
    ts->weekday = (total_days + 4) % 7;

    // Calculate year and month
    int year = 1970;
    int days_in_year;

    while (1) {
        // Check for leap year
        int is_leap = 0;
        if (year % 4 == 0) {
            if (year % 100 != 0 || year % 400 == 0) {
                is_leap = 1;
            }
        }

        days_in_year = is_leap ? 366 : 365;

        if (total_days < days_in_year) {
            break;
        }

        total_days -= days_in_year;
        year++;
    }

    ts->year = year;

    // Calculate month and day
    int month = 0;
    int is_leap = 0;

    // Check if current year is leap
    if (ts->year % 4 == 0) {
        if (ts->year % 100 != 0 || ts->year % 400 == 0) {
            is_leap = 1;
        }
    }

    while (month < 12) {
        int days_in_month = days_per_month[month];

        // Adjust for February in leap years
        if (month == 1 && is_leap) {
            days_in_month = 29;
        }

        if (total_days < days_in_month) {
            break;
        }

        total_days -= days_in_month;
        month++;
    }

    ts->month = month + 1;  // Convert to 1-based
    ts->day = total_days + 1;  // Convert to 1-based
}

// Read time using RISC-V time CSR (most reliable)
static uint64 read_time_csr(void) {
    uint64 time;
    // Read the time CSR
    asm volatile("csrr %0, time" : "=r"(time));
    return time;
}

uint64 sys_datetime(void) {
    struct timestruct ts;
    uint64 addr;

    // Get user buffer address
    argaddr(0, &addr);

    if (addr == 0) {
        return -1;
    }

    // Read current time in cycles
    uint64 time_cycles = read_time_csr();

    // Convert cycles to seconds
    // QEMU virt machine: time CSR typically runs at 10 MHz
    // 10,000,000 cycles = 1 second
    uint64 seconds_since_boot = time_cycles / 10000000;

    // Calculate current UNIX time
    uint64 current_unix_time = BOOT_EPOCH + seconds_since_boot;

    // Convert UNIX time to timestruct
    unix_to_timestruct(current_unix_time, &ts);

    // Copy to user space
    if (copyout(myproc()->pagetable, addr, (char *)&ts, sizeof(ts)) < 0) {
        return -1;
    }

    return 0;
}
