/*
 * Continuum 0.40 Encryption Client
 * Type definitions
 *
 * Based on reverse engineering work from:
 * - sharvil/subspace (protocol documentation)
 * - plushmonkey/nullspace (encryption implementation)
 * - fcxcode/eg-asss (VIE encryption)
 */

#ifndef CONTINUUM_TYPES_H
#define CONTINUUM_TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

#define CONTINUUM_MAX_PACKET_SIZE 520

#endif /* CONTINUUM_TYPES_H */
