#ifndef GC_FILE_H
#define GC_FILE_H

#include <sys/stat.h>   // stat
#include <stdio.h>
#include <errno.h>

long gc_file_size(FILE *f);
int gc_file_exists (char *filename);
int gc_file_check_extension(const char *filepath, const char *extension);
int gc_copy_file(const char *src_path, const char *dest_path);
int gc_delete_file(const char *path);
int gc_move_file(const char *src_path, const char *dest_path);
unsigned char* gc_read_file(const char *path, size_t *out_size);
void gc_read_file_lines(const char *path);

#endif