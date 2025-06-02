#include "common.h"

typedef enum {
    GC_WAV_OK = 0,
    GC_WAV_ERR_IO,
    GC_WAV_ERR_INVALID_FORMAT,
    GC_WAV_ERR_UNSUPPORTED_FORMAT,
    GC_WAV_ERR_MEMORY,
} gc_wav_error_t;

typedef struct {
    uint16_t audio_format;      // PCM = 1 (linear quantization)
    uint16_t num_channels;      // Mono = 1, Stereo = 2
    uint32_t sample_rate;       // 44100, 48000, etc.
    uint32_t byte_rate;         // sample_rate * num_channels * bits_per_sample/8
    uint16_t block_align;       // num_channels * bits_per_sample/8
    uint16_t bits_per_sample;   // 8, 16, 24, 32
    uint32_t data_size;         // Size of audio data in bytes
    uint8_t *data;              // Pointer to raw PCM data (malloc'd)
} gc_wav_t;

DLLEXPORT gc_wav_t* gc_wav_open(const char *filename, gc_wav_error_t *err);

DLLEXPORT void gc_wav_close(gc_wav_t *wav);

gc_wav_t* gc_wav_open(const char *filename, gc_wav_error_t *err) {
    if (err) *err = GC_WAV_OK;

    HANDLE hFile = CreateFileA(
        filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        if (err) *err = GC_WAV_ERR_IO;
        return NULL;
    }

    DWORD bytesRead;
    char riff[4];
    if (!ReadFile(hFile, riff, 4, &bytesRead, NULL) || bytesRead != 4 || memcmp(riff, "RIFF", 4) != 0) {
        CloseHandle(hFile);
        if (err) *err = GC_WAV_ERR_INVALID_FORMAT;
        return NULL;
    }

    // Skip chunk size
    SetFilePointer(hFile, 4, NULL, FILE_CURRENT);

    char wave[4];
    if (!ReadFile(hFile, wave, 4, &bytesRead, NULL) || bytesRead != 4 || memcmp(wave, "WAVE", 4) != 0) {
        CloseHandle(hFile);
        if (err) *err = GC_WAV_ERR_INVALID_FORMAT;
        return NULL;
    }

    gc_wav_t *wav = (gc_wav_t*)gc_malloc(sizeof(gc_wav_t));
    if (!wav) {
        CloseHandle(hFile);
        if (err) *err = GC_WAV_ERR_MEMORY;
        return NULL;
    }
    memset(wav, 0, sizeof(gc_wav_t));

    int found_fmt = 0;
    int found_data = 0;

    while (!found_fmt || !found_data) {
        char chunk_id[4];
        if (!ReadFile(hFile, chunk_id, 4, &bytesRead, NULL) || bytesRead != 4) break;

        uint32_t chunk_size = gc_read_u32_le(hFile);
        if (chunk_size == 0) break;

        if (memcmp(chunk_id, "fmt ", 4) == 0) {
            if (chunk_size < 16) {
                gc_free(wav);
                CloseHandle(hFile);
                if (err) *err = GC_WAV_ERR_INVALID_FORMAT;
                return NULL;
            }

            wav->audio_format = gc_read_u16_le(hFile);
            wav->num_channels = gc_read_u16_le(hFile);
            wav->sample_rate = gc_read_u32_le(hFile);
            wav->byte_rate = gc_read_u32_le(hFile);
            wav->block_align = gc_read_u16_le(hFile);
            wav->bits_per_sample = gc_read_u16_le(hFile);

            if (chunk_size > 16) {
                SetFilePointer(hFile, chunk_size - 16, NULL, FILE_CURRENT);
            }

            if (wav->audio_format != 1) {
                gc_free(wav);
                CloseHandle(hFile);
                if (err) *err = GC_WAV_ERR_UNSUPPORTED_FORMAT;
                return NULL;
            }

            found_fmt = 1;
        } else if (memcmp(chunk_id, "data", 4) == 0) {
            wav->data_size = chunk_size;
            wav->data = (uint8_t*)gc_malloc(chunk_size);
            if (!wav->data) {
                gc_free(wav);
                CloseHandle(hFile);
                if (err) *err = GC_WAV_ERR_MEMORY;
                return NULL;
            }

            if (!ReadFile(hFile, wav->data, chunk_size, &bytesRead, NULL) || bytesRead != chunk_size) {
                gc_free(wav->data);
                gc_free(wav);
                CloseHandle(hFile);
                if (err) *err = GC_WAV_ERR_IO;
                return NULL;
            }

            found_data = 1;
        } else {
            SetFilePointer(hFile, chunk_size, NULL, FILE_CURRENT);
        }
    }

    CloseHandle(hFile);

    if (!found_fmt || !found_data) {
        if (wav->data) gc_free(wav->data);
        gc_free(wav);
        if (err) *err = GC_WAV_ERR_INVALID_FORMAT;
        return NULL;
    }

    return wav;
}

void gc_wav_close(gc_wav_t *wav) {
    if (!wav) return;
    if (wav->data) gc_free(wav->data);
    gc_free(wav);
}
