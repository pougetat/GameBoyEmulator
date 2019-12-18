#ifndef PPU_H
#define PPU_H

#include <stdint.h>

typedef struct GameBoy GameBoy;

typedef struct Ppu {
    uint8_t ppu_cur_mode;
    uint32_t ppu_cur_mode_clock;
    uint32_t ppu_cur_frame_clock;
} Ppu;

void ppu_step(GameBoy * gameboy_ptr);

#endif