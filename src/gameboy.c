#include <stdbool.h>
#include <stdlib.h>

#include "gameboy.h"
#include "cpu.h"
#include "ppu.h"
#include "mmu.h"

GameBoy * gameboy_init()
{
    GameBoy * gameboy_ptr = malloc(sizeof(GameBoy));
    
    gameboy_ptr->cpu_ptr = cpu_init();
    gameboy_ptr->mmu_ptr = mmu_init();
    gameboy_ptr->ppu_ptr = ppu_init();
    gameboy_ptr->gameboy_clock = 0;

    return gameboy_ptr;
}

void gameboy_setup_boot(GameBoy * gameboy_ptr)
{
    FILE * bootrom_file = fopen("../testRoms/simple.gb", "r");
    mmu_read_rom(gameboy_ptr->mmu_ptr->memory_map, bootrom_file, 0, MAX_ROM_SIZE);
}

void gameboy_read_rom(GameBoy * gameboy_ptr, FILE * rom_file)
{
    mmu_read_rom(gameboy_ptr->mmu_ptr->memory_map, rom_file, 0x100, MAX_ROM_SIZE);
}

void gameboy_run(GameBoy * gameboy_ptr)
{
    while (true)
    {
        cpu_step(gameboy_ptr);
        ppu_step(gameboy_ptr);
    }
}

uint8_t gameboy_prev_executed_cycles(GameBoy * gameboy_ptr)
{
    return gameboy_ptr->cpu_ptr->prev_instruction_cycles;
}