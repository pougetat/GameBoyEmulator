#include "mmap.h"
#include "cpu.h"

// NOP
#define NOP ;

// register_dest <- immediate n
#define LD_r_n(reg, cpu_ptr, mmap_ptr) \
    reg = FETCH_8BIT_VAL(mmap_ptr, cpu_ptr->regPC); \

// register_dest <- register_src
#define LD_r_r(reg_dest, reg_src, cpu_ptr) \
    reg_dest = reg_src; \

// register pair <- immediate nn
#define LD_rr_nn(reg_high, reg_low, cpu_ptr, mmap_ptr) \
    reg_high = FETCH_16BIT_VAL(mmap_ptr, cpu_ptr->regPC) >> 8; \
    reg_low = FETCH_16BIT_VAL(mmap_ptr, cpu_ptr->regPC) & 0xFF;

// regSP <- immediate nn
#define LD_SP_nn(cpu_ptr, mmap_ptr) \
    cpu_ptr->regSP = FETCH_16BIT_VAL(mmap_ptr, cpu_ptr->regPC);

// (address) <- register
#define LD_addr_r(mmap_ptr, address, reg) \
    STORE_8BIT_VAL(mmap_ptr, address, reg)

// (address) <- register; increment register pair
#define LDI_addr_r(mmap_ptr, reg_high, reg_low, src_reg) \
    LD_addr_r(mmap_ptr, REG_PAIR_VAL(reg_high, reg_low), src_reg) \
    INC_rr(reg_high, reg_low)

// (address) <- register; decrement register pair
#define LDD_addr_r(mmap_ptr, reg_high, reg_low, src_reg) \
    LD_addr_r(mmap_ptr, REG_PAIR_VAL(reg_high, reg_low), src_reg) \
    DEC_rr(reg_high, reg_low)

// increment register pair
#define INC_rr(reg_high, reg_low) \
    if (reg_low == 0xFF) \
    { \
        INC_r(reg_high) \
        reg_low = 0; \
    } \
    else \
    { \
        INC_r(reg_low) \
    }

// increment register
#define INC_r(reg) \
    reg++;

// decrement register pair
#define DEC_rr(reg_high, reg_low) \
    if (reg_low == 0) \
    { \
        DEC_r(reg_high) \
        reg_low = 0xFF; \
    } \
    else \
    { \
        DEC_r(reg_low) \
    }

#define DEC_r(reg) \
    reg--;

#define XOR_A(regA, reg) \
    regA = regA ^ reg;

#define BIT(bit_num, reg, flag_reg) \
    flag_reg |= TEST_BIT(bit_num, reg) << 7

#define TEST_BIT(bit_num, reg) \
    ((reg & (0x1 << bit_num)) >> bit_num)