#ifndef GC_COMMON_H
#define GC_COMMON_H


#include "gc_defs.h"
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// Read little-endian unsigned 32-bit integer from Windows HANDLE, returns 0 on failure
uint32_t gc_read_u32_le(HANDLE hFile);

// Read little-endian unsigned 16-bit integer from Windows HANDLE, returns 0 on failure
uint16_t gc_read_u16_le(HANDLE hFile);

#ifdef __cplusplus
}
#endif

#endif // GC_COMMON_H
