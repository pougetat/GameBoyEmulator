#ifndef CPU_H
#define CPU_H

#include <stdint.h>

struct Cpu {
    uint8_t registers[7]; // B, C, D, E, H, L, A
    uint16_t sp; // stack pointer
    uint16_t pc; // program counter
    uint16_t flag; // flag register
};

struct Cpu init_cpu();

void execute_instruction(uint8_t * mmap, struct Cpu * cpu);

#endif