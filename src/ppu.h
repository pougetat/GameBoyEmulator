#include <stdint.h>

#include "mmu.h"

struct PPU {
    uint32_t * ppu_clock;
}

void ppu_step(struct PPU * ppu, uint8_t * memory_map);