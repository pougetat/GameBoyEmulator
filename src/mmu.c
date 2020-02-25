#include "mmu.h"
#include <string.h>
#include <stdlib.h>

#include "stdio.h"

Mmu * mmu_init()
{
    Mmu * mmu = malloc(sizeof(Mmu));
    mmu->memory_map = malloc(MEMORY_MAP_SIZE);

    return mmu;
}

void mmu_read_rom(uint8_t * memory_map, FILE * rom_file, memory_addr start, memory_addr end)
{
    int byte;
    uint16_t cur_pos = start;
    for (memory_addr addr = 0; addr < start; addr++)
    {
        fgetc(rom_file);
    }
    while ((byte = fgetc(rom_file)) != EOF & cur_pos < end & cur_pos < MAX_ROM_SIZE)
    {
        memory_map[cur_pos++] = (uint8_t) byte;
    }
    fclose(rom_file);
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

void mmu_store_8bit_val(uint8_t * memory_map, memory_addr address, uint8_t value)
{
    switch (address)
    {
        case R_BOOT_ROM_CONTROL:
            // disabling boot rom
            mmu_read_rom(memory_map, fopen("../testRoms/tetris.gb", "r"), 0, 0x100);
            break;
        default:
            break;
    }
    memory_map[address] = value;
}