#include "gc_string.h"
#include <stdint.h>
#include <stdio.h>

// Function to read a null-terminated string at a specific address.
char* gc_CMSReadString(FILE* file, int32_t address) {
    if (address == 0) {
        return NULL;
    }
    
    long currentPosition = ftell(file);
    fseek(file, address, SEEK_SET);
    
    char buffer[256];
    size_t i = 0;
    while (i < sizeof(buffer) - 1) {
        char c;
        if (fread(&c, 1, 1, file) != 1 || c == '\0') {
            break;
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    
    fseek(file, currentPosition, SEEK_SET);
    return strdup(buffer);
}
