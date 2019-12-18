#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <stdio.h>
#include <stdint.h>

typedef struct Cpu Cpu;
typedef struct Ppu Ppu;
typedef struct Mmu Mmu;

typedef struct GameBoy {
    Cpu * cpu_ptr;
    Ppu * ppu_ptr;
    Mmu * mmu_ptr;

    uint32_t * gameboy_clock;
} GameBoy;

GameBoy * gameboy_init();
void gameboy_read_rom(GameBoy * gameboy, FILE * rom_file);
void gameboy_run(GameBoy * gameboy);
uint8_t gameboy_prev_executed_cycles(GameBoy * gameboy);

#endif