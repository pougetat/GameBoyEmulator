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
#define STAT_MODE_1(STAT_REG)              (STAT_REG & 0b00000011) == 0b00000001
#define STAT_MATCH(STAT_REG)               (STAT_REG & 0b00000100) == 0b00000100
#define STAT_INT_MODE_0_FLAG 0b00001000
#define STAT_INT_MODE_1_FLAG 0b00010000
#define STAT_INT_MODE_2_FLAG 0b00100000
#define STAT_INT_MATCH_FLAG  0b01000000
#define STAT_INT_MODE_0_ENABLED(STAT_FLAG) (STAT_REG & STAT_INT_MODE_0_FLAG) == 0b00001000
#define STAT_INT_MODE_1_ENABLED(STAT_FLAG) (STAT_REG & STAT_INT_MODE_1_FLAG) == 0b00010000
#define STAT_INT_MODE_2_ENABLED(STAT_FLAG) (STAT_REG & STAT_INT_MODE_2_FLAG) == 0b00100000
#define STAT_INT_MATCH_ENABLED(STAT_FLAG)  (STAT_REG & STAT_INT_MATCH_FLAG) == 0b01000000

#define PPU_MODE_2_CYCLES 80    // OAM search : Scanning OAM for sprite coordinates that appear on this line
#define PPU_MODE_3_CYCLES 172   // Reading OAM and VRAM to generate picture
#define PPU_MODE_0_CYCLES 204   // H-blank
#define PPU_MODE_1_CYCLES 4560  // V-blank
#define PPU_LINE_CYCLES 456
#define PPU_MODE_1_BASE_LINE 144
#define PPU_FRAME_CYCLES 70224

// LCD related macros

#define SCREEN_PIXEL_HEIGHT 144
#define SCREEN_PIXEL_WIDTH 160
#define TILE_PIXEL_HEIGHT 8
#define TILE_PIXEL_WIDTH 8
#define BG_TILES_PER_ROW 32
#define BG_TILES_PER_COLUMN 32

// Functions used in this file

void fill_frame_pixel_line(uint8_t line, Gui * gui_ptr, uint8_t * memory_map);
uint8_t * get_tile_data_addr(uint8_t cur_x, uint8_t cur_y, uint8_t * memory_map);
uint8_t get_pixel(uint8_t cur_x, uint8_t cur_y, uint8_t * memory_map);
uint8_t get_tile_pixel(uint8_t tile_offset_x, uint8_t tile_offset_y, uint8_t * tile_data_addr);
void synchronize_clocks(GameBoy * gameboy_ptr, Ppu * ppu_ptr);
void change_stat_mode(Ppu * ppu_ptr, uint8_t * memory_map, uint8_t mode);

Ppu * ppu_init()
{
    Ppu * ppu_ptr = malloc(sizeof(Ppu));
    ppu_ptr->ppu_cur_mode_clock = 0;
    ppu_ptr->ppu_cur_frame_clock = 0;
    ppu_ptr->gui_ptr = gui_init();
    return ppu_ptr;
}

void ppu_step(GameBoy * gameboy_ptr)
{
    printf("ppu => step start \n");
    
    uint8_t * memory_map = gameboy_ptr->mmu_ptr->memory_map;
    Ppu * ppu_ptr = gameboy_ptr->ppu_ptr;
    debug_ppu(memory_map, ppu_ptr);

    if (!LCDC_DISPLAY_ENABLED(memory_map[R_LCDC_ADDR]))
    {
        return;
    }

    synchronize_clocks(gameboy_ptr, ppu_ptr);
    uint8_t stat_reg = memory_map[R_STAT_ADDR];

    if (STAT_MODE_2(stat_reg) && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_2_CYCLES)
    {
        change_stat_mode(ppu_ptr, memory_map, 3);
    }
    else if (STAT_MODE_3(stat_reg) && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_3_CYCLES)
    {
        change_stat_mode(ppu_ptr, memory_map, 0);
        uint8_t line_to_fill = memory_map[R_LY_ADDR];
        fill_frame_pixel_line(line_to_fill, ppu_ptr->gui_ptr, memory_map);
    }
    else if (STAT_MODE_0(stat_reg) && ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_0_CYCLES)
    {
        if (memory_map[R_LY_ADDR] == PPU_MODE_1_BASE_LINE - 1)
        {
            change_stat_mode(ppu_ptr, memory_map, 1);
            gui_render_frame(ppu_ptr->gui_ptr);
        }
        else
        {
            change_stat_mode(ppu_ptr, memory_map, 2);
        }
        memory_map[R_LY_ADDR]++;
    }
    else if (STAT_MODE_1(stat_reg))
    {
        if (ppu_ptr->ppu_cur_mode_clock >= PPU_MODE_1_CYCLES)
        {
            change_stat_mode(ppu_ptr, memory_map, 2);
            memory_map[R_LY_ADDR] = 0;
        }
        else
        {
            memory_map[R_LY_ADDR] = PPU_MODE_1_BASE_LINE + (uint8_t) (ppu_ptr->ppu_cur_mode_clock / PPU_LINE_CYCLES);
        }
    }
}

void fill_frame_pixel_line(uint8_t line, Gui * gui_ptr, uint8_t * memory_map)
{
    uint8_t cur_y = memory_map[R_SCY_ADDR] + memory_map[R_LY_ADDR];
    uint8_t cur_x = memory_map[R_SCX_ADDR];

    for (uint8_t i = 0; i < SCREEN_PIXEL_WIDTH; i++)
    {
        gui_ptr->frame_data[line][i] = get_pixel(cur_x + i, cur_y, memory_map);
    }
}

uint8_t * get_tile_data_addr(uint8_t cur_x, uint8_t cur_y, uint8_t * memory_map)
{
    // This function will be responsible for checking what tile data storage mode has
    // been chosen and fetching the correct tile data address

    uint8_t * bg_tilemap = memory_map + 0x9800;
    uint8_t * bg_tileset = memory_map + 0x9000;
    uint8_t tile_line = (uint8_t) (cur_y /  TILE_PIXEL_HEIGHT);
    uint8_t tile_column = (uint8_t) (cur_x / TILE_PIXEL_WIDTH);
    return bg_tileset + bg_tilemap[BG_TILES_PER_ROW * tile_line + tile_column];
}

uint8_t get_tile_pixel(uint8_t tile_offset_x, uint8_t tile_offset_y, uint8_t * tile_data_addr)
{
    // explanation here => https://www.huderlem.com/demos/gameboy2bpp.html
    uint8_t pixel_bit_high = (tile_data_addr[tile_offset_y * 2] & (0b10000000 >> tile_offset_x)) >> (TILE_PIXEL_WIDTH - tile_offset_x);
    uint8_t pixel_bit_low = (tile_data_addr[tile_offset_y * 2 + 1] & (0b10000000 >> tile_offset_x)) >> (TILE_PIXEL_WIDTH - tile_offset_x);
    return (pixel_bit_high << 1) | pixel_bit_low;
}

uint8_t get_pixel(uint8_t cur_x, uint8_t cur_y, uint8_t * memory_map)
{
    uint8_t * tile_data_addr = get_tile_data_addr(cur_x, cur_y, memory_map);
    return get_tile_pixel(cur_x % TILE_PIXEL_WIDTH, cur_y % TILE_PIXEL_HEIGHT, tile_data_addr);
}

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

void enable_stat_interrupts(uint8_t * memory_map, uint8_t interrupts)
{
    memory_map[R_STAT_ADDR] |= interrupts;
}

void clear_stat_interrupts(uint8_t * memory_map)
{
    memory_map[R_STAT_ADDR] &= 0b00000111;
}

void debug_ppu(uint8_t * memory_map, Ppu * ppu_ptr)
{
    printf("PPU state : \n \n");
    printf("    ppu cur mode clock = %i \n", ppu_ptr->ppu_cur_mode_clock);
    printf("    ppu cur frame clock = %i \n", ppu_ptr->ppu_cur_frame_clock);
    printf("    ppu cur mode = %i \n", memory_map[R_STAT_ADDR] & 0b11);
    printf("    LY = %i \n", memory_map[R_LY_ADDR]);

    printf("\n");
}