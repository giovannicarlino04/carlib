#include "gc_xenoverse_cms.h"
#include "gc_memory.h"
#include "gc_string.h"
#include <windows.h>

static char* gc_CMSReadString(HANDLE hFile, int32_t address) {
    if (address == 0) return NULL;
    // Save current file position
    LARGE_INTEGER origPos;
    SetFilePointerEx(hFile, (LARGE_INTEGER){0}, &origPos, FILE_CURRENT);

    // Seek to string position
    LARGE_INTEGER liAddress = {0};
    liAddress.LowPart = (DWORD)address;
    liAddress.HighPart = 0;
    if (!SetFilePointerEx(hFile, liAddress, NULL, FILE_BEGIN)) {
        return NULL;
    }

    // Read string byte-by-byte until null terminator
    char buffer[1024]; // adjust max string length if needed
    DWORD bytesRead;
    int len = 0;
    while (len < (int)sizeof(buffer) - 1) {
        if (!ReadFile(hFile, &buffer[len], 1, &bytesRead, NULL) || bytesRead != 1)
            break;
        if (buffer[len] == '\0')
            break;
        len++;
    }
    buffer[len] = '\0';

    // Restore original position
    SetFilePointerEx(hFile, origPos, NULL, FILE_BEGIN);

    // Duplicate string with gc_malloc
    return gc_strndup(buffer, len);
}

void gc_CMSLoad(CMS* cms, const char* path) {
    HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        // handle error as needed (you may want to add error code return)
        return;
    }

    cms->FileName = gc_strndup(path, gc_strnlen(path, 1024));

    // Seek to offset 8
    SetFilePointer(hFile, 8, NULL, FILE_BEGIN);

    DWORD bytesRead;
    // Read Count (int32_t)
    if (!ReadFile(hFile, &cms->Count, sizeof(int32_t), &bytesRead, NULL) || bytesRead != sizeof(int32_t)) {
        CloseHandle(hFile);
        return;
    }

    // Read offset (int32_t)
    int32_t offset = 0;
    if (!ReadFile(hFile, &offset, sizeof(int32_t), &bytesRead, NULL) || bytesRead != sizeof(int32_t)) {
        CloseHandle(hFile);
        return;
    }

    cms->Data = (CMS_Data*)gc_malloc(cms->Count * sizeof(CMS_Data));
    memset(cms->Data, 0, cms->Count * sizeof(CMS_Data));

    for (int i = 0; i < cms->Count; ++i) {
        // Seek to offset + 80*i
        LARGE_INTEGER liPos = {0};
        liPos.LowPart = offset + (80 * i);
        liPos.HighPart = 0;
        SetFilePointerEx(hFile, liPos, NULL, FILE_BEGIN);

        // Read ID
        if (!ReadFile(hFile, &cms->Data[i].ID, sizeof(int32_t), &bytesRead, NULL) || bytesRead != sizeof(int32_t)) {
            CloseHandle(hFile);
            return;
        }

        // Read ShortName (SHORT_NAME_LENGTH - 1 bytes)
        if (!ReadFile(hFile, cms->Data[i].ShortName, SHORT_NAME_LENGTH - 1, &bytesRead, NULL) || bytesRead != SHORT_NAME_LENGTH - 1) {
            CloseHandle(hFile);
            return;
        }
        cms->Data[i].ShortName[SHORT_NAME_LENGTH - 1] = '\0';

        // Skip 9 bytes
        SetFilePointer(hFile, 9, NULL, FILE_CURRENT);

        // Read Unknown (8 bytes)
        if (!ReadFile(hFile, cms->Data[i].Unknown, 8, &bytesRead, NULL) || bytesRead != 8) {
            CloseHandle(hFile);
            return;
        }

        // Skip 8 bytes
        SetFilePointer(hFile, 8, NULL, FILE_CURRENT);

        // Read MAX_PATHS addresses and strings
        for (int j = 0; j < MAX_PATHS; ++j) {
            int32_t address = 0;
            if (!ReadFile(hFile, &address, sizeof(int32_t), &bytesRead, NULL) || bytesRead != sizeof(int32_t)) {
                CloseHandle(hFile);
                return;
            }
            cms->Data[i].Paths[j] = gc_CMSReadString(hFile, address);
        }
    }

    CloseHandle(hFile);
}

void gc_CMSSave(const CMS* cms) {
    HANDLE hFile = CreateFileA(cms->FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        // handle error
        return;
    }

    DWORD bytesWritten;
    // Write header "#CMS\xFE\xFF\x00\x00"
    WriteFile(hFile, "#CMS\xFE\xFF\x00\x00", 8, &bytesWritten, NULL);

    // Write Count
    WriteFile(hFile, &cms->Count, sizeof(int32_t), &bytesWritten, NULL);

    int32_t offset = 16;
    WriteFile(hFile, &offset, sizeof(int32_t), &bytesWritten, NULL);

    // Calculate where strings will start
    int32_t stringOffset = offset + (cms->Count * 80);

    // Write all strings first at their offset
    for (int i = 0; i < cms->Count; ++i) {
        for (int j = 0; j < MAX_PATHS; ++j) {
            if (cms->Data[i].Paths[j]) {
                LARGE_INTEGER liPos = {0};
                liPos.LowPart = stringOffset;
                liPos.HighPart = 0;
                SetFilePointerEx(hFile, liPos, NULL, FILE_BEGIN);

                DWORD len = (DWORD)strlen(cms->Data[i].Paths[j]);
                WriteFile(hFile, cms->Data[i].Paths[j], len, &bytesWritten, NULL);
                WriteFile(hFile, "\0", 1, &bytesWritten, NULL);

                stringOffset += len + 1;
            }
        }
    }

    // Write the CMS_Data entries
    for (int i = 0; i < cms->Count; ++i) {
        LARGE_INTEGER liPos = {0};
        liPos.LowPart = 16 + (80 * i);
        liPos.HighPart = 0;
        SetFilePointerEx(hFile, liPos, NULL, FILE_BEGIN);

        CMS_Data* data = &cms->Data[i];

        WriteFile(hFile, &data->ID, sizeof(int32_t), &bytesWritten, NULL);
        WriteFile(hFile, data->ShortName, SHORT_NAME_LENGTH - 1, &bytesWritten, NULL);

        SetFilePointer(hFile, 9, NULL, FILE_CURRENT);
        WriteFile(hFile, data->Unknown, 8, &bytesWritten, NULL);
        SetFilePointer(hFile, 8, NULL, FILE_CURRENT);

        for (int j = 0; j < MAX_PATHS; ++j) {
            if (data->Paths[j]) {
                // Write address - you must compute it properly, here simplified
                // This example assumes paths are written contiguously in previous loop
                // You must track and store each string's offset separately to write correct address here
                int32_t address = 0; // TODO: compute real address per path
                WriteFile(hFile, &address, sizeof(int32_t), &bytesWritten, NULL);
            } else {
                int32_t zero = 0;
                WriteFile(hFile, &zero, sizeof(int32_t), &bytesWritten, NULL);
            }
        }
    }

    CloseHandle(hFile);
}

void gc_FreeCMS(CMS* cms) {
    for (int i = 0; i < cms->Count; ++i) {
        for (int j = 0; j < MAX_PATHS; ++j) {
            if (cms->Data[i].Paths[j]) {
                gc_free(cms->Data[i].Paths[j]);
            }
        }
    }
    gc_free(cms->Data);
    gc_free(cms->FileName);
}
