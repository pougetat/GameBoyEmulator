#include <stdlib.h>

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

#define PPU_FRAME_CYCLES 70224

void ppu_render_scanline(GameBoy * gameboy_ptr);
void ppu_synchronize_clocks(GameBoy * gameboy_ptr, Ppu * ppu_ptr);

Ppu * ppu_init()
{
    Ppu * ppu_ptr = malloc(sizeof(Ppu));
    ppu_ptr->ppu_cur_mode = 0;
    ppu_ptr->ppu_cur_mode_clock = 0;
    ppu_ptr->ppu_cur_frame_clock = 0;

    return ppu_ptr;
}

void ppu_step(GameBoy * gameboy_ptr)
{
    Ppu * ppu_ptr = gameboy_ptr->ppu_ptr;

    ppu_synchronize_clocks(gameboy_ptr, ppu_ptr);

    if (ppu_ptr->ppu_cur_mode == PPU_MODE_2 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_2_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;
        ppu_ptr->ppu_cur_mode = PPU_MODE_3;
    }
    else if (ppu_ptr->ppu_cur_mode == PPU_MODE_3 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_3_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;
        ppu_ptr->ppu_cur_mode = PPU_MODE_0;

        ppu_render_scanline(gameboy_ptr);
    }
    else if (ppu_ptr->ppu_cur_mode == PPU_MODE_0 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_0_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;

        if (ppu_ptr->ppu_cur_frame_clock >= PPU_FRAME_CYCLES)
        {
            ppu_ptr->ppu_cur_mode = PPU_MODE_1;
        }
        else
        {
            ppu_ptr->ppu_cur_mode = PPU_MODE_2;
        }
    }
    else if (ppu_ptr->ppu_cur_mode == PPU_MODE_1 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_1_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;
        ppu_ptr->ppu_cur_mode = PPU_MODE_2;
    }
}

void ppu_render_scanline(GameBoy * gameboy_ptr){}

void ppu_synchronize_clocks(GameBoy * gameboy_ptr, Ppu * ppu_ptr)
{
    uint8_t cycles = gameboy_prev_executed_cycles(gameboy_ptr);

    ppu_ptr->ppu_cur_mode_clock += cycles;
    ppu_ptr->ppu_cur_frame_clock += cycles;
}