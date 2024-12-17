#include "gc_time.h"
#include <math.h> 

// Function to get the elapsed time (in seconds) since start_time
double gc_getElapsedTime(Timer* timer) {
    return ((double)(clock() - timer->start_time)) / CLOCKS_PER_SEC;
}

void gc_sleepFor(double seconds) {
    struct timespec ts;
    ts.tv_sec = (time_t)floor(seconds);  // Integer part (seconds)
    ts.tv_nsec = (long)((seconds - ts.tv_sec) * 1000000000L);  // Fractional part (nanoseconds)
    nanosleep(&ts, NULL);
}

// Function to create and initialize a timer
void gc_startTimer(Timer* timer) {
    timer->start_time = clock();
}
