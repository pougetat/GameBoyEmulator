#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "../src/gameboy.h"
#include "../src/mmu.h"
#include "../src/cpu.h"
#include "../src/ppu.h"

// These variables describe important boot rom instruction addresses
#define SETUP_VRAM_START_ADDRESS 0x3
#define SETUP_AUDIO_START_ADDRESS 0xc
#define SETUP_BG_PALETTE_START_ADDRESS 0x1d
#define CONVERT_AND_LOAD_LOGO_DATA_TO_VRAM 0x21
#define LOAD_ADDITIONAL_BYTES_TO_VRAM 0x34

// These variables describe the memory layout
#define VRAM_START_ADDRESS 0x8000
#define VRAM_END_ADDRESS 0x9FFF

void check_stack_setup(struct Cpu * cpu_ptr)
{
    assert(cpu_ptr->regSP == 0xFFFE);
}

void check_vram_setup(uint8_t * memory_map)
{
    for (memory_addr address = VRAM_START_ADDRESS; address <= VRAM_END_ADDRESS; address++)
    {
        assert(memory_map[address] == 0);
    }
}

void check_audio_setup(uint8_t * memory_map)
{
    assert(memory_map[0xFF11] == 0x80);   
    assert(memory_map[0xFF12] == 0xF3);
    assert(memory_map[0xFF24] == 0x77);   
    assert(memory_map[0xFF25] == 0xF3);   
    assert(memory_map[0xFF26] == 0x80);   
}

void check_background_palette_setup(uint8_t * memory_map)
{
    assert(memory_map[0xFF47] == 0xFC);
}

int main(void)
{
    GameBoy * gameboy_ptr = gameboy_init();
    FILE * rom_file = fopen("../testRoms/simple.gb", "r"); 
    gameboy_read_rom(gameboy_ptr, rom_file);
    gameboy_run(gameboy_ptr);

    Cpu * cpu_ptr = gameboy_ptr->cpu_ptr;
    uint8_t * memory_map = gameboy_ptr->mmu_ptr->memory_map;

    while (cpu_ptr->regPC != SETUP_VRAM_START_ADDRESS)
    {
        cpu_step(gameboy_ptr);
    }
    check_stack_setup(cpu_ptr);

    while (cpu_ptr->regPC != SETUP_AUDIO_START_ADDRESS)
    {
        cpu_step(gameboy_ptr);
    }
    check_vram_setup(memory_map);

    while (cpu_ptr->regPC != SETUP_BG_PALETTE_START_ADDRESS)
    {
        cpu_step(gameboy_ptr);
    }
    check_audio_setup(memory_map);

    while (cpu_ptr->regPC != CONVERT_AND_LOAD_LOGO_DATA_TO_VRAM)
    {
        cpu_step(gameboy_ptr);
    }
    check_background_palette_setup(memory_map);

    while (cpu_ptr->regPC != LOAD_ADDITIONAL_BYTES_TO_VRAM)
    {
        cpu_step(gameboy_ptr);
    }

    return 0;
}