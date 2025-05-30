#ifndef GC_WAV_H
#define GC_WAV_H

#include "gc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

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

// Load WAV file from disk, returns pointer on success, NULL on failure
// On failure, *err is set to error code if err != NULL
gc_wav_t* gc_wav_open(const char *filename, gc_wav_error_t *err);

// Free memory allocated by gc_wav_open
void gc_wav_close(gc_wav_t *wav);

#ifdef __cplusplus
}
#endif

#endif // GC_WAV_H
