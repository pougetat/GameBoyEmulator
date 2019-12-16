#include "mmu.h"
#include <string.h>
#include <stdlib.h>

#define MEMORY_MAP_SIZE 65536 // 0x0000 - 0xFFFF
#define MAX_ROM_SIZE 16384 // 0x0000 - 0x3FFF (only handling 32 KB roms for now)

uint8_t * init_memory_map(FILE * rom_file)
{    
    uint8_t * memory_map = malloc(MEMORY_MAP_SIZE);

    int byte;
    uint16_t cur_pos = 0;
    while ((byte = fgetc(rom_file)) != EOF & cur_pos < MAX_ROM_SIZE)
    {
        memory_map[cur_pos++] = (uint8_t) byte;
    }
    fclose(rom_file);

    for (int i = 0; i < 256; i++) {
        printf("%i : %x \n", i, memory_map[i]);
    }
    printf("\n");

    return memory_map;
}

// GB is little endian so we handle the conversion here
uint16_t fetch_16bit_val(uint8_t * memory_map, memory_addr address)
{
    return ((uint16_t) memory_map[address+1]) << 8 | memory_map[address];
}

uint8_t fetch_8bit_val(uint8_t * memory_map, memory_addr address)
{
    return memory_map[address];
}

int8_t fetch_signed_8bit_val(uint8_t * memory_map, memory_addr address)
{
    return (int8_t) memory_map[address];
}

void store_8bit_val(uint8_t * memory_map, memory_addr address, uint8_t value)
{
    memory_map[address] = value;
}

void store_16bit_val(uint8_t * memory_map, memory_addr address, uint16_t value)
{
    store_8bit_val(memory_map, address, (uint8_t) (value & 0xFF));
    store_8bit_val(memory_map, address+1, (uint8_t) (value >> 8));
}