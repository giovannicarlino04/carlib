#include "gc_swf.h"
#include "gc_common.h"
#include <stdlib.h>
#include <string.h>
#include <zlib.h>


gc_swf_context_t* gc_swf_open(const char *filename, gc_swf_error_t *err) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        if (err) *err = GC_SWF_ERR_IO;
        return NULL;
    }

    gc_swf_context_t *ctx = (gc_swf_context_t *)calloc(1, sizeof(gc_swf_context_t));
    if (!ctx) {
        fclose(f);
        if (err) *err = GC_SWF_ERR_MEMORY;
        return NULL;
    }

    ctx->file = f;

    char signature[3];
    fread(signature, 1, 3, f);
    ctx->header.version = fgetc(f);
    ctx->header.file_length = gc_read_u32_le(f);

    if (memcmp(signature, "FWS", 3) == 0) {
        ctx->header.compression = GC_SWF_COMP_NONE;
    } else if (memcmp(signature, "CWS", 3) == 0) {
        ctx->header.compression = GC_SWF_COMP_ZLIB;
    } else if (memcmp(signature, "ZWS", 3) == 0) {
        ctx->header.compression = GC_SWF_COMP_LZMA; // not implemented
    } else {
        free(ctx);
        fclose(f);
        if (err) *err = GC_SWF_ERR_INVALID;
        return NULL;
    }

    // Skip RECT (frame size), for simplicity just parse dummy
    uint8_t rect_nbits = fgetc(f) >> 3;
    int rect_bytes = ((5 * rect_nbits) + 7) / 8;
    fseek(f, rect_bytes - 1, SEEK_CUR);

    ctx->header.frame_rate = fgetc(f) + (fgetc(f) << 8);
    ctx->header.frame_count = gc_read_u16_le(f);

    if (err) *err = GC_SWF_OK;
    return ctx;
}

void gc_swf_close(gc_swf_context_t *ctx) {
    if (!ctx) return;
    if (ctx->file) fclose(ctx->file);
    free(ctx);
}

gc_swf_header_t* gc_swf_get_header(gc_swf_context_t *ctx) {
    if (!ctx) return NULL;
    return &ctx->header;
}

int gc_swf_read_next_tag(gc_swf_context_t *ctx, gc_swf_tag_t *tag) {
    if (!ctx || !ctx->file || !tag) return 0;

    uint16_t tag_code_and_length = gc_read_u16_le(ctx->file);
    tag->tag_code = tag_code_and_length >> 6;
    uint32_t length = tag_code_and_length & 0x3F;

    if (length == 0x3F) {
        length = gc_read_u32_le(ctx->file); // long tag
    }

    tag->tag_length = length;
    tag->tag_data = (uint8_t *)malloc(length);
    if (!tag->tag_data) return 0;

    fread(tag->tag_data, 1, length, ctx->file);
    return 1;
}

void gc_swf_free_tag(gc_swf_tag_t *tag) {
    if (tag && tag->tag_data) {
        free(tag->tag_data);
        tag->tag_data = NULL;
    }
}
