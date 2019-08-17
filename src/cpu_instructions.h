#include "mmap.h"
#include "cpu.h"

#define LD_r_n(reg, cpu_ptr, mmap_ptr) \
    reg = FETCH_8BIT_VAL(mmap_ptr, cpu_ptr->regPC); \
    cpu_ptr->regPC++;

#define LD_r_nn(reg_high, reg_low, cpu_ptr, mmap_ptr) \
    reg_high = FETCH_16BIT_VAL(mmap_ptr, cpu_ptr->regPC) >> 8; \
    reg_low = FETCH_16BIT_VAL(mmap_ptr, cpu_ptr->regPC) & 0xFF; \
    cpu_ptr->regPC = cpu_ptr->regPC + 2;

#define LD_SP_nn(cpu_ptr, mmap_ptr) \
    cpu_ptr->regSP = FETCH_16BIT_VAL(mmap_ptr, cpu_ptr->regPC); \
    cpu_ptr->regPC = cpu_ptr->regPC + 2;

#define ALU_r(operation, reg, cpu_ptr) \
    switch(operation) { \
        case 5: \
            XOR_A(reg, cpu_ptr) \
            break; \
        default: \
            break; \
    }

#define XOR_A(reg, cpu_ptr) \
        cpu_ptr->regA = cpu_ptr->regA ^ reg;
