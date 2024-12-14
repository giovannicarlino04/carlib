#include "time.h"

// Function to get the current time (in seconds)
double getCurrentTime() {
    return (double)clock() / CLOCKS_PER_SEC;
}

// Function to get the elapsed time (in seconds) since start_time
double getElapsedTime(Timer* timer) {
    return ((double)(clock() - timer->start_time)) / CLOCKS_PER_SEC;
}

// Function to sleep for a specified number of seconds (supports fractional seconds)
void sleepFor(double seconds) {
    struct timespec ts;
    ts.tv_sec = (time_t)seconds;  // seconds part
    ts.tv_nsec = (long)((seconds - ts.tv_sec) * 1000000000L);  // nanoseconds part
    nanosleep(&ts, NULL);
}

// Function to create and initialize a timer
void startTimer(Timer* timer) {
    timer->start_time = clock();
}
