#ifndef GC_MP4_H
#define GC_MP4_H
#include "gc_defs.h"

typedef struct gc_mp4 gc_mp4;

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

// Initialize and open the MP4 file
gc_mp4* gc_open(const char* path);

// Decode the next frame into BGRA format
int gc_decode_next_frame(gc_mp4* mp4, gc_frame_t* frame);

// Free a decoded frame
void gc_free_frame(gc_frame_t* frame);

// Close and free resources
void gc_close(gc_mp4* mp4);

#ifdef __cplusplus
}
#endif

#endif // GC_MP4_H
