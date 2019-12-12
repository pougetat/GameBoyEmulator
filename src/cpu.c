#include <stdlib.h>

#include "memory_map.h"
#include "cpu.h"

struct Cpu * init_cpu()
{
    struct Cpu * cpu_ptr = malloc(sizeof(struct Cpu));

    cpu_ptr->regB = 0;
    cpu_ptr->regC = 0;
    cpu_ptr->regD = 0;
    cpu_ptr->regE = 0;
    cpu_ptr->regH = 0;
    cpu_ptr->regL = 0;
    cpu_ptr->regSP = 0;
    cpu_ptr->regPC = 0;

    return cpu_ptr;
}

/* CPU instructions */

// NOP
void nop(){}

// register_dest <- immediate n
void ld_r_n(uint8_t * reg_ptr, struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    *reg_ptr = fetch_8bit_val(memory_map, cpu_ptr->regPC);
}

// register_dest <- register_src
void ld_r_r(uint8_t * reg_dest_ptr, uint8_t reg_src)
{
    *reg_dest_ptr = reg_src;
}

// register pair <- immediate nn
void ld_rr_nn(uint8_t * reg_high_ptr, uint8_t * reg_low_ptr, struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    *reg_high_ptr = fetch_16bit_val(memory_map, cpu_ptr->regPC) >> 8; \
    *reg_low_ptr = fetch_16bit_val(memory_map, cpu_ptr->regPC) & 0xFF;
}

// regSP <- immediate nn
void ld_sp_nn(struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    cpu_ptr->regSP = fetch_16bit_val(memory_map, cpu_ptr->regPC);
}

// (address) <- register
void ld_addr_r(memory_addr address, uint8_t reg, uint8_t * memory_map)
{
    memory_map[address] = 0x9;
    store_8bit_val(memory_map, address, reg);
}

// register <- (address)
void ld_r_addr(uint8_t * reg_ptr, memory_addr address, uint8_t * memory_map)
{
    *reg_ptr = memory_map[address];
}

// (address) <- register; increment register pair
#define LDI_addr_r(reg_high, reg_low, src_reg) \
    ld_addr_r(REG_PAIR_VAL(reg_high, reg_low), src_reg, memory_map); \
    INC_rr(reg_high, reg_low);

// (address) <- register; decrement register pair
#define LDD_addr_r(reg_high, reg_low, src_reg) \
    ld_addr_r(REG_PAIR_VAL(reg_high, reg_low), src_reg, memory_map); \
    DEC_rr(reg_high, reg_low);

// increment register pair
#define INC_rr(reg_high, reg_low) \
    if (reg_low == 0xFF) \
    { \
        reg_high++; \
        reg_low = 0; \
    } \
    else \
    { \
        reg_low++; \
    }

// increment register
#define INC_r(reg) \
    SET_H_FLAG(cpu_ptr, WILL_CARRY_3_TO_4(reg)); \
    reg++; \
    SET_Z_FLAG(cpu_ptr, (reg == 0)); \
    SET_N_FLAG(cpu_ptr, 0);

// decrement register pair
#define DEC_rr(reg_high, reg_low) \
    if (reg_low == 0) \
    { \
        reg_high--; \
        reg_low = 0xFF; \
    } \
    else \
    { \
        reg_low--; \
    }

// decrement register
#define DEC_r(reg) \
    SET_H_FLAG(cpu_ptr, !WILL_BORROW_FROM_4(reg)); \
    reg--; \
    SET_Z_FLAG(cpu_ptr, (reg == 0)); \
    SET_N_FLAG(cpu_ptr, 1);

// jump if Z flag not set
#define JR_NZ_sn \
    if (TEST_BIT_IS_0(cpu_ptr->FLAG, 7)) \
    { \
        cpu_ptr->regPC += fetch_signed_8bit_val(memory_map, cpu_ptr->regPC); \
    }

// jump if Z flag set
#define JR_Z_sn \
    if (!TEST_BIT_IS_0(cpu_ptr->FLAG, 7)) \
    { \
        cpu_ptr->regPC += fetch_signed_8bit_val(memory_map, cpu_ptr->regPC); \
    }

// jump
#define JR_sn \
    cpu_ptr->regPC += fetch_signed_8bit_val(memory_map, cpu_ptr->regPC);

// xor reg with register A
#define XOR_A(regA, reg) \
    regA = regA ^ reg; \
    SET_Z_FLAG(cpu_ptr, (regA == 0)); \
    SET_N_FLAG(cpu_ptr, 0); \
    SET_H_FLAG(cpu_ptr, 0); \
    SET_C_FLAG(cpu_ptr, 0); \

// compare register A with value
#define CP_A(value) \
    SET_Z_FLAG(cpu_ptr, (cpu_ptr->regA == value)); \
    SET_N_FLAG(cpu_ptr, 1); \
    SET_H_FLAG(cpu_ptr, (int) (cpu_ptr->regA & 0xf) < (int) (value & 0xf)); \
    SET_C_FLAG(cpu_ptr, (int) cpu_ptr->regA < (int) value);

// rotate register left
#define RL_r(reg_ptr) \
    SET_Z_FLAG(cpu_ptr, (*reg_ptr << 1 == 0)); \
    SET_N_FLAG(cpu_ptr, 0); \
    SET_H_FLAG(cpu_ptr, 0); \
    SET_C_FLAG(cpu_ptr, *reg_ptr >> 7 & 0x1); \
    *reg_ptr = *reg_ptr << 1; \

// Z <- TEST_BIT_IS_0(bit_num, register)
// N <- reset
// H <- set
#define BIT(bit_num, reg) \
    SET_Z_FLAG(cpu_ptr, TEST_BIT_IS_0(reg, bit_num)); \
    SET_N_FLAG(cpu_ptr, 0); \
    SET_H_FLAG(cpu_ptr, 1);

#define TEST_BIT_IS_0(reg, bit_num) \
    ((reg >> bit_num) ^ 0x1)

#define CALL_nn \
    cpu_ptr->regSP--; \
    store_16bit_val(memory_map, cpu_ptr->regSP, cpu_ptr->regPC+2); \
    cpu_ptr->regSP--; \
    cpu_ptr->regPC = fetch_16bit_val(memory_map, cpu_ptr->regPC);

#define RET \
    cpu_ptr->regSP++; \
    cpu_ptr->regPC = fetch_16bit_val(memory_map, cpu_ptr->regSP); \
    cpu_ptr->regSP++;

#define PUSH_rr(reg_high, reg_low) \
    cpu_ptr->regSP--; \
    store_16bit_val(memory_map, cpu_ptr->regSP, REG_PAIR_VAL(reg_high, reg_low)); \
    cpu_ptr->regSP--;

#define POP_rr(reg_high, reg_low) \
    cpu_ptr->regSP++; \
    reg_high = fetch_16bit_val(memory_map, cpu_ptr->regSP) >> 8; \
    reg_low = fetch_16bit_val(memory_map, cpu_ptr->regSP) & 0xFF; \
    cpu_ptr->regSP++;

/*
    An 8 bit opcode can be broken down in the following way :
    [ 7, 6, 5, 4, 3, 2, 1, 0]
    |  x  |    y   |    z   |
          |  p  | q|
*/
void execute_instruction(uint8_t * memory_map, struct Cpu * cpu_ptr)
{
    uint8_t opcode = memory_map[cpu_ptr->regPC++];

    switch (opcode)
    {
        case 0x00:
            nop();
            break;
        case 0x01:
            ld_rr_nn(&(cpu_ptr->regB), &(cpu_ptr->regC), cpu_ptr, memory_map);
            cpu_ptr->regPC = cpu_ptr->regPC + 2;
            break;
        case 0x02:
            ld_addr_r(REG_PAIR_VAL(cpu_ptr->regB, cpu_ptr->regC), cpu_ptr->regA, memory_map);
            break;
        case 0x03:
            INC_rr(cpu_ptr->regB, cpu_ptr->regC);
            break;
        case 0x04:
            INC_r(cpu_ptr->regB);
            break;
        case 0x05:
            DEC_r(cpu_ptr->regB);
            break;
        case 0x06:
            ld_r_n(&(cpu_ptr->regB), cpu_ptr, memory_map);
            cpu_ptr->regPC++;
            break;
        case 0x0c:
            INC_r(cpu_ptr->regC);
            break;
        case 0x0D:
            DEC_r(cpu_ptr->regC);
            break;
        case 0x0E:
            ld_r_n(&(cpu_ptr->regC), cpu_ptr, memory_map);            
            cpu_ptr->regPC++;
            break;
        case 0x11:
            ld_rr_nn(&(cpu_ptr->regD), &(cpu_ptr->regE), cpu_ptr, memory_map);
            cpu_ptr->regPC = cpu_ptr->regPC + 2;
            break;
        case 0x13:
            INC_rr(cpu_ptr->regD, cpu_ptr->regE);
            break;
        case 0x16:
            ld_r_n(&(cpu_ptr->regD), cpu_ptr, memory_map);                        
            cpu_ptr->regPC++;
            break;
        case 0x17:
            RL_r(&(cpu_ptr->regA));
            break;
        case 0x18:
            JR_sn;
            cpu_ptr->regPC++;
            break;
        case 0x1A:
            ld_r_addr(&(cpu_ptr->regA), REG_PAIR_VAL(cpu_ptr->regD, cpu_ptr->regE), memory_map);
            break;
        case 0x1E:
            ld_r_n(&(cpu_ptr->regE), cpu_ptr, memory_map);            
            cpu_ptr->regPC++;
            break;
        case 0x20:
            JR_NZ_sn;
            cpu_ptr->regPC++;
            break;
        case 0x21:
            ld_rr_nn(&(cpu_ptr->regH), &(cpu_ptr->regL), cpu_ptr, memory_map);
            cpu_ptr->regPC = cpu_ptr->regPC + 2;
            break;
        case 0x22:
            LDI_addr_r(cpu_ptr->regH, cpu_ptr->regL, cpu_ptr->regA);
            break;
        case 0x23:
            INC_rr(cpu_ptr->regH, cpu_ptr->regL);
            break;
        case 0x26:
            ld_r_n(&(cpu_ptr->regH), cpu_ptr, memory_map);            
            cpu_ptr->regPC++;
            break;
        case 0x28:
            JR_Z_sn;
            cpu_ptr->regPC++;
            break;
        case 0x2E:
            ld_r_n(&(cpu_ptr->regL), cpu_ptr, memory_map);                        
            cpu_ptr->regPC++;
            break;
        case 0x31:
            ld_sp_nn(cpu_ptr, memory_map);
            cpu_ptr->regPC = cpu_ptr->regPC + 2;
            break;
        case 0x32:
            LDD_addr_r(cpu_ptr->regH, cpu_ptr->regL, cpu_ptr->regA);
            break;
        case 0x3D:
            DEC_r(cpu_ptr->regA);
            break;
        case 0x3E:
            ld_r_n(&(cpu_ptr->regA), cpu_ptr, memory_map);                        
            cpu_ptr->regPC++;
            break;
        case 0x4F: // ld c a
            ld_r_r(&(cpu_ptr->regC), cpu_ptr->regA);
            break;
        case 0x57:
            ld_r_r(&(cpu_ptr->regD), cpu_ptr->regA);
            break;
        case 0x67:
            ld_r_r(&(cpu_ptr->regH), cpu_ptr->regA);
            break;
        case 0x77:
            ld_addr_r(REG_PAIR_VAL(cpu_ptr->regH, cpu_ptr->regL), cpu_ptr->regA, memory_map);
            break;
        case 0x78 ... 0x7D:
            ld_r_r(&(cpu_ptr->regA), *get_reg_by_num(cpu_ptr, opcode & 0b111));
            break;
        case 0xA8 ... 0xAD: 
            XOR_A(cpu_ptr->regA, *get_reg_by_num(cpu_ptr, opcode & 0xF));
            break;
        case 0xAE:
            XOR_A(cpu_ptr->regA, REG_PAIR_VAL(cpu_ptr->regH, cpu_ptr->regL));
            break;
        case 0xAF:
            XOR_A(cpu_ptr->regA, cpu_ptr->regA);
            break;
        case 0xC1:
            POP_rr(cpu_ptr->regB, cpu_ptr->regC);
            break;
        case 0xC5:
            PUSH_rr(cpu_ptr->regB, cpu_ptr->regC);
            break;
        case 0xC9:
            RET;
            break;
        case 0xCD:
            CALL_nn(cpu_ptr->regSP);
            break;
        case 0xE0:
            ld_addr_r(0xFF00 + ((uint16_t) fetch_8bit_val(memory_map, cpu_ptr->regPC)), cpu_ptr->regA, memory_map);
            cpu_ptr->regPC++;
            break;
        case 0xE2:
            ld_addr_r(0xFF00 + ((uint16_t) cpu_ptr->regC), cpu_ptr->regA, memory_map);
            break;
        case 0xEA:
            ld_addr_r(fetch_16bit_val(memory_map, cpu_ptr->regPC), cpu_ptr->regA, memory_map);
            cpu_ptr->regPC++;
            cpu_ptr->regPC++;
            break;
        case 0xF0:
            ld_r_addr(&(cpu_ptr->regA), 0xFF00 + ((uint16_t) fetch_8bit_val(memory_map, cpu_ptr->regPC)), memory_map);
            cpu_ptr->regPC++;
            break;
        case 0xF2:
            ld_r_addr(&(cpu_ptr->regA), 0xFF00 + ((uint16_t) cpu_ptr->regC), memory_map);
            break;
        case 0xFE:
            CP_A(fetch_8bit_val(memory_map, cpu_ptr->regPC));
            cpu_ptr->regPC++;
            break;
        case 0xCB:
            opcode = memory_map[cpu_ptr->regPC++];

            switch(opcode)
            {
                case 0x10 ... 0x15:
                    RL_r(get_reg_by_num(cpu_ptr, opcode & 0xF));
                    break;
                case 0x40 ... 0x45:
                    BIT(0, *get_reg_by_num(cpu_ptr, opcode & 0xF));
                    break;
                case 0x48 ... 0x4D:
                    BIT(1, *get_reg_by_num(cpu_ptr, opcode & 0b111));
                    break;
                case 0x50 ... 0x55:
                    BIT(2, *get_reg_by_num(cpu_ptr, opcode & 0xF));
                    break;
                case 0x58 ... 0x5D:
                    BIT(3, *get_reg_by_num(cpu_ptr, opcode & 0b111));
                    break;
                case 0x60 ... 0x65:
                    BIT(4, *get_reg_by_num(cpu_ptr, opcode & 0xF));
                    break;
                case 0x68 ... 0x6D:
                    BIT(5, *get_reg_by_num(cpu_ptr, opcode & 0b111));
                    break;
                case 0x70 ... 0x75:
                    BIT(6, *get_reg_by_num(cpu_ptr, opcode & 0xF));
                    break;
                case 0x78 ... 0x7D:
                    BIT(7, *get_reg_by_num(cpu_ptr, opcode & 0b111));
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

    debug_cpu(memory_map, cpu_ptr);
}

uint8_t * get_reg_by_num(struct Cpu * cpu_ptr, uint8_t reg_num)
{
    if (reg_num == 0)
    {
        return &(cpu_ptr->regB);
    }
    else if (reg_num == 1)
    {
        return &(cpu_ptr->regC);
    }
    else if (reg_num == 2)
    {
        return &(cpu_ptr->regD);
    }
    else if (reg_num == 3)
    {
        return &(cpu_ptr->regE);
    }
    else if (reg_num == 4)
    {
        return &(cpu_ptr->regH);
    }
    else if (reg_num == 5)
    {
        return &(cpu_ptr->regL);
    }
    else if (reg_num == 6)
    {
        return &(cpu_ptr->regA);
    }
}

void debug_cpu(uint8_t * memory_map, struct Cpu * cpu_ptr)
{
    printf("CPU state : \n \n");
    // register values
    printf("    reg B = 0x%x \n", cpu_ptr->regB);
    printf("    reg C = 0x%x \n", cpu_ptr->regC);
    printf("    reg D = 0x%x \n", cpu_ptr->regD);
    printf("    reg E = 0x%x \n", cpu_ptr->regE);
    printf("    reg H = 0x%x \n", cpu_ptr->regH);
    printf("    reg L = 0x%x \n", cpu_ptr->regL);
    printf("    reg A = 0x%x \n \n", cpu_ptr->regA);
    printf("    FLAG Z = 0x%x \n", (cpu_ptr->FLAG & 0b10000000) >> 7);
    printf("    FLAG N = 0x%x \n", (cpu_ptr->FLAG & 0b01000000) >> 6);
    printf("    FLAG H = 0x%x \n", (cpu_ptr->FLAG & 0b00100000) >> 5);
    printf("    FLAG C = 0x%x \n \n", (cpu_ptr->FLAG & 0b00010000) >> 4);
    printf("    sp = 0x%x \n", cpu_ptr->regSP);
    printf("    pc = 0x%x \n", cpu_ptr->regPC);
    // current instruction opcode
    printf("    pc instruction: 0x%x \n", memory_map[cpu_ptr->regPC]);

    printf("\n");
}