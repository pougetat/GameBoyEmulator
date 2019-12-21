#ifndef MMU_H
#define MMU_H

#include <stdio.h>
#include <stdint.h>

typedef struct GameBoy GameBoy;
typedef uint16_t memory_addr;

typedef struct Mmu {
    uint8_t * memory_map;
} Mmu;

// memory mapped register address

    // LCD related registers

#define R_LCDC_ADDR 0xFF40 // LCD contol (R/W)
#define R_STAT_ADDR 0xFF41 // LCDC status (R/W)
#define R_SCY_ADDR 0xFF42  // Scroll Y (R/W)
#define R_SCX_ADDR 0xFF43  // Scroll X (R/W)
#define R_LY_ADDR 0xFF44   // LCDC Y-Coordinate (R)
#define R_LYC_ADDR 0xFF45  // LY Compare (R/W)
#define R_BGP_ADDR 0xFF47  // BG Palette Data (W)

Mmu * mmu_init();
void mmu_read_rom(struct Mmu * mmu, FILE * rom_file);
void mmu_step(GameBoy * gameboy_ptr);

uint16_t mmu_fetch_16bit_val(uint8_t * memory_map, memory_addr address);
int8_t mmu_fetch_signed_8bit_val(uint8_t * memory_map, memory_addr address);
void mmu_store_16bit_val(uint8_t * memory_map, memory_addr address, uint16_t value);

#endif