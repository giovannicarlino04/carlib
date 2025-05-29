#include "gc_string.h"
#include <stdint.h>
#include <stdio.h>

// Helper for escaping strings
static void gc_json_escape_string(const char* str, FILE* out) {
    fputc('"', out);
    for (const char* p = str; *p; ++p) {
        switch (*p) {
            case '\\': fputs("\\\\", out); break;
            case '"':  fputs("\\\"", out); break;
            case '\n': fputs("\\n", out); break;
            case '\r': fputs("\\r", out); break;
            case '\t': fputs("\\t", out); break;
            default:
            if ((unsigned char)*p < 32)
                fprintf(out, "\\u%04x", *p);
            else
                fputc(*p, out);
        }
    }
    fputc('"', out);
}

// Function to read a null-terminated string at a specific address.
char* gc_CMSReadString(FILE* file, int32_t address) {
    if (address == 0) {
        return NULL;
    }
    
    long currentPosition = ftell(file);
    fseek(file, address, SEEK_SET);
    
    char buffer[256];
    size_t i = 0;
    while (i < sizeof(buffer) - 1) {
        char c;
        if (fread(&c, 1, 1, file) != 1 || c == '\0') {
            break;
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';
    
    fseek(file, currentPosition, SEEK_SET);
    return strdup(buffer);
}

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
