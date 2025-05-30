#include "gc_string.h"

static void gc_json_escape_string(const char* str, HANDLE out) {
    const char* p = str;
    DWORD written;
    char c;
    
    c = '"';
    WriteFile(out, &c, 1, &written, NULL);

    while (*p) {
        switch (*p) {
            case '\"': WriteFile(out, "\\\"", 2, &written, NULL); break;
            case '\\': WriteFile(out, "\\\\", 2, &written, NULL); break;
            case '\b': WriteFile(out, "\\b", 2, &written, NULL); break;
            case '\f': WriteFile(out, "\\f", 2, &written, NULL); break;
            case '\n': WriteFile(out, "\\n", 2, &written, NULL); break;
            case '\r': WriteFile(out, "\\r", 2, &written, NULL); break;
            case '\t': WriteFile(out, "\\t", 2, &written, NULL); break;
            default:
                if ((unsigned char)*p < 0x20) {
                    // Control characters as \u00XX
                    char buf[7];
                    wsprintfA(buf, "\\u%04X", (unsigned char)*p);
                    WriteFile(out, buf, 6, &written, NULL);
                } else {
                    WriteFile(out, p, 1, &written, NULL);
                }
                break;
        }
        p++;
    }

    c = '"';
    WriteFile(out, &c, 1, &written, NULL);
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
