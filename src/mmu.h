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

#define LCDC_R_ADDR 0xFF40 // LCD contol (R/W)
#define STAT_R_ADDR 0xFF41 // LCDC status (R/W)
#define SCY_R_ADDR 0xFF42  // Scroll Y (R/W)
#define SCX_R_ADDR 0xFF43  // Scroll X (R/W)
#define LY_R_ADDR 0xFF44   // LCDC Y-Coordinate (R)
#define LYC_R_ADDR 0xFF45  // LY Compare (R/W)
#define BGP_R_ADDR 0xFF47  // BG Palette Data (W)

Mmu * mmu_init();
void mmu_step(GameBoy * gameboy_ptr);

uint16_t fetch_16bit_val(uint8_t * memory_map, memory_addr address);
uint8_t fetch_8bit_val(uint8_t * memory_map, memory_addr address);
int8_t fetch_signed_8bit_val(uint8_t * memory_map, memory_addr address);
void store_16bit_val(uint8_t * memory_map, memory_addr address, uint16_t value);
void store_8bit_val(uint8_t * memory_map, memory_addr address, uint8_t value);

#endif