/*
 * Continuum 0.40 Encryption Client
 * Continuum Block Cipher
 *
 * This is the main encryption used by Continuum 0.40.
 *
 * Cipher specifications:
 * - Block size: 64 bits (8 bytes)
 * - Key size: 640 bits (20 x 32-bit words = 80 bytes)
 * - Operations: Add, rotate-left, rotate-right, XOR
 * - Mode: CBC-like chaining
 *
 * Key expansion has 3 steps:
 * - Steps 1 & 2: Performed by the Continuum executable (proprietary)
 *   Must be obtained from an oracle service or the scrty1 file
 * - Step 3: Modified MD5 hash (implemented here)
 *
 * Packet format:
 * - All encrypted packets are prepended with 8-bit CRC8 checksum
 * - If CRC is 0xFF or packet looks like connection init, 0xFF escape byte added
 */

#ifndef CONTINUUM_CRYPT_H
#define CONTINUUM_CRYPT_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONTINUUM_KEY_WORDS 20
#define CONTINUUM_KEY_BYTES (CONTINUUM_KEY_WORDS * sizeof(u32))

typedef enum {
    CONT_STATE_NONE,       /* Encryption not started */
    CONT_STATE_EXPANDING,  /* Waiting for key expansion from oracle */
    CONT_STATE_WAITING,    /* Waiting for server acknowledgment (0x00 0x02) */
    CONT_STATE_INITIALIZED /* Ready for encrypted communication */
} ContinuumState;

typedef struct {
    ContinuumState state;
    u32 expanded_key[CONTINUUM_KEY_WORDS];  /* 80-byte expanded key */
    u32 key1;                                /* Our key (used in step 3) */
    u32 key2;                                /* Server's key (used in steps 1-2) */
    u32 key_send_tick;                       /* For resend timeout */
    u32 resend_count;
} ContinuumEncrypt;

/* Initialize Continuum encryption context */
void continuum_init(ContinuumEncrypt* ctx);

/* Set the partially expanded key (from oracle/scrty1, steps 1-2 done)
 * Then finalize with key1 (step 3 - modified MD5) */
void continuum_set_expanded_key(ContinuumEncrypt* ctx, const u32* partial_key);

/* Finalize key expansion (step 3)
 * Call this after setting partial key from oracle */
void continuum_finalize_expansion(ContinuumEncrypt* ctx, u32 key1);

/* Encrypt a packet
 * Prepends CRC8 checksum and applies block cipher
 * dest buffer must be at least size + 2 bytes
 * Returns size of encrypted data */
size_t continuum_encrypt(ContinuumEncrypt* ctx, const u8* src, u8* dest, size_t size);

/* Decrypt a packet in-place
 * Verifies CRC8 checksum
 * Returns size of decrypted data, or 0 if CRC check failed */
size_t continuum_decrypt(ContinuumEncrypt* ctx, u8* data, size_t size);

/* Low-level block cipher encrypt (for advanced use) */
void continuum_block_encrypt(void* dest, const void* src, u32 len, const u32* key);

/* Low-level block cipher decrypt (for advanced use) */
void continuum_block_decrypt(void* dest, const void* src, u32 len, const u32* key);

/* Check if encryption is initialized and ready */
int continuum_is_initialized(const ContinuumEncrypt* ctx);

/* Check if we're waiting for key expansion */
int continuum_is_expanding(const ContinuumEncrypt* ctx);

#ifdef __cplusplus
}
#endif

#endif /* CONTINUUM_CRYPT_H */
