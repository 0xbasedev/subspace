/*
 * Continuum 0.40 Encryption Client
 * Checksum functions
 *
 * CRC8: Packet integrity (polynomial x^8 + x^5 + x^4 + 1)
 * CRC32: Map checksum, general data integrity
 * VIE Checksum: Client executable verification
 */

#ifndef CONTINUUM_CHECKSUM_H
#define CONTINUUM_CHECKSUM_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CRC8 checksum for packet integrity
 * Polynomial: x^8 + x^5 + x^4 + 1
 * Used by Continuum to verify encrypted packets */
u8 crc8(const u8* data, size_t len);

/* CRC8 with repeated byte value (optimization for repeated data) */
u8 crc8_repeat(u8 value, size_t len);

/* CRC32 checksum (standard polynomial 0xEDB88320) */
u32 crc32(const u8* data, size_t len);

/* CRC32 for map tiles (skips empty and certain tile types) */
u32 crc32_map(const u8* tiles, size_t len);

/* VIE client executable checksum
 * Returns a checksum value based on security key */
u32 vie_checksum(u32 key);

/* Settings checksum
 * Computes checksum of arena settings structure */
u32 settings_checksum(u32 key, const u32* settings_data, size_t count);

/* Weapon packet checksum (simple XOR) */
u8 weapon_checksum(const u8* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* CONTINUUM_CHECKSUM_H */
