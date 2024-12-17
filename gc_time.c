#include "gc_time.h"

DWORD gc_ns_to_ms(LONGLONG nsec) {
    return (DWORD)(nsec / 1000000L);
}

int gc_nanosleep(time_t sec, long nsec) {
    LARGE_INTEGER frequency, start, current;
    LONGLONG elapsed_ns;
    LONGLONG total_ns = (LONGLONG)sec * BILLION + nsec;

    if (nsec < 0 || nsec >= BILLION) {
        fprintf(stderr, "Invalid nanoseconds value. Must be between 0 and 999999999.\n");
        return -1;
    }

    if (!QueryPerformanceFrequency(&frequency)) {
        fprintf(stderr, "High-resolution timer not available.\n");
        return -1;
    }

    QueryPerformanceCounter(&start);

    while (1) {
        QueryPerformanceCounter(&current);

        elapsed_ns = (current.QuadPart - start.QuadPart) * BILLION / frequency.QuadPart;

        if (elapsed_ns >= total_ns) {
            break;
        }

        DWORD remaining_ms = gc_ns_to_ms((LONGLONG)(total_ns - elapsed_ns));
        if (remaining_ms > 0) {
            Sleep(remaining_ms);
        }
    }

    return 0; 
}

// Function to get the current time (in seconds)
double gc_getCurrentTime() {
    return (double)clock() / CLOCKS_PER_SEC;
}

// Function to get the elapsed time (in seconds) since start_time
double gc_getElapsedTime(Timer* timer) {
    return ((double)(clock() - timer->start_time)) / CLOCKS_PER_SEC;
}

// Function to sleep for a specified number of seconds (supports fractional seconds)
void gc_sleepFor(double seconds) {
    struct timespec ts;
    ts.tv_sec = (time_t)seconds;  // seconds part
    ts.tv_nsec = (long)((seconds - ts.tv_sec) * 1000000000L);  // nanoseconds part
    gc_nanosleep(ts.tv_sec, ts.tv_nsec);
}

// Function to create and initialize a timer
void gc_startTimer(Timer* timer) {
    timer->start_time = clock();
}
