#include "mmap.h"
#include "cpu.h"

#define NOP ;
#define EX_AF_AF ;
#define DJNZ_d() ;
#define JR_d() ;
#define JR_cc_d();

#define LD_r_n(reg, cpu_ptr, mmap_ptr) \
    cpu_ptr->registers[reg] = FETCH_8BIT_VAL(mmap_ptr, cpu_ptr->pc); \
    cpu_ptr->pc++;

#define LD_r_nn(reg, cpu_ptr, mmap_ptr) \
    uint16_t value = FETCH_16BIT_VAL(mmap_ptr, cpu_ptr->pc); \
    cpu_ptr->pc = cpu_ptr->pc + 2; \
    if (reg == 3) { cpu_ptr->sp = value; } \
    else { cpu->registers[2*reg] = value >> 8; cpu->registers[2*reg+1] = value & 0xFF; }

#define ALU_r(operation, reg, cpu_ptr) \
    switch(operation) { \
        case 5: \
            XOR_A(reg, cpu_ptr) \
            break; \
        default: \
            break; \
    }

#define XOR_A(reg, cpu_ptr) \
        cpu_ptr->registers[A] = cpu_ptr->registers[A] ^ cpu_ptr->registers[reg]; 
