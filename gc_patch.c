#include "gc_patch.h"
#include <string.h>
#include <stdio.h>

static BOOL gc_unprotect_memory(BYTE* address, SIZE_T len, DWORD* oldProtect) {
    return VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, oldProtect);
}

static void gc_reprotect_memory(BYTE* address, SIZE_T len, DWORD oldProtect) {
    DWORD _;
    VirtualProtect(address, len, oldProtect, &_);
}

// Helper to convert pattern string to bytes
static BOOL gc_pattern_match(const BYTE* data, const BYTE* pattern, const char* mask, SIZE_T len) {
    for (SIZE_T i = 0; i < len; i++) {
        if (mask[i] == 'x' && data[i] != pattern[i]) return FALSE;
    }
    return TRUE;
}

static void gc_parse_pattern(const char* pattern_str, BYTE* pattern, char* mask, SIZE_T* out_len) {
    SIZE_T i = 0;
    const char* p = pattern_str;
    
    while (*p) {
        while (*p == ' ') p++;
        
        if (!*p) break;
        
        if (*p == '?') {
            pattern[i] = 0x00;
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

BYTE* gc_aobscan(const char* pattern_str, BYTE* start, SIZE_T size) {
    BYTE pattern[256];
    char mask[256];
    SIZE_T pattern_len = 0;
    
    gc_parse_pattern(pattern_str, pattern, mask, &pattern_len);
    if (pattern_len == 0) return NULL;
    
    for (SIZE_T i = 0; i <= size - pattern_len; i++) {
        if (gc_pattern_match(start + i, pattern, mask, pattern_len)) {
            return start + i;
        }
    }
    
    return NULL;
}
BYTE* gc_aobscan_external(const char* pattern_str, BYTE* start, SIZE_T size, HANDLE process) {
    BYTE pattern[256];
    char mask[256];
    SIZE_T pattern_len = 0;
    
    gc_parse_pattern(pattern_str, pattern, mask, &pattern_len);
    if (pattern_len == 0) return NULL;
    
    BYTE* buffer = (BYTE*)malloc(size);
    if (!buffer) return NULL;
    
    SIZE_T bytesRead;
    if (!ReadProcessMemory(process, start, buffer, size, &bytesRead) || bytesRead < size) {
        free(buffer);
        return NULL;
    }
    
    for (SIZE_T i = 0; i <= bytesRead - pattern_len; i++) {
        if (gc_pattern_match(buffer + i, pattern, mask, pattern_len)) {
            free(buffer);
            return start + i;
        }
    }
    
    free(buffer);
    return NULL;
}
BOOL gc_patch_bytes(BYTE* address, const BYTE* bytes, SIZE_T len, GC_Patch* out_patch) {
    DWORD oldProtect;
    if (!gc_unprotect_memory(address, len, &oldProtect)) return FALSE;
    
    if (out_patch) {
        out_patch->address = address;
        out_patch->length = len;
        out_patch->original = (BYTE*)malloc(len);
        if (!out_patch->original) return FALSE;
        memcpy(out_patch->original, address, len);
    }
    
    memcpy(address, bytes, len);
    gc_reprotect_memory(address, len, oldProtect);
    return TRUE;
}

BOOL gc_patch_nop(BYTE* address, SIZE_T len, GC_Patch* out_patch) {
    BYTE* nop_block = (BYTE*)malloc(len);
    if (!nop_block) return FALSE;
    memset(nop_block, 0x90, len); // NOP = 0x90
    BOOL result = gc_patch_bytes(address, nop_block, len, out_patch);
    free(nop_block);
    return result;
}

BOOL gc_restore_bytes(const GC_Patch* patch) {
    if (!patch || !patch->original || !patch->address) return FALSE;
    DWORD oldProtect;
    if (!gc_unprotect_memory(patch->address, patch->length, &oldProtect)) return FALSE;
    
    memcpy(patch->address, patch->original, patch->length);
    gc_reprotect_memory(patch->address, patch->length, oldProtect);
    return TRUE;
}
