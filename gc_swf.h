#ifndef GC_SWF_H
#define GC_SWF_H

#include "gc_defs.h"
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GC_SWF_OK = 0,
    GC_SWF_ERR_IO,
    GC_SWF_ERR_INVALID,
    GC_SWF_ERR_MEMORY
} gc_swf_error_t;

typedef enum {
    GC_SWF_COMP_NONE = 0,
    GC_SWF_COMP_ZLIB,
    GC_SWF_COMP_LZMA
} gc_swf_compression_t;

typedef struct {
    uint8_t version;
    uint32_t file_length;
    gc_swf_compression_t compression;
    uint16_t frame_count;
    float frame_rate;
    uint32_t frame_width;
    uint32_t frame_height;
} gc_swf_header_t;

typedef struct {
    uint16_t tag_code;
    uint32_t tag_length;
    uint8_t *tag_data;
} gc_swf_tag_t;

typedef struct {
    HANDLE *file;
    gc_swf_header_t header;
    uint32_t current_offset;
    int compressed_stream_start;
    void *decompressor; // optional in full version
} gc_swf_context_t;

// Public API
gc_swf_context_t* gc_swf_open(const char *filename, gc_swf_error_t *err);
void gc_swf_close(gc_swf_context_t *ctx);
gc_swf_header_t* gc_swf_get_header(gc_swf_context_t *ctx);
int gc_swf_read_next_tag(gc_swf_context_t *ctx, gc_swf_tag_t *tag);
void gc_swf_free_tag(gc_swf_tag_t *tag);

#ifdef __cplusplus
}
#endif

#endif // GC_SWF_H
