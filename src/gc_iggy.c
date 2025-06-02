#include "common.h"

// Define signature for IggyFile
#define IGGY_SIGNATURE  0xED0A6749

// IggyHeader structure
struct IGGYHeader
{
    uint32_t signature;            // 0x00
    uint32_t version;              // 0x04
    unsigned char plattform[4];          // 0x08
    uint32_t unk_0C;               // 0x0C
    unsigned char reserved[0xC];         // 0x10
    uint32_t num_subfiles;         // 0x1C
} ;

// IGGYSubFileEntry structure
struct IGGYSubFileEntry
{
    uint32_t id;                   // 0x00
    uint32_t size;                 // 0x04
    uint32_t size2;                // 0x08
    uint32_t offset;               // 0x0C
} ;

// IGGYFlashHeader32 structure
struct IGGYFlashHeader32
{
    uint32_t main_offset;          // 0x00
    uint32_t as3_section_offset;   // 0x04
    uint32_t unk_offset;           // 0x08
    uint32_t unk_offset2;          // 0x0C
    uint32_t unk_offset3;          // 0x10
    uint32_t unk_offset4;          // 0x14
    uint64_t unk_18;               // 0x18
    uint32_t width;                // 0x20
    uint32_t height;               // 0x24
    uint32_t unk_28;               // 0x28
    uint32_t unk_2C;               // 0x2C
    uint32_t unk_30;               // 0x30
    uint32_t unk_34;               // 0x34
    uint32_t unk_38;               // 0x38
    uint32_t unk_3C;               // 0x3C
    float framerate;             // 0x40
    uint32_t unk_44;               // 0x44
    uint32_t unk_48;               // 0x48
    uint32_t unk_4C;               // 0x4C
    uint32_t names_offset;         // 0x50
    uint32_t unk_offset5;          // 0x54
    uint64_t unk_58;               // 0x58
    uint32_t last_section_offset;  // 0x60
    uint32_t unk_offset6;          // 0x64
    uint32_t as3_code_offset;      // 0x68
    uint32_t as3_names_offset;     // 0x6C
    uint32_t unk_70;               // 0x70
    uint32_t unk_74;               // 0x74
    uint32_t unk_78;               // 0x78
    uint32_t unk_7C;               // 0x7C
} ; 

// IGGYFlashHeader64 structure
struct IGGYFlashHeader64
{
    uint64_t main_offset;          // 0x00
    uint64_t as3_section_offset;   // 0x08
    uint64_t unk_offset;           // 0x10
    uint64_t unk_offset2;          // 0x18
    uint64_t unk_offset3;          // 0x20
    uint64_t unk_offset4;          // 0x28
    uint64_t unk_30;               // 0x30
    uint32_t width;                // 0x38
    uint32_t height;               // 0x3C
    uint32_t unk_40;               // 0x40
    uint32_t unk_44;               // 0x44
    uint32_t unk_48;               // 0x48
    uint32_t unk_4C;               // 0x4C
    uint32_t unk_50;               // 0x50
    uint32_t unk_54;               // 0x54
    float framerate;             // 0x58
    uint32_t unk_5C;               // 0x5C
    uint64_t unk_60;               // 0x60
    uint64_t unk_68;               // 0x68
    uint64_t names_offset;         // 0x70
    uint64_t unk_offset5;          // 0x78
    uint64_t unk_80;               // 0x80
    uint64_t last_section_offset;  // 0x88
    uint64_t unk_offset6;          // 0x90
    uint64_t as3_code_offset;      // 0x98
    uint64_t as3_names_offset;     // 0xA0
    uint32_t unk_A8;               // 0xA8
    uint32_t unk_AC;               // 0xAC
    uint32_t unk_B0;               // 0xB0
    uint32_t unk_B4;               // 0xB4
} ;

// Verifica se il file ha estensione ".iggy"
DLLEXPORT int gc_isIggyFile(const char *filepath) {

    return gc_file_check_extension(filepath, ".iggy"); // Nessuna estensione trovata, non è un file .iggy
}

DLLEXPORT int gc_parse_flash_data32(const uint8_t *buf, size_t size, struct IGGYFlashHeader32 *hdr_out) {
    if (!buf || size < sizeof(struct IGGYFlashHeader32) || !hdr_out)
        return -1;
    memcpy(hdr_out, buf, sizeof(struct IGGYFlashHeader32));
    if (hdr_out->main_offset < sizeof(struct IGGYFlashHeader32))
        return -2;
    if (hdr_out->as3_section_offset >= size || hdr_out->unk_offset >= size || hdr_out->unk_offset2 >= size || hdr_out->unk_offset3 >= size)
        return -3;
    return 0;
}

DLLEXPORT int gc_parse_flash_data64(const uint8_t *buf, size_t size, struct IGGYFlashHeader64 *hdr_out) {
    if (!buf || size < sizeof(struct IGGYFlashHeader64) || !hdr_out)
        return -1;
    memcpy(hdr_out, buf, sizeof(struct IGGYFlashHeader64));
    if (hdr_out->main_offset < sizeof(struct IGGYFlashHeader64))
        return -2;
    if (hdr_out->as3_section_offset >= size || hdr_out->unk_offset >= size || hdr_out->unk_offset2 >= size || hdr_out->unk_offset3 >= size)
        return -3;
    return 0;
}

DLLEXPORT uint8_t *gc_get_abc_blob32(const uint8_t *flash_buf, size_t flash_size, uint32_t *psize) {
    if (!flash_buf || flash_size < sizeof(struct IGGYFlashHeader32) || !psize)
        return NULL;
    const struct IGGYFlashHeader32 *hdr = (const struct IGGYFlashHeader32 *)flash_buf;
    size_t as3_code_offset = hdr->as3_code_offset;
    if (as3_code_offset == 1 || as3_code_offset + 8 > flash_size)
        return NULL;
    const uint8_t *ptr = flash_buf + as3_code_offset;
    uint32_t size = *(const uint32_t *)(ptr - 4); // ptr[4] in C++
    if (size == 0 || (ptr + size) > (flash_buf + flash_size))
        return NULL;
    *psize = size;
    uint8_t *out = (uint8_t *)malloc(size);
    if (!out) return NULL;
    memcpy(out, ptr, size);
    return out;
}

DLLEXPORT uint8_t *gc_get_abc_blob64(const uint8_t *flash_buf, size_t flash_size, uint32_t *psize) {
    if (!flash_buf || flash_size < sizeof(struct IGGYFlashHeader64) || !psize)
        return NULL;
    const struct IGGYFlashHeader64 *hdr = (const struct IGGYFlashHeader64 *)flash_buf;
    size_t as3_code_offset = (size_t)hdr->as3_code_offset;
    if (as3_code_offset == 1 || as3_code_offset + 0xC > flash_size)
        return NULL;
    const uint8_t *ptr = flash_buf + as3_code_offset + 0xC;
    uint32_t size = *(const uint32_t *)(flash_buf + as3_code_offset + 8); // ptr[8] in C++
    if (size == 0 || (ptr + size) > (flash_buf + flash_size))
        return NULL;
    *psize = size;
    uint8_t *out = (uint8_t *)malloc(size);
    if (!out) return NULL;
    memcpy(out, ptr, size);
    return out;
}

DLLEXPORT int gc_set_abc_blob32(uint8_t *flash_buf, size_t flash_size, const uint8_t *abc, uint32_t abc_size) {
    if (!flash_buf || !abc || flash_size < sizeof(struct IGGYFlashHeader32))
        return -1;
    struct IGGYFlashHeader32 *hdr = (struct IGGYFlashHeader32 *)flash_buf;
    size_t as3_code_offset = hdr->as3_code_offset;
    if (as3_code_offset == 1 || as3_code_offset + 8 > flash_size)
        return -2;
    uint8_t *ptr = flash_buf + as3_code_offset;
    *(uint32_t *)(ptr - 4) = abc_size;
    memcpy(ptr, abc, abc_size);
    return 0;
}

DLLEXPORT int gc_set_abc_blob64(uint8_t *flash_buf, size_t flash_size, const uint8_t *abc, uint32_t abc_size) {
    struct IGGYFlashHeader64 *hdr = (struct IGGYFlashHeader64 *)flash_buf;
    size_t as3_code_offset = (size_t)hdr->as3_code_offset;
    if (as3_code_offset == 1 || as3_code_offset + 0xC > flash_size)
        return -2;
    uint8_t *ptr = flash_buf + as3_code_offset + 0xC;
    *(uint32_t *)(flash_buf + as3_code_offset + 8) = abc_size;
    memcpy(ptr, abc, abc_size);
    return 0;
    memcpy(ptr, abc, abc_size);
    return 0;
}

// --- Begin C implementations of IggyFile logic ---
// Write an uncompressed SWF header to a file, using IGGY flash header for width/height/framerate/framecount
internal void write_uncompressed_swf_header_from_iggy(FILE *f, uint32_t file_length, uint32_t width, uint32_t height, float framerate, uint16_t frame_count) {
    // Signature: "FWS"
    fputc('F', f);
    fputc('W', f);
    fputc('S', f);

    // Version: 8 (can be changed)
    fputc(8, f);

    // FileLength: 4 bytes, little-endian
    fwrite(&file_length, 4, 1, f);

    // FrameSize (RECT): width/height in twips (1 px = 20 twips)
    // RECT encoding: Nbits=15 (enough for most cases), 5 bits for Nbits, then 4x Nbits for Xmin, Xmax, Ymin, Ymax
    // We'll use: Nbits=15, Xmin=0, Xmax=width*20, Ymin=0, Ymax=height*20
    uint32_t xmax = width * 20;
    uint32_t ymax = height * 20;
    uint8_t rect[9] = {0};
    // Nbits = 15
    rect[0] = 0x78; // 01111 000 (Nbits=15)
    // Pack Xmin (0), Xmax, Ymin (0), Ymax into bits
    // Xmin: 0 (15 bits)
    // Xmax: 15 bits
    rect[1] = (xmax >> 10) & 0xFF;
    rect[2] = (xmax >> 2) & 0xFF;
    rect[3] = ((xmax & 0x3) << 6);
    // Ymin: 0 (15 bits)
    // Ymax: 15 bits
    rect[4] = (ymax >> 11) & 0xFF;
    rect[5] = (ymax >> 3) & 0xFF;
    rect[6] = ((ymax & 0x7) << 5);
    // The rest is padding (zeros)
    fwrite(rect, 9, 1, f);

    // FrameRate: UI16 (8.8 fixed), e.g. 24.0 = 0x1800
    uint16_t frame_rate_fixed8 = (uint16_t)(framerate * 256.0f);
    fwrite(&frame_rate_fixed8, 2, 1, f);

    // FrameCount: UI16
    fwrite(&frame_count, 2, 1, f);
}
// Helper: align value up to alignment (must be power of 2)
internal size_t align2(size_t value, size_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

// Helper: get pointer at offset
internal uint8_t *get_offset_ptr(uint8_t *base, uint32_t offset) {
    return base + offset;
}

// Load IGGY flash data (32-bit)
DLLEXPORT int gc_load_flash_data32(const uint8_t *buf, size_t size,
    uint8_t **main_section, size_t *main_size,
    uint8_t **as3_names_section, size_t *as3_names_size,
    uint8_t **as3_code_section, size_t *as3_code_size,
    uint8_t **names_section, size_t *names_size,
    uint8_t **last_section, size_t *last_size,
    uint32_t *as3_offset)
{
    if (!buf || size < sizeof(struct IGGYFlashHeader32)) return -1;
    const struct IGGYFlashHeader32 *hdr = (const struct IGGYFlashHeader32 *)buf;

    if (hdr->main_offset < sizeof(struct IGGYFlashHeader32)) return -2;
    if (hdr->as3_section_offset >= size || hdr->unk_offset >= size || hdr->unk_offset2 >= size || hdr->unk_offset3 >= size)
        return -3;

    size_t prev_offset = 0;
    size_t next_offset = hdr->as3_section_offset;

    // main_section
    *main_size = next_offset - prev_offset;
    *main_section = malloc(*main_size);
    memcpy(*main_section, buf + prev_offset, *main_size);

    prev_offset = next_offset;

    // as3_names_section
    if (hdr->as3_code_offset != 1)
        next_offset = hdr->as3_code_offset + offsetof(struct IGGYFlashHeader32, as3_code_offset);
    else if (hdr->names_offset != 1)
        next_offset = hdr->names_offset + offsetof(struct IGGYFlashHeader32, names_offset);
    else if (hdr->last_section_offset != 1)
        next_offset = hdr->last_section_offset + offsetof(struct IGGYFlashHeader32, last_section_offset);
    else
        next_offset = size;

    *as3_names_size = next_offset - prev_offset;
    *as3_names_section = malloc(*as3_names_size);
    memcpy(*as3_names_section, buf + prev_offset, *as3_names_size);

    prev_offset = next_offset;

    // as3_code_section
    if (hdr->as3_code_offset != 1) {
        if (hdr->names_offset != 1)
            next_offset = hdr->names_offset + offsetof(struct IGGYFlashHeader32, names_offset);
        else if (hdr->last_section_offset != 1)
            next_offset = hdr->last_section_offset + offsetof(struct IGGYFlashHeader32, last_section_offset);
        else
            next_offset = size;

        *as3_offset = (uint32_t)prev_offset;
        *as3_code_size = next_offset - prev_offset;
        *as3_code_section = malloc(*as3_code_size);
        memcpy(*as3_code_section, buf + prev_offset, *as3_code_size);

        prev_offset = next_offset;
    } else {
        *as3_code_section = NULL;
        *as3_code_size = 0;
    }

    // names_section
    if (hdr->names_offset != 1) {
        if (hdr->last_section_offset != 1)
            next_offset = hdr->last_section_offset + offsetof(struct IGGYFlashHeader32, last_section_offset);
        else
            next_offset = size;

        *names_size = next_offset - prev_offset;
        *names_section = malloc(*names_size);
        memcpy(*names_section, buf + prev_offset, *names_size);

        prev_offset = next_offset;
    } else {
        *names_section = NULL;
        *names_size = 0;
    }

    // last_section
    if (hdr->last_section_offset != 1) {
        next_offset = size;
        *last_size = next_offset - prev_offset;
        *last_section = malloc(*last_size);
        memcpy(*last_section, buf + prev_offset, *last_size);
    } else {
        *last_section = NULL;
        *last_size = 0;
    }

    // Check as3 code section
    if (*as3_code_size != 0) {
        uint8_t *as3_ptr = *as3_code_section;
        size_t max_as3_size = *as3_code_size;
        if (max_as3_size < 8 || *(uint32_t *)&as3_ptr[4] > max_as3_size-8)
            return -4;
    }

    return 0;
}

// Load IGGY flash data (64-bit)
DLLEXPORT int gc_load_flash_data64(const uint8_t *buf, size_t size,
    uint8_t **main_section, size_t *main_size,
    uint8_t **as3_names_section, size_t *as3_names_size,
    uint8_t **as3_code_section, size_t *as3_code_size,
    uint8_t **names_section, size_t *names_size,
    uint8_t **last_section, size_t *last_size,
    uint32_t *as3_offset)
{
    if (!buf || size < sizeof(struct IGGYFlashHeader64)) return -1;
    const struct IGGYFlashHeader64 *hdr = (const struct IGGYFlashHeader64 *)buf;

    if (hdr->main_offset < sizeof(struct IGGYFlashHeader64)) return -2;
    if (hdr->as3_section_offset >= size || hdr->unk_offset >= size || hdr->unk_offset2 >= size || hdr->unk_offset3 >= size)
        return -3;

    size_t prev_offset = 0;
    size_t next_offset = hdr->as3_section_offset;

    // main_section
    *main_size = next_offset - prev_offset;
    *main_section = malloc(*main_size);
    memcpy(*main_section, buf + prev_offset, *main_size);

    prev_offset = next_offset;

    // as3_names_section
    if (hdr->as3_code_offset != 1)
        next_offset = hdr->as3_code_offset + offsetof(struct IGGYFlashHeader64, as3_code_offset);
    else if (hdr->names_offset != 1)
        next_offset = hdr->names_offset + offsetof(struct IGGYFlashHeader64, names_offset);
    else if (hdr->last_section_offset != 1)
        next_offset = hdr->last_section_offset + offsetof(struct IGGYFlashHeader64, last_section_offset);
    else
        next_offset = size;

    *as3_names_size = next_offset - prev_offset;
    *as3_names_section = malloc(*as3_names_size);
    memcpy(*as3_names_section, buf + prev_offset, *as3_names_size);

    prev_offset = next_offset;

    // as3_code_section
    if (hdr->as3_code_offset != 1) {
        if (hdr->names_offset != 1)
            next_offset = hdr->names_offset + offsetof(struct IGGYFlashHeader64, names_offset);
        else if (hdr->last_section_offset != 1)
            next_offset = hdr->last_section_offset + offsetof(struct IGGYFlashHeader64, last_section_offset);
        else
            next_offset = size;

        *as3_offset = (uint32_t)prev_offset;
        *as3_code_size = next_offset - prev_offset;
        *as3_code_section = malloc(*as3_code_size);
        memcpy(*as3_code_section, buf + prev_offset, *as3_code_size);

        prev_offset = next_offset;
    } else {
        *as3_code_section = NULL;
        *as3_code_size = 0;
    }

    // names_section
    if (hdr->names_offset != 1) {
        if (hdr->last_section_offset != 1)
            next_offset = hdr->last_section_offset + offsetof(struct IGGYFlashHeader64, last_section_offset);
        else
            next_offset = size;

        *names_size = next_offset - prev_offset;
        *names_section = malloc(*names_size);
        memcpy(*names_section, buf + prev_offset, *names_size);

        prev_offset = next_offset;
    } else {
        *names_section = NULL;
        *names_size = 0;
    }

    // last_section
    if (hdr->last_section_offset != 1) {
        next_offset = size;
        *last_size = next_offset - prev_offset;
        *last_section = malloc(*last_size);
        memcpy(*last_section, buf + prev_offset, *last_size);
    } else {
        *last_section = NULL;
        *last_size = 0;
    }

    // Check as3 code section
    if (*as3_code_size != 0) {
        uint8_t *as3_ptr = *as3_code_section;
        size_t max_as3_size = *as3_code_size;
        if (max_as3_size < 0x10 || *(uint32_t *)&as3_ptr[8] > max_as3_size-0xC)
            return -4;
    }

    return 0;
}

// Save flash data (32-bit)
DLLEXPORT void gc_save_flash_data32(uint8_t *buf, size_t size,
    const uint8_t *main_section, size_t main_size,
    const uint8_t *as3_names_section, size_t as3_names_size,
    const uint8_t *as3_code_section, size_t as3_code_size,
    const uint8_t *names_section, size_t names_size,
    const uint8_t *last_section, size_t last_size)
{
    struct IGGYFlashHeader32 *hdr = (struct IGGYFlashHeader32 *)buf;
    memcpy(buf, main_section, main_size);
    memcpy(buf+main_size, as3_names_section, as3_names_size);
    memcpy(buf+main_size+as3_names_size, as3_code_section, as3_code_size);
    memcpy(buf+main_size+as3_names_size+as3_code_size, names_section, names_size);
    memcpy(buf+main_size+as3_names_size+as3_code_size+names_size, last_section, last_size);

    hdr->as3_section_offset = (uint32_t)main_size;
    if (hdr->names_offset != 1)
        hdr->names_offset = (uint32_t)(main_size+as3_names_size+as3_code_size-offsetof(struct IGGYFlashHeader32, names_offset));
    if (hdr->last_section_offset != 1)
        hdr->last_section_offset = (uint32_t)(size-last_size-offsetof(struct IGGYFlashHeader32, last_section_offset));
}

// Save flash data (64-bit)
DLLEXPORT void gc_save_flash_data64(uint8_t *buf, size_t size,
    const uint8_t *main_section, size_t main_size,
    const uint8_t *as3_names_section, size_t as3_names_size,
    const uint8_t *as3_code_section, size_t as3_code_size,
    const uint8_t *names_section, size_t names_size,
    const uint8_t *last_section, size_t last_size)
{
    struct IGGYFlashHeader64 *hdr = (struct IGGYFlashHeader64 *)buf;
    memcpy(buf, main_section, main_size);
    memcpy(buf+main_size, as3_names_section, as3_names_size);
    memcpy(buf+main_size+as3_names_size, as3_code_section, as3_code_size);
    memcpy(buf+main_size+as3_names_size+as3_code_size, names_section, names_size);
    memcpy(buf+main_size+as3_names_size+as3_code_size+names_size, last_section, last_size);

    hdr->as3_section_offset = (uint32_t)main_size;
    if (hdr->names_offset != 1)
        hdr->names_offset = (uint32_t)(main_size+as3_names_size+as3_code_size-offsetof(struct IGGYFlashHeader64, names_offset));
    if (hdr->last_section_offset != 1)
        hdr->last_section_offset = (uint32_t)(size-last_size-offsetof(struct IGGYFlashHeader64, last_section_offset));
}

DLLEXPORT size_t gc_get_flash_data_size(size_t main_size, size_t as3_names_size, size_t as3_code_size, size_t names_size, size_t last_size) {
    return main_size + as3_names_size + as3_code_size + names_size + last_size;
}

// Extract the ABC blob from an as3_code_section buffer (used for both 32/64-bit)
DLLEXPORT uint8_t *gc_get_abc_blob_from_section(const uint8_t *as3_code_section, size_t as3_code_size, int is_64, uint32_t *psize) {
    if (!as3_code_section || as3_code_size == 0 || !psize) return NULL;
    uint32_t size = is_64 ? *(uint32_t *)&as3_code_section[8] : *(uint32_t *)&as3_code_section[4];
    if (size == 0 || (is_64 ? (size + 0xC > as3_code_size) : (size + 8 > as3_code_size))) return NULL;
    *psize = size;
    uint8_t *buf = (uint8_t *)malloc(size);
    if (!buf) return NULL;
    memcpy(buf, as3_code_section + (is_64 ? 0xC : 8), size);
    return buf;
}

// Set the ABC blob in an as3_code_section buffer (used for both 32/64-bit)
DLLEXPORT int gc_set_abc_blob_in_section(uint8_t **as3_code_section, size_t *as3_code_size, int is_64, const void *abc, uint32_t size) {
    if (!as3_code_section || !*as3_code_section || !abc || *as3_code_size == 0) return 0;
    uint32_t new_size = (uint32_t)((size + (is_64 ? 0xC : 8) + 0xF) & ~0xF); // align to 0x10
    uint8_t *new_section = (uint8_t *)realloc(*as3_code_section, new_size);
    if (!new_section) return 0;
    if (is_64) {
        memset(new_section + 0xC, 0, new_size - 0xC);
        memcpy(new_section + 0xC, abc, size);
        *(uint32_t *)&new_section[8] = size;
    } else {
        memset(new_section + 8, 0, new_size - 8);
        memcpy(new_section + 8, abc, size);
        *(uint32_t *)&new_section[4] = size;
    }
    *as3_code_section = new_section;
    *as3_code_size = new_size;
    return 1;
}