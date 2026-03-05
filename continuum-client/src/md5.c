/*
 * Continuum 0.40 Encryption Client
 * Modified MD5 implementation
 *
 * Derived from the RSA Data Security, Inc. MD5 Message Digest Algorithm.
 * Modifications for Continuum:
 * - Magic initialization values adjusted
 * - "Mangler" constant XORed into state transforms
 *
 * Original copyright:
 * Copyright (C) 1990, RSA Data Security, Inc. All rights reserved.
 */

#include "md5.h"
#include <string.h>

static void Transform(u32* buf, u32* in, u32 mangler);

static const u8 PADDING[64] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F, G, H, I are basic MD5 functions */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* FF, GG, HH, II transformations for rounds 1, 2, 3, 4 */
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F((b), (c), (d)) + (x) + (u32)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G((b), (c), (d)) + (x) + (u32)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H((b), (c), (d)) + (x) + (u32)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
    (a) += I((b), (c), (d)) + (x) + (u32)(ac); \
    (a) = ROTATE_LEFT((a), (s)); \
    (a) += (b); \
}

void MD5Init(MD5_CTX* ctx, u32 mangler) {
    ctx->mangler = mangler;
    ctx->i[0] = ctx->i[1] = 0;

    /* Modified magic initialization constants (XORed with mangler) */
    ctx->buf[0] = 0x67352301 ^ mangler;
    ctx->buf[1] = 0xefcdab79 ^ mangler;
    ctx->buf[2] = 0x98baccfe ^ mangler;
    ctx->buf[3] = 0x11325476 ^ mangler;
}

void MD5Update(MD5_CTX* ctx, const u8* data, u32 len) {
    u32 in[16];
    int mdi;
    u32 i, ii;

    /* Compute number of bytes mod 64 */
    mdi = (int)((ctx->i[0] >> 3) & 0x3F);

    /* Update number of bits */
    if ((ctx->i[0] + ((u32)len << 3)) < ctx->i[0])
        ctx->i[1]++;
    ctx->i[0] += ((u32)len << 3);
    ctx->i[1] += ((u32)len >> 29);

    while (len--) {
        ctx->in[mdi++] = *data++;

        if (mdi == 0x40) {
            for (i = 0, ii = 0; i < 16; i++, ii += 4) {
                in[i] = (((u32)ctx->in[ii + 3]) << 24) |
                        (((u32)ctx->in[ii + 2]) << 16) |
                        (((u32)ctx->in[ii + 1]) << 8) |
                        ((u32)ctx->in[ii]);
            }
            Transform(ctx->buf, in, ctx->mangler);
            mdi = 0;
        }
    }
}

void MD5Final(MD5_CTX* ctx) {
    u32 in[16];
    int mdi;
    u32 i, ii;
    u32 padLen;

    /* Save number of bits */
    in[14] = ctx->i[0];
    in[15] = ctx->i[1];

    /* Compute number of bytes mod 64 */
    mdi = (int)((ctx->i[0] >> 3) & 0x3F);

    /* Pad out to 56 mod 64 */
    padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
    MD5Update(ctx, PADDING, padLen);

    /* Append length in bits and transform */
    for (i = 0, ii = 0; i < 14; i++, ii += 4) {
        in[i] = (((u32)ctx->in[ii + 3]) << 24) |
                (((u32)ctx->in[ii + 2]) << 16) |
                (((u32)ctx->in[ii + 1]) << 8) |
                ((u32)ctx->in[ii]);
    }
    Transform(ctx->buf, in, ctx->mangler);

    /* Store buffer in digest */
    for (i = 0, ii = 0; i < 4; i++, ii += 4) {
        ctx->digest[ii]     = (u8)(ctx->buf[i] & 0xFF);
        ctx->digest[ii + 1] = (u8)((ctx->buf[i] >> 8) & 0xFF);
        ctx->digest[ii + 2] = (u8)((ctx->buf[i] >> 16) & 0xFF);
        ctx->digest[ii + 3] = (u8)((ctx->buf[i] >> 24) & 0xFF);
    }
}

/* MD5 Transform - constants XORed with mangler */
static void Transform(u32* buf, u32* in, u32 mangler) {
    u32 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

    /* Round 1 */
    #define S11 7
    #define S12 12
    #define S13 17
    #define S14 22
    FF(a, b, c, d, in[0],  S11, 0xd76a5478 ^ mangler);
    FF(d, a, b, c, in[1],  S12, 0xe867b756 ^ mangler);
    FF(c, d, a, b, in[2],  S13, 0x242073db ^ mangler);
    FF(b, c, d, a, in[3],  S14, 0xc18d3eee ^ mangler);
    FF(a, b, c, d, in[4],  S11, 0xf57308af ^ mangler);
    FF(d, a, b, c, in[5],  S12, 0x4787c62a ^ mangler);
    FF(c, d, a, b, in[6],  S13, 0xa8384613 ^ mangler);
    FF(b, c, d, a, in[7],  S14, 0xfd469501 ^ mangler);
    FF(a, b, c, d, in[8],  S11, 0x688098d8 ^ mangler);
    FF(d, a, b, c, in[9],  S12, 0x8b44f7af ^ mangler);
    FF(c, d, a, b, in[10], S13, 0xff8f5bb1 ^ mangler);
    FF(b, c, d, a, in[11], S14, 0x898cd7be ^ mangler);
    FF(a, b, c, d, in[12], S11, 0x6b901822 ^ mangler);
    FF(d, a, b, c, in[13], S12, 0xfd987193 ^ mangler);
    FF(c, d, a, b, in[14], S13, 0xa679438e ^ mangler);
    FF(b, c, d, a, in[15], S14, 0x49740821 ^ mangler);

    /* Round 2 */
    #define S21 5
    #define S22 9
    #define S23 14
    #define S24 20
    GG(a, b, c, d, in[1],  S21, 0xf6112562 ^ mangler);
    GG(d, a, b, c, in[6],  S22, 0xc540b340 ^ mangler);
    GG(c, d, a, b, in[11], S23, 0x265e4a51 ^ mangler);
    GG(b, c, d, a, in[0],  S24, 0xe9b4c7aa ^ mangler);
    GG(a, b, c, d, in[5],  S21, 0xd627105d ^ mangler);
    GG(d, a, b, c, in[10], S22, 0x2541453 ^ mangler);
    GG(c, d, a, b, in[15], S23, 0xd8a15681 ^ mangler);
    GG(b, c, d, a, in[4],  S24, 0xe7d3fbc8 ^ mangler);
    GG(a, b, c, d, in[9],  S21, 0x21e4cde6 ^ mangler);
    GG(d, a, b, c, in[14], S22, 0xc33707d6 ^ mangler);
    GG(c, d, a, b, in[3],  S23, 0xf4d50387 ^ mangler);
    GG(b, c, d, a, in[8],  S24, 0x455a14ed ^ mangler);
    GG(a, b, c, d, in[13], S21, 0xa933e905 ^ mangler);
    GG(d, a, b, c, in[2],  S22, 0xfcefa3f8 ^ mangler);
    GG(c, d, a, b, in[7],  S23, 0x676f01d9 ^ mangler);
    GG(b, c, d, a, in[12], S24, 0x8d294c8a ^ mangler);

    /* Round 3 */
    #define S31 4
    #define S32 11
    #define S33 16
    #define S34 23
    HH(a, b, c, d, in[5],  S31, 0xf1fa3942 ^ mangler);
    HH(d, a, b, c, in[8],  S32, 0x8771f681 ^ mangler);
    HH(c, d, a, b, in[11], S33, 0x6d3d6122 ^ mangler);
    HH(b, c, d, a, in[14], S34, 0xfde5380c ^ mangler);
    HH(a, b, c, d, in[1],  S31, 0xa4be6a44 ^ mangler);
    HH(d, a, b, c, in[4],  S32, 0x4bdec3a9 ^ mangler);
    HH(c, d, a, b, in[7],  S33, 0xf6b84b60 ^ mangler);
    HH(b, c, d, a, in[10], S34, 0xbebfbc70 ^ mangler);
    HH(a, b, c, d, in[13], S31, 0x28937ec6 ^ mangler);
    HH(d, a, b, c, in[0],  S32, 0xeaa127fa ^ mangler);
    HH(c, d, a, b, in[3],  S33, 0xd4ef8085 ^ mangler);
    HH(b, c, d, a, in[6],  S34, 0x4881d05 ^ mangler);
    HH(a, b, c, d, in[9],  S31, 0xd9d4d339 ^ mangler);
    HH(d, a, b, c, in[12], S32, 0xe66b99e5 ^ mangler);
    HH(c, d, a, b, in[15], S33, 0x1fa27cf8 ^ mangler);
    HH(b, c, d, a, in[2],  S34, 0xc4ac3665 ^ mangler);

    /* Round 4 */
    #define S41 6
    #define S42 10
    #define S43 15
    #define S44 21
    II(a, b, c, d, in[0],  S41, 0xf4342244 ^ mangler);
    II(d, a, b, c, in[7],  S42, 0x432a6f97 ^ mangler);
    II(c, d, a, b, in[14], S43, 0xab9423a7 ^ mangler);
    II(b, c, d, a, in[5],  S44, 0xfc95a039 ^ mangler);
    II(a, b, c, d, in[12], S41, 0x655559c3 ^ mangler);
    II(d, a, b, c, in[3],  S42, 0x8f0ccc92 ^ mangler);
    II(c, d, a, b, in[10], S43, 0xf5eff47d ^ mangler);
    II(b, c, d, a, in[1],  S44, 0x85845dd1 ^ mangler);
    II(a, b, c, d, in[8],  S41, 0x6fa57e4f ^ mangler);
    II(d, a, b, c, in[15], S42, 0xf52ce6e0 ^ mangler);
    II(c, d, a, b, in[6],  S43, 0xa3014514 ^ mangler);
    II(b, c, d, a, in[13], S44, 0x4e0851a1 ^ mangler);
    II(a, b, c, d, in[4],  S41, 0xf7557e82 ^ mangler);
    II(d, a, b, c, in[11], S42, 0x453af235 ^ mangler);
    II(c, d, a, b, in[2],  S43, 0x2ad782bb ^ mangler);
    II(b, c, d, a, in[9],  S44, 0xeb56d391 ^ mangler);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}
