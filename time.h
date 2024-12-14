#pragma once

#include <stdio.h>
#include <time.h>


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

double getCurrentTime();
double getElapsedTime(Timer* timer);
void sleepFor(double seconds);
void startTimer(Timer* timer);