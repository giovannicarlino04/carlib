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