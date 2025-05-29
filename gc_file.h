#ifndef GC_FILE_H
#define GC_FILE_H

#include <sys/stat.h>   // stat
#include <stdbool.h>    // bool type
#include <stdio.h>

bool gc_file_exists (char *filename);
bool gc_file_check_extension(const char *filepath, const char *extension);
long gc_file_size(FILE *f);

#endif