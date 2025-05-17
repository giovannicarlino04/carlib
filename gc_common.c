#include "gc_common.h"
#include "gc_memory.h"

char* gc_strndup(const char *str, size_t n) {
    size_t len = strnlen(str, n);
    
    char* copy = (char*)gc_malloc(len + 1);  

    if (copy) {
        memcpy(copy, str, len);
        copy[len] = '\0';
    }

    return copy;
}

size_t gc_strnlen(const char *str, size_t max_len) {
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    return len;
}
 
int ends_with(const char *str, const char *suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (str_len < suffix_len) {
        return 0;
    }
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

void get_filename_without_extension(const char *file_path, char *out_name) {
    const char *dot = strrchr(file_path, '.');
    size_t len = dot ? (size_t)(dot - file_path) : strlen(file_path);
    strncpy(out_name, file_path, len);
    out_name[len] = '\0';
}
