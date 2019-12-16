#include <stdio.h>
#include <stdint.h>

uint8_t * init_memory_map(FILE * rom_file);

typedef uint16_t memory_addr;

uint16_t fetch_16bit_val(uint8_t * memory_map, memory_addr address);
uint8_t fetch_8bit_val(uint8_t * memory_map, memory_addr address);
int8_t fetch_signed_8bit_val(uint8_t * memory_map, memory_addr address);
void store_16bit_val(uint8_t * memory_map, memory_addr address, uint16_t value);
void store_8bit_val(uint8_t * memory_map, memory_addr address, uint8_t value);