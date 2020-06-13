#include <iostream>
#include <fstream>


void bin(uint8_t b) {
    for (int i=0; i<8; i++) {
        std::cout << ((b & 0x80U) ? '1' : '0');
        b = b << 1U;
    }
}
void bin(uint8_t * data, unsigned int n) {
    for (unsigned int i = 0; i<n; i++) {
        bin(data[i]);
//        std::cout << " ";
    }
}

void populate(uint8_t * table) {
    const uint16_t gen = 0x9BU << 4U;
    // crc: b00000000 - b11110000
    for (uint8_t quad = 0; quad < 16; quad++) {
        uint16_t crc = quad << 8U;
        for (unsigned int i=0; i<4; i++) {
            if ((0x800U >> i) & crc)
                crc ^= (gen >> (1+i));
        }
        table[quad] = crc;
    }
}
/*
 * 10100111 - gen
 *
 * 01001000 = crc
 * 01100101 = byte
 * * 01001000 00000000 = crc << 8
 * *     0111 0101     = table[crc>>4] << 4
 * * 01001111 01010000 = (crc<<8)^(table[crc>>4]<<4) = a
 * *          11010001 = table[(a>>8) & 0x0f]
 * * 01001111 10000001 = a ^ table[(a>>8) & 0x0f]
 * *          11100100 = (a^table[(a>>8) & 0x0f]) ^ byte
 *
 * 10100111
 * 11001000 11101010
 * 11000001 11101010
 *     1001 1111
 *          10100111
 *          10111101
 * 0000100000000000 = 0x800 */

uint8_t crc8_i(uint8_t * table, uint8_t * data, unsigned int n, uint8_t checkpoint=0x00U, bool debug=false) {
    uint8_t crc = checkpoint;
    if (debug) {
        uint8_t * copy = new uint8_t [n];
        memccpy(copy, data, sizeof(uint8_t), n);

        std::cout << "----" << std::endl;
        for (int i=0; i<n; i++) {
            std::cout << std::hex << (unsigned int) data[i] << " ";
        }
        std::cout << std::endl;
        bin(copy, n); std::cout << std::endl;

//        // [####](####)######## [] ~> ()
//        crc ^= table[crc>>4] >> 4;
//        // ....[(####)](####)#### [] ~> ()
//        copy[0] ^= table[crc>>4] << 4;
//        copy[0] ^= table[crc&0xf];

        copy[0] ^= checkpoint;
        bin(copy, n); std::cout << " - crc init applied" << std::endl;
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
    }
    else {
        for (int byte_idx = 0; byte_idx < n; byte_idx++) {
            uint8_t byte = data[byte_idx];
            bin(crc); std::cout << " "; bin(byte); std::cout << " ";
            crc = ((crc<<4) ^ table[crc>>4]) ^ (byte>>4);
            bin(crc); std::cout << " ";
            crc = ((crc<<4) ^ table[crc>>4]) ^ (byte & 0x0f);
            bin(crc); std::cout << std::endl;
        }
    }
    return crc;
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

int main() {
    uint8_t * table = new uint8_t[16];
    populate(table);

    char msg[] = "1ag";
    int datalen = strlen(msg);

    uint8_t tx_crc = crc8_i(table, (uint8_t*)msg, datalen+1);
    uint8_t rx_crc = crc8_i(table, (uint8_t*)msg, datalen, 0x00, true);

    msg[datalen] = rx_crc;
    uint8_t rx_check = crc8_i(table, (uint8_t*)msg, datalen+1, 0x00, true);
    uint8_t rx_cont_check = 0x00;
    for (int i=0; i<datalen+1; i++) {
        rx_cont_check = crc8_i(table, (uint8_t*)msg + i, 1, rx_cont_check);
    }

    std::cout << std::hex <<
    (unsigned int) tx_crc << " " <<
    (unsigned int) rx_crc << " " <<
    (unsigned int) rx_check << " " <<
    (unsigned int) rx_cont_check << std::endl;
}
