# mk/CRC8
This is my implementation of CRC8 checksum calculation algorithm.

Functions in the crc8.h are ready-to-use. `populate()` needs an empty `uint8_t[]` with size of at least 16. 
This array will be used then as a first argument to `crc8()`.

`crc8()` also supports segmented input, that is, `A == B` in the following snippet:
```
// uint8_t * data; int datalen;
// uint8_t * table;

uint8_t A = crc8(table, data, datalen);

uint8_t B = 0;
for (int i=0; i<datalen; i++) B = crc8(table, data+i, 1, B);

// assert(A == B);
```

`main.cpp` contains debugging version of algorithm and development-purpose garbage (I am going to clean things up soon) 

`table.txt` contains description of the table contents for the `0x9B` generative polynomial