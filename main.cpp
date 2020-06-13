#include <iostream>
#include <fstream>
#include "crc8debug.h"

int main() {
    uint8_t * table = new uint8_t[16];
    populate(table);

    char msg[] = "123";
    int datalen = strlen(msg);

    // Important note: msg[datalen] == 0
    // This is a crucial requirement in crc calculation (last N bits must be 0 for crcN)

    // Calculated by transmitter
    uint8_t tx_crc = crc8_descriptive(table, (uint8_t*)msg, datalen);
    msg[datalen] = tx_crc;

    // Now msg[datalen] == its crc
    // CRC is kinda mod division.
    // (a - (a mod b)) mod b == 0
    // CRC uses special addition operation such that a+b == a-b
    // So, crc(a + crc(a)) == 0 (that's why we need that empty space at the end of a)

    uint8_t rx_check = crc8_descriptive(table, (uint8_t*)msg, datalen+1);
    uint8_t rx_cont_check = 0x00;
    for (int i=0; i<datalen+1; i++) {
        rx_cont_check = crc8_descriptive(table, (uint8_t*)msg + i, 1, rx_cont_check);
    }

    // tx_crc - real CRC(a)
    // rx_check must be 0 for uncorrupted msg
    // rx_cont_check - same, but with segmented input

    std::cout << std::hex <<
    (unsigned int) tx_crc << " " <<
    (unsigned int) rx_check << " " <<
    (unsigned int) rx_cont_check << std::endl;
}
