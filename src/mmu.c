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

void mmu_read_rom(struct Mmu * mmu, FILE * rom_file, memory_addr start_address)
{
    int byte;
    uint16_t cur_pos = start_address;
    for (memory_addr addr = 0; addr < start_address; addr++)
    {
        fgetc(rom_file);
    }
    while ((byte = fgetc(rom_file)) != EOF & cur_pos < MAX_ROM_SIZE)
    {
        mmu->memory_map[cur_pos++] = (uint8_t) byte;
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

void mmu_add_cartridge_logo(uint8_t * memory_map)
{
    memory_map[0x104] = 0xCE;memory_map[0x105] = 0xED;memory_map[0x106] = 0x66;
    memory_map[0x107] = 0x66;memory_map[0x108] = 0xCC;memory_map[0x109] = 0x0D;
    memory_map[0x10A] = 0x00;memory_map[0x10B] = 0x0B;memory_map[0x10C] = 0x03;
    memory_map[0x10D] = 0x73;memory_map[0x10E] = 0x00;memory_map[0x10F] = 0x83;
    memory_map[0x110] = 0x00;memory_map[0x111] = 0x0C;memory_map[0x112] = 0x00;
    memory_map[0x113] = 0x0D;memory_map[0x114] = 0x00;memory_map[0x115] = 0x08;
    memory_map[0x116] = 0x11;memory_map[0x117] = 0x1F;memory_map[0x118] = 0x88;
    memory_map[0x119] = 0x89;memory_map[0x11A] = 0x00;memory_map[0x11B] = 0x0E;
    memory_map[0x11C] = 0xDC;memory_map[0x11D] = 0xCC;memory_map[0x11E] = 0x6E;
    memory_map[0x11F] = 0xE6;memory_map[0x120] = 0xDD;memory_map[0x121] = 0xDD;
    memory_map[0x122] = 0xD9;memory_map[0x123] = 0x99;memory_map[0x124] = 0xBB;
    memory_map[0x125] = 0xBB;memory_map[0x126] = 0x67;memory_map[0x127] = 0x63;
    memory_map[0x128] = 0x6E;memory_map[0x129] = 0x0E;memory_map[0x12A] = 0xEC;
    memory_map[0x12B] = 0xCC;memory_map[0x12C] = 0xDD;memory_map[0x12D] = 0xDC;
    memory_map[0x12E] = 0x99;memory_map[0x12F] = 0x9F;memory_map[0x130] = 0xBB;
    memory_map[0x131] = 0xB9;memory_map[0x132] = 0x33;memory_map[0x133] = 0x3E;
}