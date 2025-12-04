#ifndef _TIMESTRUCT_H_
#define _TIMESTRUCT_H_

struct timestruct {
    int year;
    int month;    // 1-12
    int day;      // 1-31
    int hour;     // 0-23
    int minute;   // 0-59
    int second;   // 0-59
    int weekday;  // 0-6 (Sunday=0)
};

#endif
