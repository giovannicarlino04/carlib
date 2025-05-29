#ifndef GC_COMMON_H
#define GC_COMMON_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    // Read little-endian unsigned 32-bit integer from file, returns 0 on failure
    uint32_t gc_read_u32_le(FILE *f);
    
    // Read little-endian unsigned 16-bit integer from file, returns 0 on failure
    uint16_t gc_read_u16_le(FILE *f);
    
#ifdef __cplusplus
}
#endif

#endif // GC_COMMON_H
