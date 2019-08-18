#include "mmap.h"
#include "cpu.h"

// NOP
#define NOP ;

// register_dest <- immediate n
#define LD_r_n(reg, cpu_ptr, mmap_ptr) \
    reg = FETCH_8BIT_VAL(mmap_ptr, cpu_ptr->regPC); \

// register_dest <- register_src
#define LD_r_r(reg_dest, reg_src) \
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

// register <- (address)
#define LD_r_addr(reg, mmap_ptr, address) \
    reg = mmap_ptr[address]

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
#define INC_r(reg, cpu_ptr) \
    if (CARRY) \
    { \
        SET_H_FLAG(cpu_ptr, 1); \
    } \
    reg++; \
    if (reg == 0) \
    { \
        SET_Z_FLAG(cpu_ptr, 1); \
    } \
    SET_N_FLAG(cpu_ptr, 0);

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

// decrement register
#define DEC_r(reg) \
    reg--;

#define JR_NZ_sn(cpu_ptr, mmap_ptr) \
    if (TEST_BIT_IS_0(cpu->FLAG, 7)) \
    { \
        cpu->regPC += FETCH_SIGNED_8BIT_VAL(mmap_ptr, cpu->regPC); \
    }

// xor
#define XOR_A(regA, reg) \
    regA = regA ^ reg;

// rotate register left
#define RL_r(reg_ptr, cpu_ptr) \
    if (*reg_ptr << 1 == 0) \
    { \
        SET_Z_FLAG(cpu_ptr, 1); \
    } \
    SET_N_FLAG(cpu_ptr, 0); \
    SET_H_FLAG(cpu_ptr, 0); \
    SET_C_FLAG(cpu_ptr, *reg_ptr >> 7 & 0x1); \
    *reg_ptr = *reg_ptr << 1; \

// Z <- TEST_BIT_IS_0(bit_num, register)
// N <- reset
// H <- set
#define BIT(bit_num, reg, cpu_ptr) \
    SET_Z_FLAG(cpu_ptr, TEST_BIT_IS_0(reg, bit_num)); \
    SET_N_FLAG(cpu_ptr, 0); \
    SET_H_FLAG(cpu_ptr, 1);

#define TEST_BIT_IS_0(reg, bit_num) \
    ((reg >> bit_num) ^ 0x1)

#define CALL_nn(mmap_ptr, reg_sp, cpu_ptr) \
    DEC_r(reg_sp); \
    STORE_16BIT_VAL(mmap_ptr, reg_sp, cpu_ptr->regPC); \
    DEC_r(reg_sp); \
    cpu->regPC = FETCH_16BIT_VAL(mmap_ptr, cpu_ptr->regPC);

#define RET(mmap_ptr, cpu_ptr) \
    INC_r(cpu_ptr->regSP); \
    cpu_ptr->regPC = FETCH_16BIT_VAL(mmap_ptr, cpu_ptr->regPC); \
    INC_r(cpu_ptr->regSP);

#define PUSH_rr(reg_high, reg_low, mmap_ptr, reg_sp) \
    DEC_r(reg_sp); \
    STORE_16BIT_VAL(mmap_ptr, reg_sp, REG_PAIR_VAL(reg_high, reg_low)); \
    DEC_r(reg_sp);

#define POP_rr(reg_high, reg_low, mmap_ptr, reg_sp) \
    INC_r(reg_sp); \
    reg_high = FETCH_16BIT_VAL(mmap_ptr, reg_sp) >> 8; \
    reg_low = FETCH_16BIT_VAL(mmap_ptr, reg_sp) & 0xFF; \
    INC_r(reg_sp);