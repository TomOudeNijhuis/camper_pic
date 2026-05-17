/*
 * crc16.h - CRC-16/MCRF4XX
 *
 * Poly 0x1021, init 0xFFFF, refin=true, refout=true, xorout=0x0000.
 * Reflected polynomial 0x8408 is used by the bitwise update.
 * Matches the bootloader's VERIFICATION="CRC-16" host-side scheme so
 * the same routine can verify firmware images and frame integrity.
 */

#ifndef CRC16_H
#define CRC16_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CRC16_INIT 0xFFFFU

uint16_t crc16_update(uint16_t crc, uint8_t b);
uint16_t crc16(const uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* CRC16_H */
