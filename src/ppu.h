#ifndef PPU_H
#define PPU_H

#include <stdint.h>

#include "gui/gui.h"

typedef struct GameBoy GameBoy;

typedef struct Ppu {
    uint32_t ppu_cur_mode_clock;
    uint32_t ppu_cur_frame_clock;
    Gui * gui_ptr;
} Ppu;

Ppu * ppu_init();
void ppu_step(GameBoy * gameboy_ptr);

#endif