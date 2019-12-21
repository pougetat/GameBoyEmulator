#include "mmu.h"
#include <string.h>
#include <stdlib.h>

#include "stdio.h"

#define MEMORY_MAP_SIZE 65536 // 0x0000 - 0xFFFF
#define MAX_ROM_SIZE 16384 // 0x0000 - 0x3FFF (only handling 32 KB roms for now)

Mmu * mmu_init()
{
    Mmu * mmu = malloc(sizeof(Mmu));
    mmu->memory_map = malloc(MEMORY_MAP_SIZE);

    return mmu;
}

void mmu_read_rom(struct Mmu * mmu, FILE * rom_file)
{
    int byte;
    uint16_t cur_pos = 0;
    while ((byte = fgetc(rom_file)) != EOF & cur_pos < MAX_ROM_SIZE)
    {
        mmu->memory_map[cur_pos++] = (uint8_t) byte;
    }
    fclose(rom_file);

    for (int i = 0; i < 256; i++) {
        printf("%i : %x \n", i, mmu->memory_map[i]);
    }
    printf("\n");
}

// GB is little endian so we handle the conversion here
uint16_t mmu_fetch_16bit_val(uint8_t * memory_map, memory_addr address)
{
    return ((uint16_t) memory_map[address+1]) << 8 | memory_map[address];
}

int8_t mmu_fetch_signed_8bit_val(uint8_t * memory_map, memory_addr address)
{
    return (int8_t) memory_map[address];
}

void mmu_store_16bit_val(uint8_t * memory_map, memory_addr address, uint16_t value)
{
    memory_map[address] = (uint8_t) (value & 0xFF);
    memory_map[address+1] = (uint8_t) (value >> 8);
}