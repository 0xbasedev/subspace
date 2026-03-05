/*
 * Continuum 0.40 Encryption Client
 * VIE (SubSpace) Encryption implementation
 *
 * Original SubSpace used this encryption. Continuum can fall back to it.
 * Based on ASSS enc_vie.c and nullspace VieEncrypt.
 */

#include "vie_crypt.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* VIE PRNG for keystream generation
 * This is a Linear Congruential Generator variant */
static void vie_generate_keystream(u8* keystream, u32 key) {
    s16* table = (s16*)keystream;
    s32 t;
    s64 k = key;

    for (int i = 0; i < VIE_KEYSTREAM_SIZE / 2; i++) {
        /* LCG step: key = (key * 16807) mod (2^31 - 1) with adjustment */
        t = (s32)(((s64)k * 0x834E0B5F) >> 48);
        t += t >> 31;
        k = ((k % 127773) * 16807) - (t * 2836) + 123;
        if (k == 0 || (k & 0x80000000))
            k += 0x7FFFFFFF;
        table[i] = (s16)k;
    }
}

void vie_init(VieEncrypt* ctx) {
    memset(ctx, 0, sizeof(VieEncrypt));
}

u32 vie_generate_key(void) {
    u32 tick = (u32)time(NULL);
    u32 edx = tick * 0xCCCCCCCD;

    srand(tick);

    u32 res = ((rand() % 65535) << 16) + (edx >> 3) + (rand() % 65535);
    res = (res ^ edx) - edx;

    /* Ensure key is in valid range (negative in signed representation) */
    if (res <= 0x7FFFFFFF)
        res = ~res + 1;

    return res;
}

int vie_is_valid_key(VieEncrypt* ctx, u32 server_key) {
    return (server_key == ctx->session_key) ||
           (server_key == ctx->client_key) ||
           (server_key == ((~ctx->client_key) + 1));
}

int vie_initialize(VieEncrypt* ctx, u32 server_key) {
    if (!vie_is_valid_key(ctx, server_key))
        return 0;

    if (ctx->client_key == server_key) {
        /* Encryption disabled */
        ctx->session_key = 0;
        memset(ctx->keystream, 0, VIE_KEYSTREAM_SIZE);
    } else {
        ctx->session_key = server_key;
        vie_generate_keystream(ctx->keystream, ctx->session_key);
    }

    return 1;
}

size_t vie_encrypt(VieEncrypt* ctx, const u8* src, u8* dest, size_t size) {
    if (!ctx->session_key) {
        memcpy(dest, src, size);
        return size;
    }

    u32 ksi = 0;
    size_t i = 1;
    u32 IV = ctx->session_key;

    dest[0] = src[0];

    /* Skip second byte if first byte is 0x00 (core packet marker) */
    if (src[0] == 0x00) {
        if (size <= 2) {
            memcpy(dest, src, size);
            return size;
        }
        dest[1] = src[1];
        i++;
    }

    /* Process 4-byte blocks with CBC-like XOR chaining */
    while (i + 4 <= size) {
        u32 plain = *(u32*)(src + i);
        u32 key_block = *(u32*)(ctx->keystream + ksi);
        IV = plain ^ key_block ^ IV;
        *(u32*)(dest + i) = IV;

        i += 4;
        ksi += 4;
    }

    /* Handle remaining bytes */
    size_t remaining = size - i;
    if (remaining > 0) {
        u32 block = 0;
        memcpy(&block, src + i, remaining);
        block ^= *(u32*)(ctx->keystream + ksi) ^ IV;
        memcpy(dest + i, &block, remaining);
    }

    return size;
}

size_t vie_decrypt(VieEncrypt* ctx, u8* data, size_t size) {
    if (!ctx->session_key)
        return size;

    u32 ksi = 0;
    size_t i = 1;
    u32 IV = ctx->session_key;

    /* Skip second byte if first byte is 0x00 */
    if (data[0] == 0x00) {
        if (size <= 2)
            return size;
        i++;
    }

    /* Process 4-byte blocks */
    while (i + 4 <= size) {
        u32 cipher = *(u32*)(data + i);
        u32 key_block = *(u32*)(ctx->keystream + ksi);
        *(u32*)(data + i) = key_block ^ IV ^ cipher;
        IV = cipher;

        i += 4;
        ksi += 4;
    }

    /* Handle remaining bytes */
    size_t remaining = size - i;
    if (remaining > 0) {
        u32 block = 0;
        memcpy(&block, data + i, remaining);
        block ^= *(u32*)(ctx->keystream + ksi) ^ IV;
        memcpy(data + i, &block, remaining);
    }

    return size;
}
