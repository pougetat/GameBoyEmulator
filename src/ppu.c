#include <stdlib.h>
#include <stdbool.h>

#include "ppu.h"
#include "mmu.h"
#include "gameboy.h"

// LCDC register related macros

#define LCDC_BG_ENABLED(LCDC_REG)                         (LCDC_REG & 0b00000001) == 1
#define LCDC_SPRITE_ENABLED(LCDC_REG)                     (LCDC_REG & 0b00000010) == 1
#define LCDC_SPRITE_SIZE_MODE_8x8(LCDC_REG)               (LCDC_REG & 0b00000100) == 0
#define LCDC_SPRITE_SIZE_MODE_8x16(LCDC_REG)              (LCDC_REG & 0b00000100) == 1
#define LCDC_BG_TILEMAP_ADDR_MODE_9800_9BFF(LCDC_REG)     (LCDC_REG & 0b00001000) == 0
#define LCDC_BG_TILEMAP_ADDR_MODE_9C00_9FFF(LCDC_REG)     (LCDC_REG & 0b00001000) == 1
#define LCDC_BG_WIN_TILEMAP_ADDR_MODE_8800_97FF(LCDC_REG) (LCDC_REG & 0b00010000) == 0
#define LCDC_BG_WIN_TILEMAP_ADDR_MODE_8000_8FFF(LCDC_REG) (LCDC_REG & 0b00010000) == 1
#define LCDC_WIN_ENABLED(LCDC_REG)                        (LCDC_REG & 0b00100000) == 1
#define LCDC_WIN_TILEMAP_ADDR_MODE_9800_9BFF(LCDC_REG)    (LCDC_REG & 0b01000000) == 0
#define LCDC_WIN_TILEMAP_ADDR_MODE_9C00_9FFF(LCDC_REG)    (LCDC_REG & 0b01000000) == 1
#define LCDC_DISPLAY_ENABLED(LCDC_REG)                    (LCDC_REG & 0b10000000) == 1

// STAT register related macros

#define STAT_MODE_2(STAT_REG)              (STAT_REG & 0b00000011) == 0b00000010
#define STAT_MODE_3(STAT_REG)              (STAT_REG & 0b00000011) == 0b00000011
#define STAT_MODE_0(STAT_REG)              (STAT_REG & 0b00000011) == 0b00000000
#define STAT_MODE_1(STAT_REG)              (STAT_REG & 0b00000011) == 0b00000010
#define STAT_MATCH(STAT_REG)               (STAT_REG & 0b00000100) == 0b00000100
#define STAT_INT_MODE_0_ENABLED(STAT_FLAG) (STAT_REG & 0b01111000) == 0b00001000
#define STAT_INT_MODE_1_ENABLED(STAT_FLAG) (STAT_REG & 0b01111000) == 0b00010000
#define STAT_INT_MODE_2_ENABLED(STAT_FLAG) (STAT_REG & 0b01111000) == 0b00100000
#define STAT_INT_MATCH_ENABLED(STAT_FLAG)  (STAT_REG & 0b01111000) == 0b01000000

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

    if (STAT_MODE_2(stat_reg) && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_2_CYCLES)
    {
        change_stat_mode(ppu_ptr, memory_map, 3);
    }
    else if (STAT_MODE_3(stat_reg) && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_3_CYCLES)
    {
        change_stat_mode(ppu_ptr, memory_map, 0);
        render_scanline(gameboy_ptr);
    }
    else if (STAT_MODE_0(stat_reg) && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_0_CYCLES)
    {
        if (ppu_ptr->ppu_cur_frame_clock >= PPU_FRAME_CYCLES)
        {
            change_stat_mode(ppu_ptr, memory_map, 1);
        }
        else
        {
            change_stat_mode(ppu_ptr, memory_map, 2);
        }
    }
    else if (STAT_MODE_1(stat_reg) && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_1_CYCLES)
    {
        change_stat_mode(ppu_ptr, memory_map, 2);
    }
}

void render_scanline(GameBoy * gameboy_ptr){}

void synchronize_clocks(GameBoy * gameboy_ptr, Ppu * ppu_ptr)
{
    uint8_t cycles = gameboy_prev_executed_cycles(gameboy_ptr);

    ppu_ptr->ppu_cur_mode_clock += cycles;
    ppu_ptr->ppu_cur_frame_clock += cycles;
}

void change_stat_mode(Ppu * ppu_ptr, uint8_t * memory_map, uint8_t mode)
{
    ppu_ptr->ppu_cur_mode_clock = 0;
    memory_map[R_STAT_ADDR] = (memory_map[R_STAT_ADDR] & 0b11111100) | mode;
}