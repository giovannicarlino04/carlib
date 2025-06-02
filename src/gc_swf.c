#include "common.h"

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

DLLEXPORT gc_swf_context_t* gc_swf_open(const char *filename, gc_swf_error_t *err) {
    HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        if (err) *err = GC_SWF_ERR_IO;
        return NULL;
    }

    gc_swf_context_t *ctx = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(gc_swf_context_t));
    if (!ctx) {
        CloseHandle(file);
        if (err) *err = GC_SWF_ERR_MEMORY;
        return NULL;
    }

    ctx->file = file;

    // Read signature
    char signature[3];
    DWORD bytes_read;
    if (!ReadFile(file, signature, 3, &bytes_read, NULL) || bytes_read != 3) {
        HeapFree(GetProcessHeap(), 0, ctx);
        CloseHandle(file);
        if (err) *err = GC_SWF_ERR_IO;
        return NULL;
    }

    // Read version
    uint8_t version;
    if (!ReadFile(file, &version, 1, &bytes_read, NULL) || bytes_read != 1) {
        HeapFree(GetProcessHeap(), 0, ctx);
        CloseHandle(file);
        if (err) *err = GC_SWF_ERR_IO;
        return NULL;
    }
    ctx->header.version = version;

    // Read file length
    ctx->header.file_length = gc_read_u32_le(file);

    // Check signature
    if (memcmp(signature, "FWS", 3) == 0) {
        ctx->header.compression = GC_SWF_COMP_NONE;
    } else if (memcmp(signature, "CWS", 3) == 0) {
        ctx->header.compression = GC_SWF_COMP_ZLIB;
    } else if (memcmp(signature, "ZWS", 3) == 0) {
        ctx->header.compression = GC_SWF_COMP_LZMA; // not implemented
    } else {
        HeapFree(GetProcessHeap(), 0, ctx);
        CloseHandle(file);
        if (err) *err = GC_SWF_ERR_INVALID;
        return NULL;
    }

    // Read RECT header (frame size) — skip ahead
    uint8_t nbits_byte;
    if (!ReadFile(file, &nbits_byte, 1, &bytes_read, NULL) || bytes_read != 1) {
        HeapFree(GetProcessHeap(), 0, ctx);
        CloseHandle(file);
        if (err) *err = GC_SWF_ERR_IO;
        return NULL;
    }

    uint8_t rect_nbits = nbits_byte >> 3;
    int rect_bytes = ((5 * rect_nbits) + 7) / 8;

    // Move file pointer forward
    SetFilePointer(file, rect_bytes - 1, NULL, FILE_CURRENT);

    // Read frame rate
    uint8_t byte1, byte2;
    ReadFile(file, &byte1, 1, &bytes_read, NULL);
    ReadFile(file, &byte2, 1, &bytes_read, NULL);
    ctx->header.frame_rate = byte1 + (byte2 << 8);

    // Read frame count
    ctx->header.frame_count = gc_read_u16_le(file);

    if (err) *err = GC_SWF_OK;
    return ctx;
}

DLLEXPORT void gc_swf_close(gc_swf_context_t *ctx) {
    if (!ctx) return;
    if (ctx->file && ctx->file != INVALID_HANDLE_VALUE) {
        CloseHandle(ctx->file);
    }
    HeapFree(GetProcessHeap(), 0, ctx);
}

DLLEXPORT gc_swf_header_t* gc_swf_get_header(gc_swf_context_t *ctx) {
    if (!ctx) return NULL;
    return &ctx->header;
}

DLLEXPORT int gc_swf_read_next_tag(gc_swf_context_t *ctx, gc_swf_tag_t *tag) {
    if (!ctx || ctx->file == INVALID_HANDLE_VALUE || !tag) return 0;

    uint16_t tag_code_and_length;
    DWORD bytesRead;

    // Read 2 bytes for tag code and length
    if (!ReadFile(ctx->file, &tag_code_and_length, sizeof(tag_code_and_length), &bytesRead, NULL) || bytesRead != sizeof(tag_code_and_length)) {
        return 0;
    }
    // Assuming little-endian reading, else convert with something like ntohs()
    tag_code_and_length = (uint16_t)(tag_code_and_length); // if necessary, convert from LE to host order

    tag->tag_code = tag_code_and_length >> 6;
    uint32_t length = tag_code_and_length & 0x3F;

    if (length == 0x3F) {
        uint32_t long_length;
        if (!ReadFile(ctx->file, &long_length, sizeof(long_length), &bytesRead, NULL) || bytesRead != sizeof(long_length)) {
            return 0;
        }
        length = long_length;
    }

    tag->tag_length = length;
    tag->tag_data = (uint8_t *)gc_malloc(length);
    if (!tag->tag_data) return 0;

    if (!ReadFile(ctx->file, tag->tag_data, length, &bytesRead, NULL) || bytesRead != length) {
        gc_free(tag->tag_data);
        tag->tag_data = NULL;
        return 0;
    }

    return 1;
}

DLLEXPORT void gc_swf_free_tag(gc_swf_tag_t *tag) {
    if (tag && tag->tag_data) {
        free(tag->tag_data);
        tag->tag_data = NULL;
    }
}
