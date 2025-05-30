#ifndef GC_CRYPTO_H
#define GC_CRYPTO_H

#include "gc_common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#define SHA256_HASH_SIZE 32
#define AES_BLOCK_SIZE 16
#define AES128_KEY_SIZE 16
#define HMAC_SHA256_KEY_SIZE 64
    
    void gc_sha256(const uint8_t *data, size_t len, uint8_t hash[SHA256_HASH_SIZE]);
    
    void gc_hmac_sha256(const uint8_t *key, size_t key_len,
                        const uint8_t *data, size_t data_len,
                        uint8_t out[SHA256_HASH_SIZE]);
    
    void gc_aes128_encrypt_block(const uint8_t key[AES128_KEY_SIZE],
                                 const uint8_t input[AES_BLOCK_SIZE],
                                 uint8_t output[AES_BLOCK_SIZE]);
    
    void gc_aes128_decrypt_block(const uint8_t key[AES128_KEY_SIZE],
                                 const uint8_t input[AES_BLOCK_SIZE],
                                 uint8_t output[AES_BLOCK_SIZE]);
    
    void gc_crypto_random(uint8_t *buf, size_t len);
    
#ifdef __cplusplus
}
#endif

#endif // GC_CRYPTO_H