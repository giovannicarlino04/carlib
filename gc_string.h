#ifndef GC_STRING_H
#define GC_STRING_H

#include "gc_defs.h"
#include "gc_memory.h"
#include <Windows.h>

// Function to read a null-terminated string at a specific address.
static void gc_json_escape_string(const char* str, HANDLE out);
char *gc_strndup(const char *str, size_t n);
size_t gc_strnlen(const char *str, size_t max_len);
int ends_with(const char *str, const char *suffix);
void get_filename_without_extension(const char *file_path, char *out_name);
void gc_snprintf(char *buffer, const char *format, ...);

#endif //GC_STRING_H