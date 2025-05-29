#include "gc_wav.h"
#include "gc_common.h" // for read like functions
#include "gc_memory.h" // for gc_malloc
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


gc_wav_t* gc_wav_open(const char *filename, gc_wav_error_t *err) {
    if (err) *err = GC_WAV_OK;
    
    FILE *f = fopen(filename, "rb");
    if (!f) {
        if (err) *err = GC_WAV_ERR_IO;
        return NULL;
    }
    
    // Read RIFF header
    char riff[4];
    if (fread(riff, 1, 4, f) != 4 || strncmp(riff, "RIFF", 4) != 0) {
        fclose(f);
        if (err) *err = GC_WAV_ERR_INVALID_FORMAT;
        return NULL;
    }
    
    // Skip chunk size
    if (fseek(f, 4, SEEK_CUR) != 0) {
        fclose(f);
        if (err) *err = GC_WAV_ERR_IO;
        return NULL;
    }
    
    char wave[4];
    if (fread(wave, 1, 4, f) != 4 || strncmp(wave, "WAVE", 4) != 0) {
        fclose(f);
        if (err) *err = GC_WAV_ERR_INVALID_FORMAT;
        return NULL;
    }
    
    gc_wav_t *wav = (gc_wav_t*)calloc(1, sizeof(gc_wav_t));
    if (!wav) {
        fclose(f);
        if (err) *err = GC_WAV_ERR_MEMORY;
        return NULL;
    }
    
    // Read chunks until fmt and data found
    int found_fmt = 0;
    int found_data = 0;
    while (!found_fmt || !found_data) {
        char chunk_id[4];
        if (fread(chunk_id, 1, 4, f) != 4) break;
        
        uint32_t chunk_size = gc_read_u32_le(f);
        if (chunk_size == 0) break;
        
        if (strncmp(chunk_id, "fmt ", 4) == 0) {
            if (chunk_size < 16) {
                free(wav);
                fclose(f);
                if (err) *err = GC_WAV_ERR_INVALID_FORMAT;
                return NULL;
            }
            
            wav->audio_format = gc_read_u16_le(f);
            wav->num_channels = gc_read_u16_le(f);
            wav->sample_rate = gc_read_u32_le(f);
            wav->byte_rate = gc_read_u32_le(f);
            wav->block_align = gc_read_u16_le(f);
            wav->bits_per_sample = gc_read_u16_le(f);
            
            // Skip any extra fmt bytes
            if (chunk_size > 16) {
                if (fseek(f, chunk_size - 16, SEEK_CUR) != 0) {
                    free(wav);
                    fclose(f);
                    if (err) *err = GC_WAV_ERR_IO;
                    return NULL;
                }
            }
            
            // Only PCM supported (audio_format == 1)
            if (wav->audio_format != 1) {
                free(wav);
                fclose(f);
                if (err) *err = GC_WAV_ERR_UNSUPPORTED_FORMAT;
                return NULL;
            }
            
            found_fmt = 1;
        } else if (strncmp(chunk_id, "data", 4) == 0) {
            wav->data_size = chunk_size;
            wav->data = (uint8_t*)gc_malloc(chunk_size);
            if (!wav->data) {
                free(wav);
                fclose(f);
                if (err) *err = GC_WAV_ERR_MEMORY;
                return NULL;
            }
            if (fread(wav->data, 1, chunk_size, f) != chunk_size) {
                free(wav->data);
                free(wav);
                fclose(f);
                if (err) *err = GC_WAV_ERR_IO;
                return NULL;
            }
            found_data = 1;
        } else {
            // Skip unknown chunk
            if (fseek(f, chunk_size, SEEK_CUR) != 0) break;
        }
    }
    
    fclose(f);
    
    if (!found_fmt || !found_data) {
        if (wav->data) free(wav->data);
        free(wav);
        if (err) *err = GC_WAV_ERR_INVALID_FORMAT;
        return NULL;
    }
    
    if (err) *err = GC_WAV_OK;
    return wav;
}

void gc_wav_close(gc_wav_t *wav) {
    if (!wav) return;
    if (wav->data) free(wav->data);
    free(wav);
}
