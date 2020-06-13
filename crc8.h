#ifndef MKCRC8_H
#define MKCRC8_H

void populate(uint8_t * table, uint8_t generator=0x9BU) {
    const uint16_t gen = generator << 4U;
    for (uint8_t quad = 0; quad < 16; quad++) {
        uint16_t crc = quad << 8U;
        for (unsigned int i=0; i<4; i++) {
            if ((0x800U >> i) & crc)
                crc ^= (gen >> (1+i));
        }
        table[quad] = crc;
    }
}
uint8_t crc8(uint8_t * table, uint8_t * data, unsigned int datalen, uint8_t checkpoint=0x00U) {
    uint8_t crc = checkpoint;
    for (int byte_idx = 0; byte_idx < datalen; byte_idx++) {
        uint8_t byte = data[byte_idx];
        crc = ((crc<<4) ^ table[crc>>4]);
        crc = ((crc<<4) ^ table[crc>>4]);
        crc ^= byte;
    }
    return crc;
}

#endif //AUX_MKCRC_H
