#ifndef COMMON_H
#define COMMON_H

#include <string.h>

#define uint32 uint32_t
#define uint64 uint64_t
#define uchar unsigned char

char *gc_strndup(const char *str, size_t n);
char *gc_strlen(const char *str, size_t n);
int ends_with(const char *str, const char *suffix);
void get_filename_without_extension(const char *file_path, char *out_name);
#endif