#ifndef GC_TIME_H
#define GC_TIME_H

#include <windows.h>

// High-resolution timer struct
typedef struct Timer {
    LARGE_INTEGER start_time;
} Timer;

// Returns current time in seconds (double-precision)
double gc_getCurrentTime(void);

// Returns elapsed time in seconds since timer started
double gc_getElapsedTime(Timer* timer);

// Sleeps for specified duration in seconds
void gc_sleepFor(double seconds);

// Starts the timer
void gc_startTimer(Timer* timer);

#endif // GC_TIME_H
