#include "gc_crypto.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#if defined(_WIN32)
#include <windows.h>
#include <wincrypt.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

// ================= SHA-256 =================

#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define CH(x,y,z)     (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z)    (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x)        (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x)        (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x)       (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x)       (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

static const uint32_t k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
    0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
    0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
    0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
    0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[8];
} SHA256_CTX;

static void sha256_transform(SHA256_CTX *ctx, const uint8_t data[]) {
    uint32_t a,b,c,d,e,f,g,h,i,t1,t2,m[64];
    
    for (i=0; i<16; ++i)
        m[i] = (data[i*4] << 24) | (data[i*4+1] << 16) | (data[i*4+2] << 8) | data[i*4+3];
    
    for ( ; i<64; ++i)
        m[i] = SIG1(m[i-2]) + m[i-7] + SIG0(m[i-15]) + m[i-16];
    
    a = ctx->state[0]; b = ctx->state[1]; c = ctx->state[2]; d = ctx->state[3];
    e = ctx->state[4]; f = ctx->state[5]; g = ctx->state[6]; h = ctx->state[7];
    
    for (i=0; i<64; ++i) {
        t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a,b,c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    
    ctx->state[0] += a; ctx->state[1] += b; ctx->state[2] += c; ctx->state[3] += d;
    ctx->state[4] += e; ctx->state[5] += f; ctx->state[6] += g; ctx->state[7] += h;
}

static void sha256_init(SHA256_CTX *ctx) {
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

static void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        ctx->data[ctx->datalen++] = data[i];
        if (ctx->datalen == 64) {
            sha256_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen = 0;
        }
    }
}

static void sha256_final(SHA256_CTX *ctx, uint8_t hash[]) {
    uint32_t i = ctx->datalen;
    
    ctx->data[i++] = 0x80;
    if (i > 56) {
        while (i < 64) ctx->data[i++] = 0x00;
        sha256_transform(ctx, ctx->data);
        i = 0;
    }
    
    while (i < 56) ctx->data[i++] = 0x00;
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = ctx->bitlen;
    ctx->data[62] = ctx->bitlen >> 8;
    ctx->data[61] = ctx->bitlen >> 16;
    ctx->data[60] = ctx->bitlen >> 24;
    ctx->data[59] = ctx->bitlen >> 32;
    ctx->data[58] = ctx->bitlen >> 40;
    ctx->data[57] = ctx->bitlen >> 48;
    ctx->data[56] = ctx->bitlen >> 56;
    sha256_transform(ctx, ctx->data);
    
    for (i = 0; i < 4; ++i)
        for (uint32_t j = 0; j < 8; ++j)
        hash[i + j * 4] = (ctx->state[j] >> (24 - i * 8)) & 0xff;
}

void gc_sha256(const uint8_t *data, size_t len, uint8_t hash[SHA256_HASH_SIZE]) {
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, hash);
}

// ================= HMAC-SHA256 =================

void gc_hmac_sha256(const uint8_t *key, size_t key_len,
                    const uint8_t *data, size_t data_len,
                    uint8_t out[SHA256_HASH_SIZE]) {
    uint8_t k_ipad[64] = {0};
    uint8_t k_opad[64] = {0};
    uint8_t temp_key[SHA256_HASH_SIZE];
    uint8_t temp_hash[SHA256_HASH_SIZE];
    
    if (key_len > 64) {
        gc_sha256(key, key_len, temp_key);
        key = temp_key;
        key_len = SHA256_HASH_SIZE;
    }
    
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);
    
    for (int i = 0; i < 64; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }
    
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, k_ipad, 64);
    sha256_update(&ctx, data, data_len);
    sha256_final(&ctx, temp_hash);
    
    sha256_init(&ctx);
    sha256_update(&ctx, k_opad, 64);
    sha256_update(&ctx, temp_hash, SHA256_HASH_SIZE);
    sha256_final(&ctx, out);
}

#define Nb 4      /* columns in the state (always 4 for AES)  */
#define Nk 4      /* 128-bit key  → 4 words                    */
#define Nr 10     /* number of rounds for AES-128             */


static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};
static const uint8_t rsbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};


static const uint8_t Rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08,
    0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

static void KeyExpansion(const uint8_t* key, uint8_t RoundKey[176]) {
    uint32_t i, j, k;
    uint8_t tempa[4];
    
    for (i = 0; i < Nk; ++i) {
        RoundKey[i * 4 + 0] = key[i * 4 + 0];
        RoundKey[i * 4 + 1] = key[i * 4 + 1];
        RoundKey[i * 4 + 2] = key[i * 4 + 2];
        RoundKey[i * 4 + 3] = key[i * 4 + 3];
    }
    
    for (i = Nk; i < Nb * (Nr + 1); ++i) {
        for (j = 0; j < 4; ++j)
            tempa[j] = RoundKey[(i - 1) * 4 + j];
        
        if (i % Nk == 0) {
            uint8_t t = tempa[0];
            tempa[0] = sbox[tempa[1]] ^ Rcon[i / Nk];
            tempa[1] = sbox[tempa[2]];
            tempa[2] = sbox[tempa[3]];
            tempa[3] = sbox[t];
        }
        
        for (j = 0; j < 4; ++j)
            RoundKey[i * 4 + j] = RoundKey[(i - Nk) * 4 + j] ^ tempa[j];
    }
}

static void AddRoundKey(uint8_t round, uint8_t state[4][4], const uint8_t* RoundKey) {
    for (uint8_t i = 0; i < 4; ++i)
        for (uint8_t j = 0; j < 4; ++j)
        state[j][i] ^= RoundKey[round * Nb * 4 + i * Nb + j];
}

static void SubBytes(uint8_t state[4][4]) {
    for (uint8_t i = 0; i < 4; ++i)
        for (uint8_t j = 0; j < 4; ++j)
        state[i][j] = sbox[state[i][j]];
}

static void InvSubBytes(uint8_t state[4][4]) {
    for (uint8_t i = 0; i < 4; ++i)
        for (uint8_t j = 0; j < 4; ++j)
        state[i][j] = rsbox[state[i][j]];
}

static void ShiftRows(uint8_t state[4][4]) {
    uint8_t temp;
    
    temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;
    
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;
    
    temp = state[3][0];
    state[3][0] = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = temp;
}

static void InvShiftRows(uint8_t state[4][4]) {
    uint8_t temp;
    
    temp = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = state[1][0];
    state[1][0] = temp;
    
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;
    
    temp = state[3][0];
    state[3][0] = state[3][1];
    state[3][1] = state[3][2];
    state[3][2] = state[3][3];
    state[3][3] = temp;
}

static uint8_t xtime(uint8_t x) {
    return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}

static void MixColumns(uint8_t state[4][4]) {
    uint8_t i;
    uint8_t Tmp, Tm, t;
    for (i = 0; i < 4; ++i) {
        t = state[0][i];
        Tmp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i];
        
        Tm = state[0][i] ^ state[1][i]; Tm = xtime(Tm); state[0][i] ^= Tm ^ Tmp;
        Tm = state[1][i] ^ state[2][i]; Tm = xtime(Tm); state[1][i] ^= Tm ^ Tmp;
        Tm = state[2][i] ^ state[3][i]; Tm = xtime(Tm); state[2][i] ^= Tm ^ Tmp;
        Tm = state[3][i] ^ t;           Tm = xtime(Tm); state[3][i] ^= Tm ^ Tmp;
    }
}

static void InvMixColumns(uint8_t state[4][4]) {
    uint8_t i;
    uint8_t a, b, c, d;
    for (i = 0; i < 4; ++i) {
        a = state[0][i];
        b = state[1][i];
        c = state[2][i];
        d = state[3][i];
        
        state[0][i] = xtime(xtime(xtime(a ^ c))) ^ xtime(xtime(b ^ d)) ^ a;
        state[1][i] = xtime(xtime(xtime(b ^ d))) ^ xtime(xtime(c ^ a)) ^ b;
        state[2][i] = xtime(xtime(xtime(c ^ a))) ^ xtime(xtime(d ^ b)) ^ c;
        state[3][i] = xtime(xtime(xtime(d ^ b))) ^ xtime(xtime(a ^ c)) ^ d;
    }
}

static void AES128_ECB_encrypt(const uint8_t* input, const uint8_t* key, uint8_t* output) {
    uint8_t state[4][4];
    uint8_t RoundKey[176];
    
    KeyExpansion(key, RoundKey);
    
    for (uint8_t i = 0; i < 4; ++i)
        for (uint8_t j = 0; j < 4; ++j)
        state[j][i] = input[i * 4 + j];
    
    AddRoundKey(0, state, RoundKey);
    
    for (uint8_t round = 1; round < Nr; ++round) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(round, state, RoundKey);
    }
    
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(Nr, state, RoundKey);
    
    for (uint8_t i = 0; i < 4; ++i)
        for (uint8_t j = 0; j < 4; ++j)
        output[i * 4 + j] = state[j][i];
}

static void AES128_ECB_decrypt(const uint8_t* input, const uint8_t* key, uint8_t* output) {
    uint8_t state[4][4];
    uint8_t RoundKey[176];
    
    KeyExpansion(key, RoundKey);
    
    for (uint8_t i = 0; i < 4; ++i)
        for (uint8_t j = 0; j < 4; ++j)
        state[j][i] = input[i * 4 + j];
    
    AddRoundKey(Nr, state, RoundKey);
    
    for (int8_t round = Nr - 1; round >= 1; --round) {
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(round, state, RoundKey);
        InvMixColumns(state);
    }
    
    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(0, state, RoundKey);
    
    for (uint8_t i = 0; i < 4; ++i)
        for (uint8_t j = 0; j < 4; ++j)
        output[i * 4 + j] = state[j][i];
}

// ================= Random Bytes =================

void gc_crypto_random(uint8_t *buf, size_t len) {
#if defined(_WIN32)
    HCRYPTPROV hProv;
    CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    CryptGenRandom(hProv, len, buf);
    CryptReleaseContext(hProv, 0);
#else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        for (size_t i = 0; i < len; ++i) buf[i] = rand() & 0xFF;
    } else {
        read(fd, buf, len);
        close(fd);
    }
#endif
}
