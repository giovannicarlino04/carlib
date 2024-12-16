#include "gc_common.h"
#include "gc_memory.h"

// strndup - Duplicate a string up to a specified length
char* gc_strndup(const char *str, size_t n) {
    // Determine the actual length of the string up to the maximum n characters
    size_t len = strnlen(str, n);
    
    // Allocate memory for the string and the null terminator
    char* copy = (char*)gc_malloc(len + 1);  // Allocate space for the string and '\0'

    if (copy) {
        // Copy the string up to len characters
        memcpy(copy, str, len);
        // Ensure the string is null-terminated
        copy[len] = '\0';
    }

    // Return the copied string or NULL if allocation failed
    return copy;
}

// strnlen - Find the length of a string up to a maximum length n
size_t gc_strnlen(const char *str, size_t max_len) {
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    return len;
}
