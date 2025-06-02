#include "gc_file.h"
#include "gc_memory.h"
#include <stdio.h>
#include <sys/stat.h>

long gc_file_size(FILE *f) {
    if (!f) return -1;
    long cur = ftell(f);
    if (cur == -1) return -1;
    if (fseek(f, 0, SEEK_END) != 0) return -1;
    long size = ftell(f);
    fseek(f, cur, SEEK_SET);
    return size;
}

int gc_file_exists (char *filename)
{
    struct stat buffer;   
    return (stat (filename, &buffer) == 0);
}

int gc_file_check_extension(const char *filepath, const char *extension) {
    const char *srcExtension = strrchr(filepath, '.'); 
    
    if (srcExtension != NULL) {
        return strcmp(srcExtension, extension) == 0; 
    }
    
    return 0; // Nessuna estensione trovata, non è un file .{Estensione}
}

// Reads entire file into a malloc'd buffer.
// On success, returns pointer to buffer, sets *out_size to file size.
// On failure, returns NULL, *out_size is 0.
// Caller must free() returned buffer.
unsigned char* gc_read_file(const char *path, size_t *out_size) {
    if (out_size) *out_size = 0;
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }

    long size = ftell(f);
    if (size < 0) {
        fclose(f);
        return NULL;
    }
    rewind(f);

    unsigned char *buffer = (unsigned char*)gc_malloc(size);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, size, f);
    fclose(f);

    if (read_bytes != (size_t)size) {
        free(buffer);
        return NULL;
    }

    if (out_size) *out_size = size;
    return buffer;
}

//(caller responsibility to handle)
void gc_read_file_lines(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        printf("Line: %s", line);
    }

    fclose(f);
}

int gc_copy_file(const char *src_path, const char *dest_path) {
    FILE *src = fopen(src_path, "rb");
    if (!src) {
        perror("Failed to open source file");
        return 1;
    }

    FILE *dest = fopen(dest_path, "wb");
    if (!dest) {
        perror("Failed to open destination file");
        fclose(src);
        return 1;
    }

    char buffer[4096];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dest) != bytes) {
            perror("Failed to write to destination file");
            fclose(src);
            fclose(dest);
            return 1;
        }
    }

    fclose(src);
    fclose(dest);
    return 0;
}


int gc_move_file(const char *src_path, const char *dest_path) {
    if (rename(src_path, dest_path) == 0) {
        return 0; 
    }

    if (errno != EXDEV) {  // EXDEV = cross-device link error
        perror("Failed to rename file");
        return 1;
    }

    if (gc_copy_file(src_path, dest_path) != 0) {
        return 1;
    }

    if (remove(src_path) != 0) {
        perror("Failed to delete source file after copy");
        return 1;
    }

    return 0;
}

int gc_delete_file(const char *path) {
    if (remove(path) != 0) {
        perror("Failed to delete file");
        return 1;
    }
    return 0;
}
