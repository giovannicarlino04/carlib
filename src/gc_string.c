#include "common.h"


internal void gc_json_escape_string(const char* str, HANDLE out) {
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

DLLEXPORT char* gc_strndup(const char *str, size_t n) {
    size_t len = strnlen(str, n);
    
    char* copy = (char*)gc_malloc(len + 1);  
    
    if (copy) {
        memcpy(copy, str, len);
        copy[len] = '\0';
    }
    
    return copy;
}

DLLEXPORT size_t gc_strnlen(const char *str, size_t max_len) {
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    return len;
}

DLLEXPORT int gc_ends_with(const char *str, const char *suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (str_len < suffix_len) {
        return 0;
    }
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

DLLEXPORT void gc_get_filename_without_extension(const char *file_path, char *out_name) {
    const char *dot = strrchr(file_path, '.');
    size_t len = dot ? (size_t)(dot - file_path) : strlen(file_path);
    strncpy(out_name, file_path, len);
    out_name[len] = '\0';
}
//TODO: Implement a more robust version that handles floating point numbers, padding, etc.
DLLEXPORT void gc_snprintf(char *buffer, const char *format, ...) {
    va_list args;
    va_start(args, format);

    char *out = buffer;
    const char *f = format;
    while (*f) {
        if (*f == '%') {
            f++;
            if (*f == 's') {
                const char *s = va_arg(args, const char*);
                while (s && *s) *out++ = *s++;
            } else if (*f == 'd') {
                int val = va_arg(args, int);
                char tmp[16], *p = tmp + 15;
                int neg = val < 0;
                if (neg) val = -val;
                *p = '\0';
                do { *--p = '0' + (val % 10); val /= 10; } while (val);
                if (neg) *--p = '-';
                while (*p) *out++ = *p++;
            } else if (*f == 'u') {
                unsigned int val = va_arg(args, unsigned int);
                char tmp[16], *p = tmp + 15;
                *p = '\0';
                do { *--p = '0' + (val % 10); val /= 10; } while (val);
                while (*p) *out++ = *p++;
            } else if (*f == 'x') {
                unsigned int val = va_arg(args, unsigned int);
                char tmp[16], *p = tmp + 15;
                *p = '\0';
                do {
                    int digit = val % 16;
                    *--p = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
                    val /= 16;
                } while (val);
                while (*p) *out++ = *p++;
            } else if (*f == 'c') {
                char c = (char)va_arg(args, int);
                *out++ = c;
            } else if (*f == '%') {
                *out++ = '%';
            }
            f++;
        } else {
            *out++ = *f++;
        }
    }
    *out = '\0';
    va_end(args);
}