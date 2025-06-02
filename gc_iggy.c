#include "gc_iggy.h"
#include "gc_string.h"

struct IGGYHeader gc_parse_iggy(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    struct IGGYHeader header = {0};

    if (!file)
    {
        gc_MessageBox("File Opening Error", MB_ICONERROR);
    }

    if (fread(&header, sizeof(header), 1, file) != 1)
    {
        gc_MessageBox("Header Read Error", MB_ICONERROR);
        fclose(file);
    }

    // Verifica la firma
    if (header.signature != IGGY_SIGNATURE)
    {
        fprintf(stderr, "Firma del file non valida\n");
        fclose(file);
    }

    // Stampa l'intestazione del file
    printf("Signature: 0x%" PRIx32 "\n", header.signature);
    if (header.plattform[1] == 64)
    {
        printf("Version: 64-bit\n");
    }
    else if (header.plattform[1] == 32)
    {
        printf("Version: 32-bit\n");
    }
    else
    {
        printf("Unknown Version: %" PRIu32 "\n", header.version);
    }

    // Stampa la piattaforma come esadecimale
    printf("Platform: ");
    for (int i = 0; i < 4; i++)
    {
        printf("%02X ", header.plattform[i]);
    }
    printf("\n");

    printf("Unknown 0x0C: %" PRIu32 "\n", header.unk_0C);
    printf("Number of subfiles: %" PRIu32 "\n", header.num_subfiles);

    // Leggi le voci dei sottotitoli
    size_t subfile_size = sizeof(struct IGGYSubFileEntry);
    struct IGGYSubFileEntry *subfiles = gc_malloc(header.num_subfiles * subfile_size);
    if (!subfiles)
    {
        gc_MessageBox("gc_malloc Error", MB_ICONERROR);
        fclose(file);
    }

    if (fread(subfiles, subfile_size, header.num_subfiles, file) != header.num_subfiles)
    {
        gc_MessageBox("Subfile Read Error", MB_ICONERROR);
        gc_free(subfiles);
        fclose(file);
    }

    // Stampa le voci dei sottotitoli
    for (uint32_t i = 0; i < header.num_subfiles; i++)
    {
        printf("Subfile %u:\n", i);
        printf("  ID: %" PRIu32 "\n", subfiles[i].id);
        printf("  Size: %" PRIu32 "\n", subfiles[i].size);
        printf("  Size2: %" PRIu32 "\n", subfiles[i].size2);
        printf("  Offset: %" PRIu32 "\n", subfiles[i].offset);
    }

    // Gestisci i dati Flash in base alla versione
    if (header.plattform[1] == 64)
    {
        // Logica per 64 bit
        struct IGGYFlashHeader64 flash_header;
        if (fread(&flash_header, sizeof(flash_header), 1, file) != 1)
        {
            gc_MessageBox("Flash Header (64-bit) Read Error", MB_ICONERROR);
            gc_free(subfiles);
            fclose(file);
        }

        // Stampa dell'intestazione Flash (64 bit)
        printf("\n64-bit Flash Header:\n");
        printf("Main Offset: %" PRIu64 "\n", flash_header.main_offset);
        printf("AS3 Section Offset: %" PRIu64 "\n", flash_header.as3_section_offset);
        printf("Unknown Offset: %" PRIu64 "\n", flash_header.unk_offset);
        printf("Unknown Offset2: %" PRIu64 "\n", flash_header.unk_offset2);
        printf("Unknown Offset3: %" PRIu64 "\n", flash_header.unk_offset3);
        printf("Unknown Offset4: %" PRIu64 "\n", flash_header.unk_offset4);
        printf("Unknown 0x30: %" PRIu64 "\n", flash_header.unk_30);
        printf("Width: %" PRIu32 "\n", flash_header.width);
        printf("Height: %" PRIu32 "\n", flash_header.height);
        printf("Unknown 0x40: %" PRIu32 "\n", flash_header.unk_40);
        printf("Unknown 0x44: %" PRIu32 "\n", flash_header.unk_44);
        printf("Unknown 0x48: %" PRIu32 "\n", flash_header.unk_48);
        printf("Unknown 0x4C: %" PRIu32 "\n", flash_header.unk_4C);
        printf("Names Offset: %" PRIu64 "\n", flash_header.names_offset);
        printf("Unknown Offset5: %" PRIu64 "\n", flash_header.unk_offset5);
        printf("Unknown 0x58: %f\n", flash_header.framerate);
        printf("Last Section Offset: %" PRIu64 "\n", flash_header.last_section_offset);
        printf("Unknown Offset6: %" PRIu64 "\n", flash_header.unk_offset6);
        printf("AS3 Code Offset: %" PRIu64 "\n", flash_header.as3_code_offset);
        printf("AS3 Names Offset: %" PRIu64 "\n", flash_header.as3_names_offset);
        printf("Unknown 0xA8: %" PRIu32 "\n", flash_header.unk_A8);
        printf("Unknown 0xAC: %" PRIu32 "\n", flash_header.unk_AC);
        printf("Unknown 0xB0: %" PRIu32 "\n", flash_header.unk_B0);
        printf("Unknown 0xB4: %" PRIu32 "\n", flash_header.unk_B4);
    }
    else if (header.plattform[1] == 32)
    {
        // Logica per 32 bit
        struct IGGYFlashHeader32 flash_header;
        if (fread(&flash_header, sizeof(flash_header), 1, file) != 1)
        {
            gc_MessageBox("Flash Header Read Error", MB_ICONERROR);
            gc_free(subfiles);
            fclose(file);
        }

        // Stampa dell'intestazione Flash (32 bit)
        printf("\n32-bit Flash Header:\n");
        printf("Main Offset: %" PRIu32 "\n", flash_header.main_offset);
        printf("AS3 Section Offset: %" PRIu32 "\n", flash_header.as3_section_offset);
        printf("Unknown Offset: %" PRIu32 "\n", flash_header.unk_offset);
        printf("Unknown Offset2: %" PRIu32 "\n", flash_header.unk_offset2);
        printf("Unknown Offset3: %" PRIu32 "\n", flash_header.unk_offset3);
        printf("Unknown Offset4: %" PRIu32 "\n", flash_header.unk_offset4);
        printf("Unknown 0x30: %" PRIu32 "\n", flash_header.unk_30);
        printf("Width: %" PRIu32 "\n", flash_header.width);
        printf("Height: %u\n", flash_header.height);
        printf("Framerate: %f\n", flash_header.framerate);
        printf("Unknown 0x44: %" PRIu32 "\n", flash_header.unk_44);
        printf("Unknown 0x48: %" PRIu32 "\n", flash_header.unk_48);
        printf("Unknown 0x4C: %" PRIu32 "\n", flash_header.unk_4C);
        printf("Names Offset: %" PRIu32 "\n", flash_header.names_offset);
        printf("Unknown Offset5: %" PRIu32 "\n", flash_header.unk_offset5);
        printf("Unknown 0x58: %" PRIu64 "\n", flash_header.unk_58);
        printf("Last Section Offset: %" PRIu32 "\n", flash_header.last_section_offset);
        printf("Unknown Offset6: %" PRIu32 "\n", flash_header.unk_offset6);
        printf("AS3 Code Offset: %" PRIu32 "\n", flash_header.as3_code_offset);
        printf("AS3 Names Offset: %" PRIu32 "\n", flash_header.as3_names_offset);
        printf("Unknown 0x70: %" PRIu32 "\n", flash_header.unk_70);
        printf("Unknown 0x74: %" PRIu32 "\n", flash_header.unk_74);
        printf("Unknown 0x78: %" PRIu32 "\n", flash_header.unk_78);
        printf("Unknown 0x7C: %" PRIu32 "\n", flash_header.unk_7C);
    }
    else
    {
        printf("ERROR - UNKNOWN VERSION, CAN'T PROCEED");
    }
    // Pulisci
    gc_free(subfiles);
    fclose(file);
    return header;
}
// Function to parse the subfiles from an IGGY file
int gc_parse_iggy_subfiles(const char *file_path, struct IGGYSubFileEntry *subfiles, unsigned int num_subfiles) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        return -1;  // File open error
    }

    // Assume that the subfiles start right after the header.
    // We read the subfiles into the provided subfiles array.
    fseek(file, subfiles->offset, SEEK_SET);  // Skip header

    // Read the subfile entries into the subfiles array
    for (unsigned int i = 0; i < num_subfiles; i++) {
        if (fread(&subfiles[i], sizeof(struct IGGYSubFileEntry), 1, file) != 1) {
            fclose(file);
            return -2;  // Error reading subfiles
        }
    }

    fclose(file);
    return 0;  // Success
}

// Verifica se il file ha estensione ".iggy"
int gc_isIggyFile(const char *filepath) {

    return gc_file_check_extension(filepath, ".iggy"); // Nessuna estensione trovata, non è un file .iggy
}

int gc_parse_flash_data32(const uint8_t *buf, size_t size, struct IGGYFlashHeader32 *hdr_out) {
    if (!buf || size < sizeof(struct IGGYFlashHeader32) || !hdr_out)
        return -1;
    memcpy(hdr_out, buf, sizeof(struct IGGYFlashHeader32));
    if (hdr_out->main_offset < sizeof(struct IGGYFlashHeader32))
        return -2;
    if (hdr_out->as3_section_offset >= size || hdr_out->unk_offset >= size || hdr_out->unk_offset2 >= size || hdr_out->unk_offset3 >= size)
        return -3;
    return 0;
}

int gc_parse_flash_data64(const uint8_t *buf, size_t size, struct IGGYFlashHeader64 *hdr_out) {
    if (!buf || size < sizeof(struct IGGYFlashHeader64) || !hdr_out)
        return -1;
    memcpy(hdr_out, buf, sizeof(struct IGGYFlashHeader64));
    if (hdr_out->main_offset < sizeof(struct IGGYFlashHeader64))
        return -2;
    if (hdr_out->as3_section_offset >= size || hdr_out->unk_offset >= size || hdr_out->unk_offset2 >= size || hdr_out->unk_offset3 >= size)
        return -3;
    return 0;
}

uint8_t *gc_get_abc_blob32(const uint8_t *flash_buf, size_t flash_size, uint32_t *psize) {
    if (!flash_buf || flash_size < sizeof(struct IGGYFlashHeader32) || !psize)
        return NULL;
    const struct IGGYFlashHeader32 *hdr = (const struct IGGYFlashHeader32 *)flash_buf;
    size_t as3_code_offset = hdr->as3_code_offset;
    if (as3_code_offset == 1 || as3_code_offset + 8 > flash_size)
        return NULL;
    const uint8_t *ptr = flash_buf + as3_code_offset + offsetof(struct IGGYFlashHeader32, as3_code_offset);
    uint32_t size = *(const uint32_t *)(ptr - 4); // ptr[4] in C++
    if (size == 0 || (ptr + size) > (flash_buf + flash_size))
        return NULL;
    *psize = size;
    uint8_t *out = (uint8_t *)malloc(size);
    if (!out) return NULL;
    memcpy(out, ptr, size);
    return out;
}

uint8_t *gc_get_abc_blob64(const uint8_t *flash_buf, size_t flash_size, uint32_t *psize) {
    if (!flash_buf || flash_size < sizeof(struct IGGYFlashHeader64) || !psize)
        return NULL;
    const struct IGGYFlashHeader64 *hdr = (const struct IGGYFlashHeader64 *)flash_buf;
    size_t as3_code_offset = (size_t)hdr->as3_code_offset;
    if (as3_code_offset == 1 || as3_code_offset + 0xC > flash_size)
        return NULL;
    const uint8_t *ptr = flash_buf + as3_code_offset + offsetof(struct IGGYFlashHeader64, as3_code_offset);
    uint32_t size = *(const uint32_t *)(ptr - 4); // ptr[8] in C++
    if (size == 0 || (ptr + size) > (flash_buf + flash_size))
        return NULL;
    *psize = size;
    uint8_t *out = (uint8_t *)malloc(size);
    if (!out) return NULL;
    memcpy(out, ptr, size);
    return out;
}

int gc_set_abc_blob32(uint8_t *flash_buf, size_t flash_size, const uint8_t *abc, uint32_t abc_size) {
    if (!flash_buf || !abc || flash_size < sizeof(struct IGGYFlashHeader32))
        return -1;
    struct IGGYFlashHeader32 *hdr = (struct IGGYFlashHeader32 *)flash_buf;
    size_t as3_code_offset = hdr->as3_code_offset;
    if (as3_code_offset == 1 || as3_code_offset + 8 > flash_size)
        return -2;
    uint8_t *ptr = flash_buf + as3_code_offset + offsetof(struct IGGYFlashHeader32, as3_code_offset);
    *(uint32_t *)(ptr - 4) = abc_size;
    memcpy(ptr, abc, abc_size);
    return 0;
}

int gc_set_abc_blob64(uint8_t *flash_buf, size_t flash_size, const uint8_t *abc, uint32_t abc_size) {
    if (!flash_buf || !abc || flash_size < sizeof(struct IGGYFlashHeader64))
        return -1;
    struct IGGYFlashHeader64 *hdr = (struct IGGYFlashHeader64 *)flash_buf;
    size_t as3_code_offset = (size_t)hdr->as3_code_offset;
    if (as3_code_offset == 1 || as3_code_offset + 0xC > flash_size)
        return -2;
    uint8_t *ptr = flash_buf + as3_code_offset + offsetof(struct IGGYFlashHeader64, as3_code_offset);
    *(uint32_t *)(ptr - 4) = abc_size;
    memcpy(ptr, abc, abc_size);
    return 0;
}