#include "debug.h"

// Function definition for Windows platform
#ifdef _WIN32
void MBError(const char* message) {
    MessageBoxA(NULL, message, "Error", MB_ICONERROR | MB_OK);
}
#elif __unix__ || __APPLE__
void MBError(const char* message) {
    //Poor approach blah blah- dude, I know
    printf("Error: %s\n", message);
}
#endif
