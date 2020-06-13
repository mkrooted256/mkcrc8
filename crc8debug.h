#ifndef CRC8DEBUG_H
#define CRC8DEBUG_H

#include <iostream>
#include <fstream>
#include "crc8.h"

void bin(uint8_t b) {
    for (int i=0; i<8; i++) {
        std::cout << ((b & 0x80U) ? '1' : '0');
        b = b << 1U;
    }
}
void bin(uint8_t * data, unsigned int n) {
    for (unsigned int i = 0; i<n; i++) {
        bin(data[i]);
    }
}

void logtable(uint8_t* table) {
    std::ofstream of("table.txt");
    for (int h=0; h<16; h++) {
        for (int i=0; i<8; i++) {
            of << ((h & (0x80U>>i)) ? '1' : '0');
        }
        of << " - ";
        for (int i=0; i<8; i++) {
            of << ((table[h] & (0x80U>>i)) ? '1' : '0');
        }
        of << std::endl;
    }
}

uint8_t crc8_descriptive(uint8_t * table, uint8_t * data, unsigned int n, uint8_t checkpoint=0x00U) {
    uint8_t crc;
    uint8_t * copy = new uint8_t [n];
    memccpy(copy, data, sizeof(uint8_t), n);

    std::cout << "----" << std::endl;
    for (int i=0; i<n; i++) {
        std::cout << std::hex << (unsigned int) data[i] << " ";
    }
    std::cout << std::endl;
    bin(copy, n); std::cout << std::endl;

    copy[0] ^= checkpoint;
    bin(copy, n); std::cout << " - checkpoint applied" << std::endl;
    crc = 0;

    for (int byte_idx = 0; byte_idx < n; byte_idx++) {
        uint8_t &byte = copy[byte_idx];

        uint8_t trail = table[byte >> 4];
        byte = (byte & 0xf) ^ (trail >> 4);
        if (byte_idx < n-1) {
            copy[byte_idx+1] ^= trail << 4;
        } else {
            crc = trail << 4;
        }
        bin(copy, n); std::cout << "|"; bin(crc); std::cout << std::endl;


        trail = table[byte]; // byte is already in form 0000xxxx
        byte = 0;
        if (byte_idx < n-1) {
            copy[byte_idx+1] ^= trail;
        } else {
            crc ^= trail;
        }
        bin(copy, n); std::cout << "|"; bin(crc); std::cout << std::endl;
    }
    std::cout << std::hex << (unsigned int) (crc & 0xff) << std::endl;
    return crc;
}

#endif //CRC8DEBUG_H
