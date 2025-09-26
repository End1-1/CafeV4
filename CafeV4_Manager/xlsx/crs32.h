#ifndef CRS32_H
#define CRS32_H

#include <QtGlobal>

uint32_t crc32(const uint8_t *buf, size_t size)
{
    const uint8_t *p = buf;
    uint32_t crc32_table [256];
    quint32 crc;

    // initialize CRC table
    for (quint32 i = 0; i < 256; i++) {
        crc = i;
        for (int j = 0; j < 8; j++) {
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
        }
        crc32_table[i] = crc;
    }
    //and compute
    crc = 0xFFFFFFFF;
    while (size--) {
        crc = crc32_table[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFFUL;
 }

#endif // CRS32_H
