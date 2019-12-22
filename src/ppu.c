#include <stdlib.h>
#include <stdbool.h>

#include "ppu.h"
#include "mmu.h"
#include "gameboy.h"

// LCDC register related macros

#define LCDC_BG_ENABLED(LCDC_REG)               LCDC_REG & 0b00000001
#define LCDC_SPRITE_ENABLED(LCDC_REG)           LCDC_REG & 0b00000010
#define LCDC_SPRITE_SIZE_MODE(LCDC_REG)         LCDC_REG & 0b00000100
#define LCDC_BG_TILEMAP_ADDR_MODE(LCDC_REG)     LCDC_REG & 0b00001000
#define LCDC_BG_WIN_TILEMAP_ADDR_MODE(LCDC_REG) LCDC_REG & 0b00010000
#define LCDC_WIN_ENABLED(LCDC_REG)              LCDC_REG & 0b00100000
#define LCDC_WIN_TILEMAP_ADDR_MODE(LCDC_REG)    LCDC_REG & 0b01000000
#define LCDC_DISPLAY_ENABLED(LCDC_REG)          LCDC_REG & 0b10000000

#define LCDC_SPRITE_SIZE_MODE_8x8               0
#define LCDC_SPRITE_SIZE_MODE_8x16              1
#define LCDC_BG_TILEMAP_ADDR_MODE_9800_9BFF     0
#define LCDC_BG_TILEMAP_ADDR_MODE_9C00_9FFF     1
#define LCDC_BG_WIN_TILEMAP_ADDR_MODE_8800_97FF 0
#define LCDC_BG_WIN_TILEMAP_ADDR_MODE_8000_8FFF 1
#define LCDC_WIN_TILEMAP_ADDR_MODE_9800_9BFF    0
#define LCDC_WIN_TILEMAP_ADDR_MODE_9C00_9FFF    1

// STAT register related macros

#define STAT_MODE(STAT_REG)           STAT_REG & 0b00000011
#define STAT_MATCH(STAT_REG)          STAT_REG & 0b00000100
#define STAT_INT_SELECTION(STAT_FLAG) STAT_REG & 0b01111000

#define STAT_MODE_2             0b10   // Scanline (Accessing OAM)
#define STAT_MODE_3             0b11   // Scanline (Accessing VRAM)
#define STAT_MODE_0             0b00   // Horizontal blank 
#define STAT_MODE_1             0b01   // Vertical blank
#define STAT_INT_MODE_0_ENABLED 0b0001
#define STAT_INT_MODE_1_ENABLED 0b0010
#define STAT_INT_MODE_2_ENABLED 0b0100
#define STAT_INT_MATCH_ENABLED  0b1000

#define PPU_MODE_2_CYCLES 80
#define PPU_MODE_3_CYCLES 172
#define PPU_MODE_0_CYCLES 204
#define PPU_MODE_1_CYCLES 4560

#define PPU_FRAME_CYCLES 70224

Ppu * ppu_init()
{
    Ppu * ppu_ptr = malloc(sizeof(Ppu));
    ppu_ptr->ppu_cur_mode_clock = 0;
    ppu_ptr->ppu_cur_frame_clock = 0;

    return ppu_ptr;
}

void ppu_step(GameBoy * gameboy_ptr)
{
    Ppu * ppu_ptr = gameboy_ptr->ppu_ptr;
    synchronize_clocks(gameboy_ptr, ppu_ptr);

    uint8_t * memory_map = gameboy_ptr->mmu_ptr->memory_map;
    uint8_t stat_reg = memory_map[R_STAT_ADDR];

    if (STAT_MODE(stat_reg) == STAT_MODE_2 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_2_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;
        // CHANGE STAT MODE TO 3
        
    }
    else if (STAT_MODE(stat_reg) == STAT_MODE_3 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_3_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;
        // CHANGE STAT MODE TO 0

        render_scanline(gameboy_ptr);
    }
    else if (STAT_MODE(stat_reg) == STAT_MODE_0 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_0_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;

        if (ppu_ptr->ppu_cur_frame_clock >= PPU_FRAME_CYCLES)
        {
            // CHANGE STAT MODE TO 1
        }
        else
        {
            // CHANGE STAT MODE TO 2
        }
    }
    else if (STAT_MODE(stat_reg) == STAT_MODE_1 && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_1_CYCLES)
    {
        ppu_ptr->ppu_cur_mode_clock = 0;
        // CHANGE STAT MODE TO 2
    }
}

void render_scanline(GameBoy * gameboy_ptr){}

void synchronize_clocks(GameBoy * gameboy_ptr, Ppu * ppu_ptr)
{
    uint8_t cycles = gameboy_prev_executed_cycles(gameboy_ptr);

    ppu_ptr->ppu_cur_mode_clock += cycles;
    ppu_ptr->ppu_cur_frame_clock += cycles;
}