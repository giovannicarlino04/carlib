#pragma once

#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <math.h> 

#define BILLION 1000000000L

#ifdef _POSIX_C_SOURCE
#include <unistd.h>  // For usleep on Unix-like systems
#endif

// Utility macros for platform compatibility
#ifdef _WIN32
    // Windows sleep function, in milliseconds
    #define sleep_ms(ms) _sleep(ms)
#else
    // Unix-like systems sleep function, in microseconds
    #define sleep_ms(ms) usleep(ms * 1000)
#endif

// Timer structure to store start time
typedef struct Timer {
    clock_t start_time;
} Timer;

double gc_getCurrentTime();
double gc_getElapsedTime(Timer* timer);
void gc_sleepFor(double seconds);
void gc_startTimer(Timer* timer);