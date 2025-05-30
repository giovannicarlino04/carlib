#ifndef GC_AVI_H
#define GC_AVI_H
#include "gc_defs.h"

typedef struct gc_avi gc_avi;

#ifdef __cplusplus
extern "C" {
#endif

// --- Types ---
typedef struct {
    uint8_t* data;
    int width;
    int height;
    int stride;
} gc_frame_t;

// Initialize and open the avi file
gc_avi* gc_open(const char* path);

// Decode the next frame into BGRA format
int gc_decode_next_frame(gc_avi* avi, gc_frame_t* frame);

// Free a decoded frame
void gc_free_frame(gc_frame_t* frame);

// Close and free resources
void gc_close(gc_avi* avi);

#ifdef __cplusplus
}
#endif

#endif // GC_AVI_H
