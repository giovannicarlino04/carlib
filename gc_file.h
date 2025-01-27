#pragma once

#include <sys/stat.h>   // stat
#include <stdbool.h>    // bool type

bool gc_file_exists (char *filename);
bool gc_file_check_extension(const char *filepath, const char *extension);