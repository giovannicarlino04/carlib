#include "common.h"

static LARGE_INTEGER frequency;

// High-resolution timer struct
typedef struct Timer {
    LARGE_INTEGER start_time;
} Timer;

// Initialize performance frequency only once
internal void init_frequency() {
    if (frequency.QuadPart == 0) {
        QueryPerformanceFrequency(&frequency);
    }
}

DLLEXPORT double gc_getCurrentTime(void) {
    init_frequency();
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)now.QuadPart / frequency.QuadPart;
}

DLLEXPORT void gc_startTimer(Timer* timer) {
    init_frequency();
    QueryPerformanceCounter(&timer->start_time);
}

DLLEXPORT double gc_getElapsedTime(Timer* timer) {
    init_frequency();
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - timer->start_time.QuadPart) / frequency.QuadPart;
}

DLLEXPORT void gc_sleepFor(double seconds) {
    DWORD ms = (DWORD)(seconds * 1000.0);
    Sleep(ms);
}
