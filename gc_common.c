#include "gc_common.h"

uint32_t gc_read_u32_le(HANDLE hFile) {
    uint8_t buf[4];
    DWORD bytesRead;
    if (!ReadFile(hFile, buf, 4, &bytesRead, NULL) || bytesRead != 4)
        return 0;
    return (uint32_t)buf[0] |
           ((uint32_t)buf[1] << 8) |
           ((uint32_t)buf[2] << 16) |
           ((uint32_t)buf[3] << 24);
}

uint16_t gc_read_u16_le(HANDLE hFile) {
    uint8_t buf[2];
    DWORD bytesRead;
    if (!ReadFile(hFile, buf, 2, &bytesRead, NULL) || bytesRead != 2)
        return 0;
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}