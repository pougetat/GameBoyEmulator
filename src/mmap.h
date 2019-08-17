#include <stdio.h>
#include <stdint.h>

uint8_t * init_memory_map(FILE * rom_file);

// GB is little endian so we handle the conversion here
#define FETCH_16BIT_VAL(mmap, address) (((uint16_t) mmap[address+1]) << 8 | mmap[address])
#define FETCH_8BIT_VAL(mmap, address) (mmap[address])
#define FETCH_SIGNED_8BIT_VAL(mmap, address) ((int8_t) mmap[address])
#define STORE_8BIT_VAL(mmap, address, value) mmap[address] = value;