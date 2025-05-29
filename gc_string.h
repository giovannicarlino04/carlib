#ifndef GC_STRING_H
#define GC_STRING_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>

// Function to read a null-terminated string at a specific address.
char* gc_CMSReadString(FILE* file, int32_t address);
char *gc_strndup(const char *str, size_t n);
char *gc_strlen(const char *str, size_t n);
int ends_with(const char *str, const char *suffix);
void get_filename_without_extension(const char *file_path, char *out_name);

#endif //GC_STRING_H