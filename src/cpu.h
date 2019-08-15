#include <stdint.h>

struct Cpu {
    uint8_t registers[7]; // B, C, D, E, H, L, A
    uint16_t sp_reg; // stack pointer
    uint16_t pc_reg; // program counter
    uint16_t flag_reg; // flag register
};

struct Cpu init_cpu();

void execute_instruction(uint8_t * mmap, struct Cpu cpu);