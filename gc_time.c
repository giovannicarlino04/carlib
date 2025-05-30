#include "gc_time.h"

static LARGE_INTEGER frequency;

// Initialize performance frequency only once
static void init_frequency() {
    if (frequency.QuadPart == 0) {
        QueryPerformanceFrequency(&frequency);
    }
}

double gc_getCurrentTime(void) {
    init_frequency();
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)now.QuadPart / frequency.QuadPart;
}

void gc_startTimer(Timer* timer) {
    init_frequency();
    QueryPerformanceCounter(&timer->start_time);
}

double gc_getElapsedTime(Timer* timer) {
    init_frequency();
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - timer->start_time.QuadPart) / frequency.QuadPart;
}

void gc_sleepFor(double seconds) {
    DWORD ms = (DWORD)(seconds * 1000.0);
    Sleep(ms);
}
