#ifndef GC_BINPATCH_H
#define GC_BINPATCH_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    // Scans a file for a pattern like "DE AD ?? BE EF"
    LONGLONG gc_bin_aobscan_file(const char* path, const char* pattern);
    
    // Writes a byte patch at the given file offset
    BOOL gc_bin_patch_file(const char* path, LONGLONG offset, const BYTE* bytes, SIZE_T len);
    
    // Performs a scan and patches the found result
    BOOL gc_bin_patch_aob(const char* path, const char* pattern, const BYTE* bytes, SIZE_T len);
    
    // Creates a .bak file
    BOOL gc_bin_backup(const char* path);
    
#ifdef __cplusplus
}
#endif

#endif // GC_BINPATCH_H