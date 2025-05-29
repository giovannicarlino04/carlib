#ifndef GC_PATCH_H
#define GC_PATCH_H

#include <windows.h>

typedef struct {
    BYTE* address;
    BYTE* original;
    SIZE_T length;
} GC_Patch;

/**
 * Scans memory in the current process for a given pattern.
 * Use '?' or '??' in the pattern to specify wildcards.
 * Example: "48 8B ?? ?? ?? 89"
 */
BYTE* gc_aobscan(const char* pattern, BYTE* start, SIZE_T size);

/**
 * Overwrites memory at a specific address.
 */
BOOL gc_patch_bytes(BYTE* address, const BYTE* bytes, SIZE_T len, GC_Patch* out_patch);

/**
 * NOPs a range of memory.
 */
BOOL gc_patch_nop(BYTE* address, SIZE_T len, GC_Patch* out_patch);

/**
 * Restores memory from a previously saved GC_Patch.
 */
BOOL gc_restore_bytes(const GC_Patch* patch);

#endif // GC_PATCH_H
