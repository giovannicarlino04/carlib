#ifndef GC_XENOVERSE_CMS_H
#define GC_XENOVERSE_CMS_H

#include "gc_defs.h"
#include <Windows.h>

#define SHORT_NAME_LENGTH 4
#define MAX_PATHS 7

typedef struct {
    int32_t ID;
    char ShortName[SHORT_NAME_LENGTH];
    uint8_t Unknown[8];
    char* Paths[MAX_PATHS];
} CMS_Data;

typedef struct {
    char* FileName;
    int32_t Count;
    CMS_Data* Data;
} CMS;

// Function prototypes
static char* gc_CMSReadString(HANDLE hFile, int32_t address);
void gc_CMSLoad(CMS* cms, const char* path);
void gc_CMSSave(const CMS* cms);
void gc_FreeCMS(CMS* cms);

#endif // GC_XENOVERSE_CMS_H
