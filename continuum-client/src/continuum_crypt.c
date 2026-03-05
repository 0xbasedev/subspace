/*
 * Continuum 0.40 Encryption Client
 * Continuum Block Cipher implementation
 *
 * This is the main encryption used by Continuum 0.40.
 * Based on nullspace ContinuumEncrypt.
 *
 * Cipher: Custom block cipher with 64-bit blocks and 640-bit key
 * Mode: CBC-like chaining
 * Packet format: CRC8 prepended, then encrypted
 */

#include "continuum_crypt.h"
#include "checksum.h"
#include "md5.h"
#include <string.h>

/* Mangler constant for Continuum key expansion */
#define CONTINUUM_STATE_MANGLER 432

/* Rotate left */
static inline u32 rol(u32 var, u32 amount) {
    amount &= 31;
    return (var << amount) | (var >> (32 - amount));
}

/* Rotate right */
static inline u32 ror(u32 var, u32 amount) {
    amount &= 31;
    return (var >> amount) | (var << (32 - amount));
}

void continuum_init(ContinuumEncrypt* ctx) {
    memset(ctx, 0, sizeof(ContinuumEncrypt));
    ctx->state = CONT_STATE_NONE;
}

void continuum_set_expanded_key(ContinuumEncrypt* ctx, const u32* partial_key) {
    memcpy(ctx->expanded_key, partial_key, CONTINUUM_KEY_BYTES);
}

void continuum_finalize_expansion(ContinuumEncrypt* ctx, u32 key1) {
    ctx->key1 = key1;

    /* Step 3: Modified MD5 of expanded key XORed with key1 */
    ctx->expanded_key[0] ^= key1;

    MD5_CTX md5;
    MD5Init(&md5, CONTINUUM_STATE_MANGLER);
    MD5Update(&md5, (u8*)ctx->expanded_key, CONTINUUM_KEY_BYTES);

    /* XOR MD5 state into expanded key with chaining */
    u32 last = key1;
    for (int i = 0; i < CONTINUUM_KEY_WORDS; i++) {
        last = (ctx->expanded_key[i] ^= md5.buf[i & 3] + last);
    }

    ctx->state = CONT_STATE_WAITING;
}

/* Low-level block cipher encrypt */
void continuum_block_encrypt(void* dest, const void* src, u32 len, const u32* key) {
    const u32* plain = (const u32*)src;
    u32* cipher = (u32*)dest;

    u32 blocks = len / 8;
    u32 remaining = len;

    u32 last_upper = 0;
    u32 last_lower = 0;

    /* Process 64-bit blocks */
    for (u32 i = 0; i < blocks; i++) {
        u32 lower = plain[0] ^ last_lower;
        u32 upper = plain[1] ^ last_upper;

        /* 10 rounds of add-rotate */
        for (u32 j = 0; j < 20; ) {
            lower += key[j++];
            lower = rol(lower, upper & 0xFF);
            upper += key[j++];
            upper = rol(upper, lower & 0xFF);
        }

        cipher[0] = lower;
        cipher[1] = upper;

        cipher += 2;
        plain += 2;

        last_lower = lower;
        last_upper = upper;

        remaining -= 8;
    }

    /* Handle remaining 4-byte block */
    if (remaining >= 4) {
        u32 block = plain[0] ^ last_lower;

        for (u32 i = 0; i < 20; ) {
            block += key[i++];
            block = rol(block, last_upper & 0xFF);
            last_upper += key[i++];
        }

        cipher[0] = block;
        cipher++;
        plain++;

        last_lower = block;
        remaining -= 4;
    }

    /* Handle remaining bytes */
    const u8* src_bytes = (const u8*)plain;
    u8* dest_bytes = (u8*)cipher;

    for (u32 i = 0; i < remaining; i++) {
        u8 byte = src_bytes[0] ^ (u8)last_lower;

        for (u32 j = 0; j < 20; ) {
            byte += key[j++] & 0xFF;
            byte += last_upper & 0xFF;
            last_upper += key[j++];
        }

        dest_bytes[0] = byte;
        dest_bytes++;
        src_bytes++;

        last_lower = byte;
    }
}

/* Low-level block cipher decrypt */
void continuum_block_decrypt(void* dest, const void* src, u32 len, const u32* key) {
    u32* plain = (u32*)dest;
    const u32* cipher = (const u32*)src;

    u32 blocks = len / 8;
    u32 remaining = len;

    u32 last_upper = 0;
    u32 last_lower = 0;

    /* Process 64-bit blocks */
    for (u32 i = 0; i < blocks; i++) {
        u32 lower = cipher[0];
        u32 upper = cipher[1];

        /* 10 rounds of subtract-rotate (reversed) */
        for (s32 j = 19; j >= 0; ) {
            upper = ror(upper, lower & 0xFF);
            upper -= key[j--];
            lower = ror(lower, upper & 0xFF);
            lower -= key[j--];
        }

        lower ^= last_lower;
        upper ^= last_upper;

        plain[0] = lower;
        plain[1] = upper;
        plain += 2;

        last_lower = cipher[0];
        last_upper = cipher[1];
        cipher += 2;

        remaining -= 8;
    }

    /* Handle remaining 4-byte block */
    if (remaining >= 4) {
        u32 roramount = last_upper;
        u32 block = cipher[0];

        /* Pre-compute rotation amount */
        for (u32 i = 1; i < 20; i += 2)
            roramount += key[i];

        last_upper = roramount;

        /* Reverse the encryption */
        for (s32 i = 19; i > 0; i -= 2) {
            roramount -= key[i];
            block = ror(block, roramount & 0xFF);
            block -= key[i - 1];
        }

        block ^= last_lower;

        plain[0] = block;
        plain++;

        last_lower = cipher[0];
        cipher++;

        remaining -= 4;
    }

    /* Handle remaining bytes */
    const u8* cipher_bytes = (const u8*)cipher;
    u8* plain_bytes = (u8*)plain;

    for (u32 i = 0; i < remaining; i++) {
        u8 byte = cipher_bytes[0];
        u32 accum = last_upper;

        for (u32 j = 1; j < 20; j += 2)
            accum += key[j];

        last_upper = accum;

        for (s32 j = 19; j > 0; j -= 2) {
            accum -= key[j];
            byte -= ((key[j - 1] & 0xFF) + (accum & 0xFF));
        }

        byte ^= (last_lower & 0xFF);

        plain_bytes[0] = byte;
        plain_bytes++;

        last_lower = cipher_bytes[0];
        cipher_bytes++;
    }
}

size_t continuum_encrypt(ContinuumEncrypt* ctx, const u8* src, u8* dest, size_t size) {
    u8 source[CONTINUUM_MAX_PACKET_SIZE + 1];

    /* Prepend CRC8 checksum */
    u8 crc = crc8(src, size);
    source[0] = crc;
    memcpy(source + 1, src, size);
    size++;

    /* Encrypt */
    continuum_block_encrypt(dest, source, (u32)size, ctx->expanded_key);

    /* CRC escape: if encrypted CRC is 0xFF or looks like connection init */
    if (dest[0] == 0xFF ||
        (dest[0] == 0x00 && (dest[1] == 0x01 || dest[1] == 0x10 || dest[1] == 0x11))) {
        memmove(dest + 1, dest, size);
        dest[0] = 0xFF;
        size++;
    }

    return size;
}

size_t continuum_decrypt(ContinuumEncrypt* ctx, u8* data, size_t size) {
    u8* src = data;

    /* Handle CRC escape */
    if ((u8)src[0] == 0xFF && ((u8)src[1] == 0x00 || (u8)src[1] == 0xFF)) {
        src++;
        size--;
    }

    u8 decrypted[CONTINUUM_MAX_PACKET_SIZE];
    continuum_block_decrypt(decrypted, src, (u32)size, ctx->expanded_key);

    size--;

    memcpy(data, decrypted + 1, size);

    /* Verify CRC */
    u8 crc_check = decrypted[0];
    u8 crc = crc8(decrypted + 1, size);

    if (crc != crc_check) {
        /* Try repeated-byte CRC (for certain packet types) */
        u8 repeat_crc = crc8_repeat(data[0], size);
        if (repeat_crc != crc_check) {
            return 0; /* CRC mismatch */
        }
    }

    return size;
}

int continuum_is_initialized(const ContinuumEncrypt* ctx) {
    return ctx->state == CONT_STATE_INITIALIZED;
}

int continuum_is_expanding(const ContinuumEncrypt* ctx) {
    return ctx->state == CONT_STATE_EXPANDING;
}
