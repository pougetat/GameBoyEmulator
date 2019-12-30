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

void gameboy_read_rom(GameBoy * gameboy_ptr, FILE * rom_file)
{
    mmu_read_rom(gameboy_ptr->mmu_ptr, rom_file);
}

void gameboy_run(GameBoy * gameboy_ptr)
{
    //while (gameboy_ptr->cpu_ptr->regPC != 0xA0)
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