#include <stdio.h>
#include <stdint.h>

uint8_t * init_memory_map(FILE * rom_file);

// GB is little endian so we handle the conversion here
#define FETCH_16BIT_VAL(memory_map, address) (((uint16_t) memory_map[address+1]) << 8 | memory_map[address])
#define FETCH_8BIT_VAL(memory_map, address) (memory_map[address])
#define FETCH_SIGNED_8BIT_VAL(memory_map, address) ((int8_t) memory_map[address])
#define STORE_8BIT_VAL(memory_map, address, value) memory_map[address] = value;
#define STORE_16BIT_VAL(memory_map, address, value) \
    STORE_8BIT_VAL(memory_map, address, (uint8_t) (value & 0xFF)); \
    STORE_8BIT_VAL(memory_map, address+1, (uint8_t) (value >> 8));