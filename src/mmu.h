#ifndef MMU_H
#define MMU_H

#include <stdio.h>
#include <stdint.h>

#define MAX_ROM_SIZE 32768 // (only handling 32 KB roms for now)
#define MEMORY_MAP_SIZE 65536 // 0x0000 - 0xFFFF

typedef struct GameBoy GameBoy;
typedef uint16_t memory_addr;

typedef struct Mmu {
    uint8_t * memory_map;
} Mmu;

// memory mapped register address

    // LCD related registers

#define R_LCDC_ADDR 0xFF40 // LCD contol (R/W)
#define R_STAT_ADDR 0xFF41 // LCD status (R/W)
#define R_SCY_ADDR 0xFF42  // Scroll Y (R/W)
#define R_SCX_ADDR 0xFF43  // Scroll X (R/W)
#define R_LY_ADDR 0xFF44   // LCDC Y-Coordinate (R)
#define R_LYC_ADDR 0xFF45  // LY Compare (R/W)
#define R_BGP_ADDR 0xFF47  // BG Palette Data (W)
#define R_BOOT_ROM_CONTROL 0xFF50 // Boot rom control

Mmu * mmu_init();
void mmu_read_rom(uint8_t * memory_map, FILE * rom_file, memory_addr start, memory_addr end);

uint16_t mmu_fetch_16bit_val(uint8_t * memory_map, memory_addr addr);
int8_t mmu_fetch_signed_8bit_val(uint8_t * memory_map, memory_addr addr);
void mmu_store_16bit_val(uint8_t * memory_map, memory_addr addr, uint16_t value);
void mmu_store_8bit_val(uint8_t * memory_map, memory_addr addr, uint8_t value);

#endif