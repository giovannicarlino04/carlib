#ifndef COMMON_H
#define COMMON_H

#include <windows.h>

char *gc_strndup(const char *str, size_t n);
char *gc_strlen(const char *str, size_t n);
int ends_with(const char *str, const char *suffix);
void get_filename_without_extension(const char *file_path, char *out_name);
#endif