#include <stdlib.h>
#include <stdbool.h>

#include "ppu.h"
#include "mmu.h"
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

#define LCDC_SPRITE_SIZE_MODE_8x8               0
#define LCDC_SPRITE_SIZE_MODE_8x16              1
#define LCDC_BG_TILEMAP_ADDR_MODE_9800_9BFF     0
#define LCDC_BG_TILEMAP_ADDR_MODE_9C00_9FFF     1
#define LCDC_BG_WIN_TILEMAP_ADDR_MODE_8800_97FF 0
#define LCDC_BG_WIN_TILEMAP_ADDR_MODE_8000_8FFF 1
#define LCDC_WIN_TILEMAP_ADDR_MODE_9800_9BFF    0
#define LCDC_WIN_TILEMAP_ADDR_MODE_9C00_9FFF    1

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

    synchronize_clocks(gameboy_ptr, ppu_ptr);

    if (ppu_ptr->ppu_cur_mode == PPU_MODE_2 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_2_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;
        ppu_ptr->ppu_cur_mode = PPU_MODE_3;
    }
    else if (ppu_ptr->ppu_cur_mode == PPU_MODE_3 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_3_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;
        ppu_ptr->ppu_cur_mode = PPU_MODE_0;

        render_scanline(gameboy_ptr);
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

void render_scanline(GameBoy * gameboy_ptr){}

void synchronize_clocks(GameBoy * gameboy_ptr, Ppu * ppu_ptr)
{
    uint8_t cycles = gameboy_prev_executed_cycles(gameboy_ptr);

    ppu_ptr->ppu_cur_mode_clock += cycles;
    ppu_ptr->ppu_cur_frame_clock += cycles;
}

// LCDC functions

bool    lcdc_bg_enabled(uint8_t * memory_map) { return (memory_map[R_LCDC_ADDR] & 0b1) != 0; }
bool    lcdc_sprite_enabled(uint8_t * memory_map) { return (memory_map[R_LCDC_ADDR] & 0b10) != 0; }
uint8_t lcdc_sprite_size_mode(uint8_t * memmory_map) { return memory_map[R_LCDC_ADDR] & 0b100; }
uint8_t lcdc_bg_tilemap_addr_mode(uint8_t * memory_map) { return memory_map[R_LCDC_ADDR] & 0b1000; }
uint8_t lcdc_bg_win_tilemap_addr_mode(uint8_t * memory_map) { return memory_map[R_LCDC_ADDR] & 0b10000; }
bool    lcdc_win_enabled(uint8_t * memory_map) { return (memory_map[R_LCDC_ADDR] & 0b100000) != 0; }
uint8_t lcdc_win_tilemap_addr_mode(uint8_t * memory_map) { return memory_map[R_LCDC_ADDR] & 0b1000000; }
bool    lcdc_display_enabled(uint8_t * memory_map) { return (memory_map[R_LCDC_ADDR] & 0b10000000) != 0; }