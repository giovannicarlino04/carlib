#include "gc_binpatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void parse_pattern(const char* pattern_str, BYTE* pattern, char* mask, SIZE_T* out_len) {
    SIZE_T i = 0;
    const char* p = pattern_str;
    
    while (*p) {
        while (*p == ' ') p++;
        
        if (!*p) break;
        
        if (*p == '?') {
            pattern[i] = 0;
            mask[i] = '?';
            if (*(p + 1) == '?') p++;
        } else {
            sscanf(p, "%2hhx", &pattern[i]);
            mask[i] = 'x';
        }
        
        i++;
        while (*p && *p != ' ') p++;
    }
    
    *out_len = i;
}

static BOOL pattern_matches(const BYTE* data, const BYTE* pattern, const char* mask, SIZE_T len) {
    for (SIZE_T i = 0; i < len; ++i) {
        if (mask[i] == 'x' && data[i] != pattern[i])
            return FALSE;
    }
    return TRUE;
}

LONGLONG gc_bin_aobscan_file(const char* path, const char* pattern_str) {
    FILE* file = fopen(path, "rb");
    if (!file) return -1;
    
    fseek(file, 0, SEEK_END);
    SIZE_T size = ftell(file);
    rewind(file);
    
    BYTE* data = (BYTE*)malloc(size);
    if (!data) {
        fclose(file);
        return -1;
    }
    
    fread(data, 1, size, file);
    fclose(file);
    
    BYTE pattern[256];
    char mask[256];
    SIZE_T pattern_len;
    parse_pattern(pattern_str, pattern, mask, &pattern_len);
    
    for (SIZE_T i = 0; i <= size - pattern_len; ++i) {
        if (pattern_matches(data + i, pattern, mask, pattern_len)) {
            free(data);
            return (LONGLONG)i;
        }
    }
    
    free(data);
    return -1;
}

BOOL gc_bin_patch_file(const char* path, LONGLONG offset, const BYTE* bytes, SIZE_T len) {
    if (!gc_bin_backup(path)) return FALSE;
    
    FILE* file = fopen(path, "rb+");
    if (!file) return FALSE;
    
    fseek(file, (long)offset, SEEK_SET);
    fwrite(bytes, 1, len, file);
    fclose(file);
    return TRUE;
}

BOOL gc_bin_patch_aob(const char* path, const char* pattern_str, const BYTE* patch, SIZE_T len) {
    LONGLONG offset = gc_bin_aobscan_file(path, pattern_str);
    if (offset == -1) return FALSE;
    return gc_bin_patch_file(path, offset, patch, len);
}

BOOL gc_bin_backup(const char* path) {
    char backup_path[MAX_PATH];
    snprintf(backup_path, MAX_PATH, "%s.bak", path);
    
    FILE* src = fopen(path, "rb");
    if (!src) return FALSE;
    
    FILE* dst = fopen(backup_path, "wb");
    if (!dst) {
        fclose(src);
        return FALSE;
    }
    
    char buf[4096];
    size_t bytes;
    while ((bytes = fread(buf, 1, sizeof(buf), src)) > 0) {
        fwrite(buf, 1, bytes, dst);
    }
    
    fclose(src);
    fclose(dst);
    return TRUE;
}
