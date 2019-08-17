#ifndef CPU_H
#define CPU_H

#include <stdint.h>

struct Cpu {
    uint8_t regB;
    uint8_t regC;
    uint8_t regD;
    uint8_t regE;
    uint8_t regH;
    uint8_t regL;
    uint8_t regA;
    uint16_t regSP;
    uint16_t regPC;
    uint16_t FLAG;
};

#define REG_PAIR_VAL(reg_high, reg_low) \
    (((uint16_t) reg_high << 8) | reg_low)

struct Cpu * init_cpu();

void execute_instruction(uint8_t * mmap, struct Cpu * cpu);
uint8_t get_reg_by_num(struct Cpu * cpu, uint8_t reg_num);
void debug_cpu(uint8_t * mmap, struct Cpu * cpu);

#endif