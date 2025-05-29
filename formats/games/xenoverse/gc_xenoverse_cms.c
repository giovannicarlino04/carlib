#include <stdio.h>
#include <stdlib.h>

#include "gc_xenoverse_cms.h"
#include "../../../gc_memory.h"
#include "../../../gc_string.h"

// Function to load a CMS file.
void gc_CMSLoad(CMS* cms, const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    
    cms->FileName = strdup(path);
    
    fseek(file, 8, SEEK_SET);
    fread(&cms->Count, sizeof(int32_t), 1, file);
    
    int32_t offset;
    fread(&offset, sizeof(int32_t), 1, file);
    
    cms->Data = gc_malloc(cms->Count * sizeof(CMS_Data));
    
    for (int i = 0; i < cms->Count; ++i) {
        fseek(file, offset + (80 * i), SEEK_SET);
        
        fread(&cms->Data[i].ID, sizeof(int32_t), 1, file);
        
        fread(cms->Data[i].ShortName, 1, SHORT_NAME_LENGTH - 1, file);
        cms->Data[i].ShortName[SHORT_NAME_LENGTH - 1] = '\0';
        
        fseek(file, 9, SEEK_CUR);
        fread(cms->Data[i].Unknown, sizeof(uint8_t), 8, file);
        fseek(file, 8, SEEK_CUR);
        
        for (int j = 0; j < MAX_PATHS; ++j) {
            int32_t address;
            fread(&address, sizeof(int32_t), 1, file);
            cms->Data[i].Paths[j] = gc_CMSReadString(file, address);
        }
    }
    
    fclose(file);
}

// Function to save a CMS file.
void gc_CMSSave(const CMS* cms) {
    FILE* file = fopen(cms->FileName, "wb");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    
    fwrite("#CMS\xFE\xFF\x00\x00", 1, 8, file);
    fwrite(&cms->Count, sizeof(int32_t), 1, file);
    
    int32_t offset = 16;
    fwrite(&offset, sizeof(int32_t), 1, file);
    
    offset += cms->Count * 80;
    for (int i = 0; i < cms->Count; ++i) {
        CMS_Data* data = &cms->Data[i];
        
        for (int j = 0; j < MAX_PATHS; ++j) {
            if (data->Paths[j]) {
                fseek(file, offset, SEEK_SET);
                offset += strlen(data->Paths[j]) + 1;
                fwrite(data->Paths[j], 1, strlen(data->Paths[j]), file);
                fputc('\0', file);
            }
        }
    }
    
    for (int i = 0; i < cms->Count; ++i) {
        CMS_Data* data = &cms->Data[i];
        
        fseek(file, 16 + (80 * i), SEEK_SET);
        fwrite(&data->ID, sizeof(int32_t), 1, file);
        fwrite(data->ShortName, 1, SHORT_NAME_LENGTH - 1, file);
        
        fseek(file, 9, SEEK_CUR);
        fwrite(data->Unknown, sizeof(uint8_t), 8, file);
        fseek(file, 8, SEEK_CUR);
        
        for (int j = 0; j < MAX_PATHS; ++j) {
            if (data->Paths[j]) {
                int32_t address = offset;
                fwrite(&address, sizeof(int32_t), 1, file);
            }
        }
    }
    
    fclose(file);
}

// Function to free CMS resources.
void gc_FreeCMS(CMS* cms) {
    for (int i = 0; i < cms->Count; ++i) {
        for (int j = 0; j < MAX_PATHS; ++j) {
            free(cms->Data[i].Paths[j]);
        }
    }
    free(cms->Data);
    free(cms->FileName);
}
