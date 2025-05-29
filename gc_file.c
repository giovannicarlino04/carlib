#include "gc_file.h"

bool gc_file_exists (char *filename)
{
    struct stat   buffer;   
    return (stat (filename, &buffer) == 0);
}

bool gc_file_check_extension(const char *filepath, const char *extension) {
    const char *srcExtension = strrchr(filepath, '.'); 
    
    if (srcExtension != NULL) {
        return strcmp(srcExtension, extension) == 0; 
    }
    
    return 0; // Nessuna estensione trovata, non è un file .{Estensione}
}

long gc_file_size(FILE *f) {
    if (!f) return -1;
    long cur = ftell(f);
    if (cur == -1) return -1;
    if (fseek(f, 0, SEEK_END) != 0) return -1;
    long size = ftell(f);
    fseek(f, cur, SEEK_SET);
    return size;
}
