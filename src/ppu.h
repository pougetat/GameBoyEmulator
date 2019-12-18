#ifndef PPU_H
#define PPU_H

#include <stdint.h>

typedef struct GameBoy GameBoy;

typedef struct Ppu {
    uint32_t * ppu_clock;
} Ppu;

void ppu_step(GameBoy * gameboy_ptr);

#endif