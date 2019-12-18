#include "ppu.h"
#include "gameboy.h"

#define PPU_MODE_2 2  // Scanline (Accessing OAM)
#define PPU_MODE_3 3  // Scanline (Accessing VRAM)
#define PPU_MODE_0 0  // Horizontal blank 
#define PPU_MODE_1 1  // Vertical blank

#define PPU_MODE_2_CYCLES 80
#define PPU_MODE_3_CYCLES 172
#define PPU_MODE_0_CYCLES 204
#define PPU_MODE_1_CYCLES 4560

void ppu_render_scanline(GameBoy * gameboy_ptr);

void ppu_step(GameBoy * gameboy_ptr)
{
    Ppu * ppu_ptr = gameboy_ptr->ppu_ptr;

    if (ppu_ptr->ppu_mode == PPU_MODE_2 && ppu_ptr->ppu_mode_clock > PPU_MODE_2_CYCLES)
    {
        ppu_ptr->ppu_mode_clock = 0;
        ppu_ptr->ppu_mode = PPU_MODE_3;
    }
    else if (ppu_ptr->ppu_mode == PPU_MODE_3 && ppu_ptr->ppu_mode_clock > PPU_MODE_3_CYCLES)
    {
        ppu_ptr->ppu_mode_clock = 0;
        ppu_ptr->ppu_mode = PPU_MODE_0;

        ppu_render_scanline(gameboy_ptr);
    }
    else if (ppu_ptr->ppu_mode == PPU_MODE_0 && ppu_ptr->ppu_mode_clock > PPU_MODE_0_CYCLES)
    {
        ppu_ptr->ppu_mode_clock = 0;
        ppu_ptr->ppu_mode = PPU_MODE_1;
    }
    else if (ppu_ptr->ppu_mode == PPU_MODE_1 && ppu_ptr->ppu_mode_clock > PPU_MODE_1_CYCLES)
    {
        ppu_ptr->ppu_mode_clock = 0;
        ppu_ptr->ppu_mode = PPU_MODE_2;
    }
}

void ppu_render_scanline(GameBoy * gameboy_ptr){}