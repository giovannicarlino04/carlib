#ifndef IGGY_H
#define IGGY_H

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "gc_memory.h"

#define uint32 uint32_t
#define uint64 uint64_t
#define uchar unsigned char

// Define signature for IggyFile
#define IGGY_SIGNATURE  0xED0A6749

// IggyHeader structure
struct IGGYHeader
{
    uint32 signature;            // 0x00
    uint32 version;              // 0x04
    uchar plattform[4];          // 0x08
    uint32 unk_0C;               // 0x0C
    uchar reserved[0xC];         // 0x10
    uint32 num_subfiles;         // 0x1C
} ;

// IGGYSubFileEntry structure
struct IGGYSubFileEntry
{
    uint32 id;                   // 0x00
    uint32 size;                 // 0x04
    uint32 size2;                // 0x08
    uint32 offset;               // 0x0C
} ;

// IGGYFlashHeader32 structure
struct IGGYFlashHeader32
{
    uint32 main_offset;          // 0x00
    uint32 as3_section_offset;   // 0x04
    uint32 unk_offset;           // 0x08
    uint32 unk_offset2;          // 0x0C
    uint32 unk_offset3;          // 0x10
    uint32 unk_offset4;          // 0x14
    uint64 unk_18;               // 0x18
    uint32 width;                // 0x20
    uint32 height;               // 0x24
    uint32 unk_28;               // 0x28
    uint32 unk_2C;               // 0x2C
    uint32 unk_30;               // 0x30
    uint32 unk_34;               // 0x34
    uint32 unk_38;               // 0x38
    uint32 unk_3C;               // 0x3C
    float framerate;             // 0x40
    uint32 unk_44;               // 0x44
    uint32 unk_48;               // 0x48
    uint32 unk_4C;               // 0x4C
    uint32 names_offset;         // 0x50
    uint32 unk_offset5;          // 0x54
    uint64 unk_58;               // 0x58
    uint32 last_section_offset;  // 0x60
    uint32 unk_offset6;          // 0x64
    uint32 as3_code_offset;      // 0x68
    uint32 as3_names_offset;     // 0x6C
    uint32 unk_70;               // 0x70
    uint32 unk_74;               // 0x74
    uint32 unk_78;               // 0x78
    uint32 unk_7C;               // 0x7C
} ; 

// IGGYFlashHeader64 structure
struct IGGYFlashHeader64
{
    uint64 main_offset;          // 0x00
    uint64 as3_section_offset;   // 0x08
    uint64 unk_offset;           // 0x10
    uint64 unk_offset2;          // 0x18
    uint64 unk_offset3;          // 0x20
    uint64 unk_offset4;          // 0x28
    uint64 unk_30;               // 0x30
    uint32 width;                // 0x38
    uint32 height;               // 0x3C
    uint32 unk_40;               // 0x40
    uint32 unk_44;               // 0x44
    uint32 unk_48;               // 0x48
    uint32 unk_4C;               // 0x4C
    uint32 unk_50;               // 0x50
    uint32 unk_54;               // 0x54
    float framerate;             // 0x58
    uint32 unk_5C;               // 0x5C
    uint64 unk_60;               // 0x60
    uint64 unk_68;               // 0x68
    uint64 names_offset;         // 0x70
    uint64 unk_offset5;          // 0x78
    uint64 unk_80;               // 0x80
    uint64 last_section_offset;  // 0x88
    uint64 unk_offset6;          // 0x90
    uint64 as3_code_offset;      // 0x98
    uint64 as3_names_offset;     // 0xA0
    uint32 unk_A8;               // 0xA8
    uint32 unk_AC;               // 0xAC
    uint32 unk_B0;               // 0xB0
    uint32 unk_B4;               // 0xB4
} ;
int gc_parse_iggy_subfiles(const char *file_path, struct IGGYSubFileEntry *subfiles, unsigned int num_subfiles);
struct IGGYHeader gc_parse_iggy(const char *filename);
int ends_with(const char *str, const char *suffix);

#endif //IGGY_H