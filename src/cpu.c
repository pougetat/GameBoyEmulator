#include <stdlib.h>
#include <stdbool.h>

#include "cpu.h"
#include "mmu.h"

struct Cpu * cpu_init()
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

// CPU helper functions

uint8_t read_c_flag(Cpu * cpu_ptr)
{
    return ((cpu_ptr->FLAG >> 4) & 0b1);
}

bool will_carry_3_to_4(uint8_t value1, uint8_t value2)
{
    return (value1 & 0b1111) + (value2 & 0b1111) >= 0b10000;
}

bool will_carry_7_to_8(uint8_t value1, uint8_t value2)
{
    return (uint16_t) ((uint16_t) value1 + (uint16_t) value2) >= 0b100000000;
}

/* CPU instructions */

// NOP
void nop(){}

// increment register
void inc_r(uint8_t * reg_ptr, struct Cpu * cpu_ptr)
{
    SET_H_FLAG(cpu_ptr, WILL_CARRY_3_TO_4(*reg_ptr));
    (*reg_ptr)++;
    SET_Z_FLAG(cpu_ptr, (*reg_ptr == 0));
    SET_N_FLAG(cpu_ptr, 0);
}

// increment register pair
void inc_rr(uint8_t * reg_high_ptr, uint8_t * reg_low_ptr)
{
    if (*reg_low_ptr == 0xFF)
    {
        (*reg_high_ptr)++;
        *reg_low_ptr = 0;
    }
    else
    {
        (*reg_low_ptr)++;
    }
}

// decrement register
void dec_r(uint8_t * reg_ptr, struct Cpu * cpu_ptr)
{
    SET_H_FLAG(cpu_ptr, !WILL_BORROW_FROM_4(*reg_ptr));
    (*reg_ptr)--;
    SET_Z_FLAG(cpu_ptr, (*reg_ptr == 0));
    SET_N_FLAG(cpu_ptr, 1);
}

// decrement register pair
void dec_rr(uint8_t * reg_high_ptr, uint8_t * reg_low_ptr)
{
    if (*reg_low_ptr == 0)
    {
        (*reg_high_ptr)--;
        *reg_low_ptr = 0xFF;
    }
    else
    {
        (*reg_low_ptr)--;
    }
}

// add reg to register A
void add_a(uint8_t value, Cpu * cpu_ptr)
{
    SET_H_FLAG(cpu_ptr, will_carry_3_to_4(cpu_ptr->regA, value))
    SET_C_FLAG(cpu_ptr, will_carry_7_to_8(cpu_ptr->regA, value))
    cpu_ptr->regA += value;
    SET_Z_FLAG(cpu_ptr, (cpu_ptr->regA == 0));
    SET_N_FLAG(cpu_ptr, 0);
}

// subtract reg from register A
void sub_a(uint8_t reg, Cpu * cpu_ptr)
{
    SET_H_FLAG(cpu_ptr, !WILL_BORROW_FROM_4(cpu_ptr->regA));
    SET_C_FLAG(cpu_ptr, cpu_ptr->regA < reg);
    cpu_ptr->regA -= reg;
    SET_Z_FLAG(cpu_ptr, (cpu_ptr->regA == 0));
    SET_N_FLAG(cpu_ptr, 1);
}

// register_dest <- immediate n
void ld_r_n(uint8_t * reg_ptr, struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    *reg_ptr = memory_map[cpu_ptr->regPC];
}

// register_dest <- register_src
void ld_r_r(uint8_t * reg_dest_ptr, uint8_t reg_src)
{
    *reg_dest_ptr = reg_src;
}

// register pair <- immediate nn
void ld_rr_nn(uint8_t * reg_high_ptr, uint8_t * reg_low_ptr, struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    *reg_high_ptr = mmu_fetch_16bit_val(memory_map, cpu_ptr->regPC) >> 8; \
    *reg_low_ptr = mmu_fetch_16bit_val(memory_map, cpu_ptr->regPC) & 0xFF;
}

// regSP <- immediate nn
void ld_sp_nn(struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    cpu_ptr->regSP = mmu_fetch_16bit_val(memory_map, cpu_ptr->regPC);
}

// (address) <- register
void ld_addr_r(memory_addr address, uint8_t reg, uint8_t * memory_map)
{
    memory_map[address] = reg;
}

// register <- (address)
void ld_r_addr(uint8_t * reg_ptr, memory_addr address, uint8_t * memory_map)
{
    *reg_ptr = memory_map[address];
}

// register <- (address); increment register pair
void ldi_r_addr(uint8_t * reg_dest_ptr, uint8_t * reg_high_ptr, uint8_t * reg_low_ptr, uint8_t * memory_map)
{
    ld_r_addr(reg_dest_ptr, REG_PAIR_VAL(*reg_high_ptr, *reg_low_ptr), memory_map);
    inc_rr(reg_high_ptr, reg_low_ptr);
}

// (address) <- register; increment register pair
void ldi_addr_r(uint8_t * reg_high_ptr, uint8_t * reg_low_ptr, uint8_t reg_src, uint8_t * memory_map)
{
    ld_addr_r(REG_PAIR_VAL(*reg_high_ptr, *reg_low_ptr), reg_src, memory_map);
    inc_rr(reg_high_ptr, reg_low_ptr);
}

// (address) <- register; decrement register pair
void ldd_addr_r(uint8_t * reg_high_ptr, uint8_t * reg_low_ptr, uint8_t reg_src, uint8_t * memory_map)
{
    ld_addr_r(REG_PAIR_VAL(*reg_high_ptr, *reg_low_ptr), reg_src, memory_map);
    dec_rr(reg_high_ptr, reg_low_ptr);
}

bool test_bit_is_0(uint8_t reg_ptr, uint8_t bit_num)
{
    return ((reg_ptr >> bit_num) ^ 0x1);
}

// jump if Z flag not set
void jr_nz_sn(struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    if (test_bit_is_0(cpu_ptr->FLAG, 7))
    {
        cpu_ptr->regPC += mmu_fetch_signed_8bit_val(memory_map, cpu_ptr->regPC); \
    }    
}

// jump if Z flag set
void jr_z_sn(struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    if (!test_bit_is_0(cpu_ptr->FLAG, 7))
    {
        cpu_ptr->regPC += mmu_fetch_signed_8bit_val(memory_map, cpu_ptr->regPC);
    }    
}

// jump to immediate 16 byte address
void jr_nn(Cpu * cpu_ptr, uint8_t * memory_map)
{
    printf("immediate address = 0x%x \n", mmu_fetch_16bit_val(memory_map, cpu_ptr->regPC));
    cpu_ptr->regPC = mmu_fetch_16bit_val(memory_map, cpu_ptr->regPC);
}

// jump
void jr_sn(Cpu * cpu_ptr, uint8_t * memory_map)
{
    cpu_ptr->regPC += mmu_fetch_signed_8bit_val(memory_map, cpu_ptr->regPC);
}

// xor reg with register A
void xor_a(uint8_t reg, Cpu * cpu_ptr)
{
    cpu_ptr->regA ^= reg;
    SET_Z_FLAG(cpu_ptr, (cpu_ptr->regA == 0));
    SET_N_FLAG(cpu_ptr, 0);
    SET_H_FLAG(cpu_ptr, 0);
    SET_C_FLAG(cpu_ptr, 0);
}

// or reg with register A
void or_a(uint8_t reg, Cpu * cpu_ptr)
{
    cpu_ptr->regA |= reg;
    SET_Z_FLAG(cpu_ptr, (cpu_ptr->regA == 0));
    SET_N_FLAG(cpu_ptr, 0);
    SET_H_FLAG(cpu_ptr, 0);
    SET_C_FLAG(cpu_ptr, 0);
}

// compare register A with value
void cp_a(uint8_t value, Cpu * cpu_ptr)
{
    SET_Z_FLAG(cpu_ptr, (cpu_ptr->regA == value));
    SET_N_FLAG(cpu_ptr, 1);
    SET_H_FLAG(cpu_ptr, (int) (cpu_ptr->regA & 0xf) < (int) (value & 0xf));
    SET_C_FLAG(cpu_ptr, (int) cpu_ptr->regA < (int) value);
}

// rotate register left through carry
void rl_r(uint8_t * reg_ptr, struct Cpu * cpu_ptr)
{
    uint8_t left_most_bit = (*reg_ptr) >> 7;
    *reg_ptr = ((*reg_ptr) << 1) | read_c_flag(cpu_ptr);

    SET_Z_FLAG(cpu_ptr, (*reg_ptr == 0));
    SET_N_FLAG(cpu_ptr, 0);
    SET_H_FLAG(cpu_ptr, 0);
    SET_C_FLAG(cpu_ptr, left_most_bit);
}

// rotate register A left through carry
void rla(Cpu * cpu_ptr)
{
    uint8_t left_most_bit = cpu_ptr->regA >> 7;
    cpu_ptr->regA = (cpu_ptr->regA << 1) | read_c_flag(cpu_ptr);

    SET_Z_FLAG(cpu_ptr, (cpu_ptr->regA == 0));
    SET_N_FLAG(cpu_ptr, 0);
    SET_H_FLAG(cpu_ptr, 0);
    SET_C_FLAG(cpu_ptr, left_most_bit);
}

// Z <- TEST_BIT_IS_0(bit_num, register)
// N <- reset
// H <- set
void bit(uint8_t bit_num, uint8_t reg, struct Cpu * cpu_ptr)
{
    SET_Z_FLAG(cpu_ptr, test_bit_is_0(reg, bit_num));
    SET_N_FLAG(cpu_ptr, 0);
    SET_H_FLAG(cpu_ptr, 1);
}

void call_nn(struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    cpu_ptr->regSP--;
    mmu_store_16bit_val(memory_map, cpu_ptr->regSP, cpu_ptr->regPC+2);
    cpu_ptr->regSP--;
    cpu_ptr->regPC = mmu_fetch_16bit_val(memory_map, cpu_ptr->regPC);
}

void ret(struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    cpu_ptr->regSP++;
    cpu_ptr->regPC = mmu_fetch_16bit_val(memory_map, cpu_ptr->regSP);
    cpu_ptr->regSP++;
}

void push_rr(uint8_t reg_high, uint8_t reg_low, struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    cpu_ptr->regSP--;
    mmu_store_16bit_val(memory_map, cpu_ptr->regSP, REG_PAIR_VAL(reg_high, reg_low));
    cpu_ptr->regSP--;
}

void pop_rr(uint8_t * reg_high, uint8_t * reg_low, struct Cpu * cpu_ptr, uint8_t * memory_map)
{
    cpu_ptr->regSP++;
    *reg_high = mmu_fetch_16bit_val(memory_map, cpu_ptr->regSP) >> 8;
    *reg_low = mmu_fetch_16bit_val(memory_map, cpu_ptr->regSP) & 0xFF;
    cpu_ptr->regSP++;
}

// Disable interrupts (after the instruction executed after ldi)
void ldi(){}

/*
    An 8 bit opcode can be broken down in the following way :
    [ 7, 6, 5, 4, 3, 2, 1, 0]
    |  x  |    y   |    z   |
          |  p  | q|
*/
void cpu_step(GameBoy * gameboy_ptr)
{
    Cpu * cpu_ptr = gameboy_ptr->cpu_ptr;
    uint8_t * memory_map = gameboy_ptr->mmu_ptr->memory_map;
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
            inc_rr(&(cpu_ptr->regB), &(cpu_ptr->regC));
            break;
        case 0x04:
            inc_r(&(cpu_ptr->regB), cpu_ptr);
            break;
        case 0x05:
            dec_r(&(cpu_ptr->regB), cpu_ptr);
            break;
        case 0x06:
            ld_r_n(&(cpu_ptr->regB), cpu_ptr, memory_map);
            cpu_ptr->regPC++;
            break;
        case 0x0B:
            dec_rr(&(cpu_ptr->regB), &(cpu_ptr->regC));
            break;
        case 0x0C:
            inc_r(&(cpu_ptr->regC), cpu_ptr);
            break;
        case 0x0D:
            dec_r(&(cpu_ptr->regC), cpu_ptr);
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
            inc_rr(&(cpu_ptr->regD), &(cpu_ptr->regE));
            break;
        case 0x14:
            inc_r(&(cpu_ptr->regD), cpu_ptr);
            break;
        case 0x15:
            dec_r(&(cpu_ptr->regD), cpu_ptr);
            break;
        case 0x16:
            ld_r_n(&(cpu_ptr->regD), cpu_ptr, memory_map);                        
            cpu_ptr->regPC++;
            break;
        case 0x17:
            rla(cpu_ptr);
            break;
        case 0x18:
            jr_sn(cpu_ptr, memory_map);
            cpu_ptr->regPC++;
            break;
        case 0x1A:
            ld_r_addr(&(cpu_ptr->regA), REG_PAIR_VAL(cpu_ptr->regD, cpu_ptr->regE), memory_map);
            break;
        case 0x1B:
            dec_rr(&(cpu_ptr->regD), &(cpu_ptr->regE));
            break;
        case 0x1C:
            inc_r(&(cpu_ptr->regE), cpu_ptr);
            break;
        case 0x1D:
            dec_r(&(cpu_ptr->regE), cpu_ptr);
            break;
        case 0x1E:
            ld_r_n(&(cpu_ptr->regE), cpu_ptr, memory_map);            
            cpu_ptr->regPC++;
            break;
        case 0x20:
            jr_nz_sn(cpu_ptr, memory_map);
            cpu_ptr->regPC++;
            break;
        case 0x21:
            ld_rr_nn(&(cpu_ptr->regH), &(cpu_ptr->regL), cpu_ptr, memory_map);
            cpu_ptr->regPC = cpu_ptr->regPC + 2;
            break;
        case 0x22:
            ldi_addr_r(&(cpu_ptr->regH), &(cpu_ptr->regL), cpu_ptr->regA, memory_map);
            break;
        case 0x23:
            inc_rr(&(cpu_ptr->regH), &(cpu_ptr->regL));
            break;
        case 0x24:
            inc_r(&(cpu_ptr->regH), cpu_ptr);
            break;
        case 0x25:
            dec_r(&(cpu_ptr->regH), cpu_ptr);
            break;
        case 0x26:
            ld_r_n(&(cpu_ptr->regH), cpu_ptr, memory_map);            
            cpu_ptr->regPC++;
            break;
        case 0x28:
            jr_z_sn(cpu_ptr, memory_map);
            cpu_ptr->regPC++;
            break;
        case 0x2A:
            ldi_r_addr(&(cpu_ptr->regA), &(cpu_ptr->regH), &(cpu_ptr->regL), memory_map);
            break;
        case 0x2B:
            dec_rr(&(cpu_ptr->regH), &(cpu_ptr->regL));
            break;
        case 0x2D:
            dec_r(&(cpu_ptr->regL), cpu_ptr);
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
            ldd_addr_r(&(cpu_ptr->regH), &(cpu_ptr->regL), cpu_ptr->regA, memory_map);
            break;
        case 0x36:
            ld_addr_r(REG_PAIR_VAL(cpu_ptr->regH, cpu_ptr->regL), memory_map[cpu_ptr->regPC], memory_map);
            cpu_ptr->regPC++;
            break;
        case 0x3B:
            cpu_ptr->regSP--;
            break;
        case 0x3C:
            inc_r(&(cpu_ptr->regA), cpu_ptr);
            break;
        case 0x3D:
            dec_r(&(cpu_ptr->regA), cpu_ptr);
            break;
        case 0x3E:
            ld_r_n(&(cpu_ptr->regA), cpu_ptr, memory_map);                        
            cpu_ptr->regPC++;
            break;
        case 0x4F:
            ld_r_r(&(cpu_ptr->regC), cpu_ptr->regA);
            break;
        case 0x57:
            ld_r_r(&(cpu_ptr->regD), cpu_ptr->regA);
            break;
        case 0x67:
            ld_r_r(&(cpu_ptr->regH), cpu_ptr->regA);
            break;
        case 0x70 ... 0x75:
            ld_addr_r(REG_PAIR_VAL(cpu_ptr->regH, cpu_ptr->regL), *get_reg_by_num(cpu_ptr, opcode & 0xF), memory_map);
            break;
        case 0x77:
            ld_addr_r(REG_PAIR_VAL(cpu_ptr->regH, cpu_ptr->regL), cpu_ptr->regA, memory_map);
            break;
        case 0x78 ... 0x7D:
            ld_r_r(&(cpu_ptr->regA), *get_reg_by_num(cpu_ptr, opcode & 0b111));
            break;
        case 0x80 ... 0x85:
            add_a(*get_reg_by_num(cpu_ptr, opcode & 0xF), cpu_ptr);
            break;
        case 0x86:
            add_a(memory_map[REG_PAIR_VAL(cpu_ptr->regH, cpu_ptr->regL)], cpu_ptr);
            break;
        case 0x87:
            add_a(cpu_ptr->regA, cpu_ptr);
            break;
        case 0x90 ... 0x95:
            sub_a(*get_reg_by_num(cpu_ptr, opcode & 0xF), cpu_ptr);
            break;
        case 0xA8 ... 0xAD: 
            xor_a(*get_reg_by_num(cpu_ptr, opcode & 0b111), cpu_ptr);
            break;
        case 0xAE:
            xor_a(REG_PAIR_VAL(cpu_ptr->regH, cpu_ptr->regL), cpu_ptr);
            break;
        case 0xAF:
            xor_a(cpu_ptr->regA, cpu_ptr);
            break;
        case 0xB0 ... 0xB5:
            or_a(*get_reg_by_num(cpu_ptr, opcode & 0xF), cpu_ptr);
            break;
        case 0xB8:
            cp_a(cpu_ptr->regB, cpu_ptr);
            break;
        case 0xB9:
            cp_a(cpu_ptr->regC, cpu_ptr);
            break;
        case 0xBA:
            cp_a(cpu_ptr->regD, cpu_ptr);
            break;
        case 0xBB:
            cp_a(cpu_ptr->regE, cpu_ptr);
            break;
        case 0xBC:
            cp_a(cpu_ptr->regH, cpu_ptr);
            break;
        case 0xBD:
            cp_a(cpu_ptr->regL, cpu_ptr);
            break;
        case 0xBE:
            cp_a(memory_map[REG_PAIR_VAL(cpu_ptr->regH, cpu_ptr->regL)], cpu_ptr);
            break;
        case 0xBF:
            cp_a(cpu_ptr->regA, cpu_ptr);
            break;
        case 0xC1:
            pop_rr(&(cpu_ptr->regB), &(cpu_ptr->regC), cpu_ptr, memory_map);
            break;
        case 0xC3:
            jr_nn(cpu_ptr, memory_map);
            break;
        case 0xC5:
            push_rr(cpu_ptr->regB, cpu_ptr->regC, cpu_ptr, memory_map);
            break;
        case 0xC9:
            ret(cpu_ptr, memory_map);
            break;
        case 0xCD:
            call_nn(cpu_ptr, memory_map);
            break;
        case 0xE0:
            ld_addr_r(0xFF00 + (uint16_t) memory_map[cpu_ptr->regPC], cpu_ptr->regA, memory_map);
            cpu_ptr->regPC++;
            break;
        case 0xE2:
            ld_addr_r(0xFF00 + (uint16_t) cpu_ptr->regC, cpu_ptr->regA, memory_map);
            break;
        case 0xEA:
            ld_addr_r(mmu_fetch_16bit_val(memory_map, cpu_ptr->regPC), cpu_ptr->regA, memory_map);
            cpu_ptr->regPC++;
            cpu_ptr->regPC++;
            break;
        case 0xF0:
            ld_r_addr(&(cpu_ptr->regA), 0xFF00 + (uint16_t) memory_map[cpu_ptr->regPC], memory_map);
            cpu_ptr->regPC++;
            break;
        case 0xF2:
            ld_r_addr(&(cpu_ptr->regA), 0xFF00 + (uint16_t) cpu_ptr->regC, memory_map);
            break;
        case 0xF3:
            ldi();
            break;
        case 0xFE:
            cp_a(memory_map[cpu_ptr->regPC], cpu_ptr);
            cpu_ptr->regPC++;
            break;
        case 0xCB:
            opcode = memory_map[cpu_ptr->regPC++];

            switch(opcode)
            {
                case 0x10 ... 0x15:
                    rl_r(get_reg_by_num(cpu_ptr, opcode & 0xF), cpu_ptr);
                    break;
                case 0x40 ... 0x45:
                    bit(0, *get_reg_by_num(cpu_ptr, opcode & 0xF), cpu_ptr);
                    break;
                case 0x48 ... 0x4D:
                    bit(1, *get_reg_by_num(cpu_ptr, opcode & 0b111), cpu_ptr);
                    break;
                case 0x50 ... 0x55:
                    bit(2, *get_reg_by_num(cpu_ptr, opcode & 0xF), cpu_ptr);
                    break;
                case 0x58 ... 0x5D:
                    bit(3, *get_reg_by_num(cpu_ptr, opcode & 0b111), cpu_ptr);
                    break;
                case 0x60 ... 0x65:
                    bit(4, *get_reg_by_num(cpu_ptr, opcode & 0xF), cpu_ptr);
                    break;
                case 0x68 ... 0x6D:
                    bit(5, *get_reg_by_num(cpu_ptr, opcode & 0b111), cpu_ptr);
                    break;
                case 0x70 ... 0x75:
                    bit(6, *get_reg_by_num(cpu_ptr, opcode & 0xF), cpu_ptr);
                    break;
                case 0x78 ... 0x7D:
                    bit(7, *get_reg_by_num(cpu_ptr, opcode & 0b111), cpu_ptr);
                    break;
                default:
                    printf("Register not found : %i \n", 0/0);
                    break;
            }
            break;

        default:
            debug_cpu(memory_map, cpu_ptr);
            printf("Instruction not implemented \n");
            printf("%i \n", 0/0);
            break;
    }

    cpu_ptr->prev_instruction_cycles = 4; // this will be changed
    //debug_cpu(memory_map, cpu_ptr);
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