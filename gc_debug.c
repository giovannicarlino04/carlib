#include "gc_debug.h"

// Function definition for Windows platform
#ifdef _WIN32
void gc_MessageBox(const char* message, int type) {
    MessageBoxA(NULL, message, "Error", type);
}
#else
void gc_MessageBox(const char* message,  int type) {
    printf(message);
}
#endif
