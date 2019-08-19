#include <stdlib.h>

#include "mmap.h"
#include "cpu.h"
#include "cpu_instructions.h"

struct Cpu * init_cpu()
{
    struct Cpu * cpu = malloc(sizeof(struct Cpu));

    cpu->regB = 0;
    cpu->regC = 0;
    cpu->regD = 0;
    cpu->regE = 0;
    cpu->regH = 0;
    cpu->regL = 0;
    cpu->regSP = 0;
    cpu->regPC = 0;

    return cpu;
}

/* CPU instructions */

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

/*
    An 8 bit opcode can be broken down in the following way :
    [ 7, 6, 5, 4, 3, 2, 1, 0]
    |  x  |    y   |    z   |
          |  p  | q|
*/
void execute_instruction(uint8_t * mmap, struct Cpu * cpu)
{
    uint8_t opcode = mmap[cpu->regPC++];

    switch (opcode)
    {
        case 0x00:
            NOP;
            break;
        case 0x01:
            LD_rr_nn(cpu->regB, cpu->regC, cpu, mmap);
            cpu->regPC = cpu->regPC + 2;
            break;
        case 0x02:
            LD_addr_r(mmap, REG_PAIR_VAL(cpu->regB, cpu->regC), cpu->regA);
            break;
        case 0x03:
            INC_rr(cpu->regB, cpu->regC);
            break;
        case 0x04:
            INC_r(cpu->regB);
            break;
        case 0x05:
            DEC_r(cpu->regB);
            break;
        case 0x06:
            LD_r_n(cpu->regB, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x0c:
            INC_r(cpu->regC);
            break;
        case 0x11:
            LD_rr_nn(cpu->regD, cpu->regE, cpu, mmap);
            cpu->regPC = cpu->regPC + 2;
            break;
        case 0x0E:
            LD_r_n(cpu->regC, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x16:
            LD_r_n(cpu->regD, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x17:
            RL_r(&(cpu->regA), cpu);
            break;
        case 0x1A:
            LD_r_addr(cpu->regA, mmap, REG_PAIR_VAL(cpu->regD, cpu->regE));
            break;
        case 0x1E:
            LD_r_n(cpu->regE, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x20:
            JR_NZ_sn(cpu, mmap);
            cpu->regPC++;
            break;
        case 0x21:
            LD_rr_nn(cpu->regH, cpu->regL, cpu, mmap);
            cpu->regPC = cpu->regPC + 2;
            break;
        case 0x22:
            LDI_addr_r(mmap, cpu->regH, cpu->regL, cpu->regA);
            break;
        case 0x23:
            INC_rr(cpu->regH, cpu->regL);
            break;
        case 0x26:
            LD_r_n(cpu->regH, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x2E:
            LD_r_n(cpu->regL, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x31:
            LD_SP_nn(cpu, mmap);
            cpu->regPC = cpu->regPC + 2;
            break;
        case 0x32:
            LDD_addr_r(mmap, cpu->regH, cpu->regL, cpu->regA);
            break;
        case 0x3E:
            LD_r_n(cpu->regA, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x4F: // ld c a
            LD_r_r(cpu->regC, cpu->regA);
            break;
        case 0x77:
            LD_addr_r(mmap, REG_PAIR_VAL(cpu->regH, cpu->regL), cpu->regA);
            break;
        case 0xA8 ... 0xAD: 
            XOR_A(cpu->regA, *get_reg_by_num(cpu, opcode & 0xF));
            break;
        case 0xAE:
            XOR_A(cpu->regA, REG_PAIR_VAL(cpu->regH, cpu->regL));
            break;
        case 0xAF:
            XOR_A(cpu->regA, cpu->regA);
            break;
        case 0xC1:
            POP_rr(cpu->regB, cpu->regC, mmap, cpu->regSP);
            break;
        case 0xC5:
            PUSH_rr(cpu->regB, cpu->regC, mmap, cpu->regSP);
            break;
        case 0xC9:
            RET(mmap, cpu);
            break;
        case 0xCD:
            CALL_nn(mmap, cpu->regSP, cpu);
            break;
        case 0xE0:
            LD_addr_r(mmap, 0xFF00 + FETCH_8BIT_VAL(mmap, cpu->regPC), cpu->regA);
            cpu->regPC++;
            break;
        case 0xE2:
            LD_addr_r(mmap, 0xFF00 + cpu->regC, cpu->regA);
            break;
        case 0xF2:
            LD_r_addr(cpu->regA, mmap, 0xFF00 + cpu->regC);
            break;
        case 0xCB:
            opcode = mmap[cpu->regPC++];

            switch(opcode)
            {
                case 0x10 ... 0x15:
                    RL_r(get_reg_by_num(cpu, opcode & 0xF), cpu);
                    break;
                case 0x40 ... 0x45:
                    BIT(0, *get_reg_by_num(cpu, opcode & 0xF), cpu);
                    break;
                case 0x48 ... 0x4D:
                    BIT(1, *get_reg_by_num(cpu, opcode & 0b111), cpu);
                    break;
                case 0x50 ... 0x55:
                    BIT(2, *get_reg_by_num(cpu, opcode & 0xF), cpu);
                    break;
                case 0x58 ... 0x5D:
                    BIT(3, *get_reg_by_num(cpu, opcode & 0b111), cpu);
                    break;
                case 0x60 ... 0x65:
                    BIT(4, *get_reg_by_num(cpu, opcode & 0xF), cpu);
                    break;
                case 0x68 ... 0x6D:
                    BIT(5, *get_reg_by_num(cpu, opcode & 0b111), cpu);
                    break;
                case 0x70 ... 0x75:
                    BIT(6, *get_reg_by_num(cpu, opcode & 0xF), cpu);
                    break;
                case 0x78 ... 0x7D:
                    BIT(7, *get_reg_by_num(cpu, opcode & 0b111), cpu);
                    break;
                default:
                    printf("%i", 0/0);
                    break;
            }
            break;

        default:
            printf("%i", 0/0);
            break;
    }

    debug_cpu(mmap, cpu);
}

uint8_t * get_reg_by_num(struct Cpu * cpu, uint8_t reg_num)
{
    if (reg_num == 0)
    {
        return &(cpu->regB);
    }
    else if (reg_num == 1)
    {
        return &(cpu->regC);
    }
    else if (reg_num == 2)
    {
        return &(cpu->regD);
    }
    else if (reg_num == 3)
    {
        return &(cpu->regE);
    }
    else if (reg_num == 4)
    {
        return &(cpu->regH);
    }
    else if (reg_num == 5)
    {
        return &(cpu->regL);
    }
    else if (reg_num == 6)
    {
        return &(cpu->regA);
    }
}

void debug_cpu(uint8_t * mmap, struct Cpu * cpu)
{
    printf("CPU state : \n \n");
    // register values
    printf("    reg B = 0x%x \n", cpu->regB);
    printf("    reg C = 0x%x \n", cpu->regC);
    printf("    reg D = 0x%x \n", cpu->regD);
    printf("    reg E = 0x%x \n", cpu->regE);
    printf("    reg H = 0x%x \n", cpu->regH);
    printf("    reg L = 0x%x \n", cpu->regL);
    printf("    reg A = 0x%x \n \n", cpu->regA);
    printf("    FLAG Z = 0x%x \n", (cpu->FLAG & 0b10000000) >> 7);
    printf("    FLAG N = 0x%x \n", (cpu->FLAG & 0b01000000) >> 6);
    printf("    FLAG H = 0x%x \n", (cpu->FLAG & 0b00100000) >> 5);
    printf("    FLAG C = 0x%x \n \n", (cpu->FLAG & 0b00010000) >> 4);
    printf("    sp = 0x%x \n", cpu->regSP);
    printf("    pc = 0x%x \n", cpu->regPC);
    // current instruction opcode
    printf("    pc instruction: 0x%x \n", mmap[cpu->regPC]);

    printf("\n");
}