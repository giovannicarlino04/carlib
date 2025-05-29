#ifndef GC_STRING_H
#define GC_STRING_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>

// Function to read a null-terminated string at a specific address.
char* gc_CMSReadString(FILE* file, int32_t address);

#endif //GC_STRING_H