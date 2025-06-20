#include "common.h"

#ifdef _WIN32
DLLEXPORT void gc_MessageBox(const char* message, int type) {
    MessageBoxA(NULL, message, "Error", type);
}
#else
DLLEXPORT void gc_MessageBox(const char* message,  int type) {
    char* type_linux;
    switch(type){
        case MB_ICONINFORMATION:
            type_linux = "INFO: ";
            break;
        case MB_ICONWARNING:
            type_linux = "WARNING: ";
            break;
        case MB_ICONERROR:
            type_linux = "ERROR: ";
            break;
        default:
            type_linux = "INFO: ";
            break;
    }
    char str[256];
    strcpy(str, type_linux);
    strcat(str, message);
    strcat(str, "\n");
    printf(str);
}
#endif
