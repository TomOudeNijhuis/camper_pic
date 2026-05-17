#include "../crc16.h"

uint16_t crc16_update(uint16_t crc, uint8_t b) {
    uint8_t i;
    crc ^= b;
    for (i = 0; i < 8; i++) {
        if (crc & 0x0001U) {
            crc = (uint16_t)((crc >> 1) ^ 0x8408U);
        } else {
            crc >>= 1;
        }
    }
    return crc;
}

uint16_t crc16(const uint8_t *data, uint8_t len) {
    uint16_t c = CRC16_INIT;
    uint8_t i;
    for (i = 0; i < len; i++) {
        c = crc16_update(c, data[i]);
    }
    return c;
}
