/*
 * Continuum 0.40 Encryption Client
 * Modified MD5 implementation
 *
 * This is a modified version of the RSA Data Security MD5 algorithm.
 * The modifications include:
 * - Magic initialization values adjusted
 * - "Mangler" constant XORed into state transforms
 *
 * Used in key expansion step 3.
 *
 * Original copyright:
 * Copyright (C) 1990, RSA Data Security, Inc. All rights reserved.
 */

#ifndef CONTINUUM_MD5_H
#define CONTINUUM_MD5_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    u32 mangler;           /* Continuum-specific mangler constant */
    u32 i[2];              /* Number of bits handled mod 2^64 */
    u32 buf[4];            /* Scratch buffer / state */
    u8  in[64];            /* Input buffer */
    u8  digest[16];        /* Final digest after MD5Final */
} MD5_CTX;

/* Initialize MD5 context with Continuum-specific mangler
 * For key expansion, use mangler = 432 */
void MD5Init(MD5_CTX* ctx, u32 mangler);

/* Update MD5 state with input data */
void MD5Update(MD5_CTX* ctx, const u8* data, u32 len);

/* Finalize MD5 and produce digest */
void MD5Final(MD5_CTX* ctx);

#ifdef __cplusplus
}
#endif

#endif /* CONTINUUM_MD5_H */
