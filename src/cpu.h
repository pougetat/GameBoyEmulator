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

#define SET_Z_FLAG(cpu_ptr, value) \
    SET_FLAG_VALUE(cpu_ptr, 7, value);

#define SET_N_FLAG(cpu_ptr, value) \
    SET_FLAG_VALUE(cpu_ptr, 6, value);

#define SET_H_FLAG(cpu_ptr, value) \
    SET_FLAG_VALUE(cpu_ptr, 5, value);

#define SET_C_FLAG(cpu_ptr, value) \
    SET_FLAG_VALUE(cpu_ptr, 4, value);

#define SET_FLAG_VALUE(cpu_ptr, bitnum, value) \
    if (value == 1) \
    { \
        cpu_ptr->FLAG |= 0x1 << bitnum; \
    } \
    else \
    { \
        cpu_ptr->FLAG &= ~(0x1 << bitnum); \
    } \

#define WILL_CARRY_3_TO_4(reg) \
    ((reg & 0b100) >> 2 == 1)

#define WILL_BORROW_FROM_4(reg) \
    (reg & 0b1111 == 0b1000)

struct Cpu * init_cpu();

void execute_instruction(uint8_t * memory_map, struct Cpu * cpu);
uint8_t * get_reg_by_num(struct Cpu * cpu, uint8_t reg_num);
void debug_cpu(uint8_t * memory_map, struct Cpu * cpu);

#endif