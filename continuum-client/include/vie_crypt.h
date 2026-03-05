/*
 * Continuum 0.40 Encryption Client
 * VIE (SubSpace) Encryption
 *
 * This is the original SubSpace encryption from VIE (Virgin Interactive Entertainment).
 * It uses a PRNG-generated keystream with CBC-like XOR chaining.
 *
 * Continuum can fall back to VIE encryption if the server doesn't support
 * Continuum encryption, or if encryption is disabled (key == 0).
 */

#ifndef CONTINUUM_VIE_CRYPT_H
#define CONTINUUM_VIE_CRYPT_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VIE_KEYSTREAM_SIZE 520

typedef struct {
    u32 session_key;
    u32 client_key;
    u8  keystream[VIE_KEYSTREAM_SIZE];
} VieEncrypt;

/* Initialize VIE encryption context */
void vie_init(VieEncrypt* ctx);

/* Generate a random client key */
u32 vie_generate_key(void);

/* Initialize encryption with server's key response
 * Returns 1 on success, 0 if key is invalid */
int vie_initialize(VieEncrypt* ctx, u32 server_key);

/* Check if a server key is valid for our client key */
int vie_is_valid_key(VieEncrypt* ctx, u32 server_key);

/* Encrypt a packet
 * Returns size of encrypted data (same as input) */
size_t vie_encrypt(VieEncrypt* ctx, const u8* src, u8* dest, size_t size);

/* Decrypt a packet in-place
 * Returns size of decrypted data (same as input) */
size_t vie_decrypt(VieEncrypt* ctx, u8* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* CONTINUUM_VIE_CRYPT_H */
