#include <stdlib.h>

#include "mmap.h"
#include "cpu.h"
#include "cpu_instructions.h"

struct Cpu * init_cpu() {
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

/*
    An 8 bit opcode can be broken down in the following way :
    [ 7, 6, 5, 4, 3, 2, 1, 0]
    |  x  |    y   |    z   |
          |  p  | q|
*/
void execute_instruction(uint8_t * mmap, struct Cpu * cpu)
{
    debug_cpu(mmap, cpu);

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
        case 0x11:
            LD_rr_nn(cpu->regD, cpu->regE, cpu, mmap);
            cpu->regPC = cpu->regPC + 2;
            break;
        case 0x0E:
            LD_r_n(cpu->regC, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x21:
            LD_rr_nn(cpu->regH, cpu->regL, cpu, mmap);
            cpu->regPC = cpu->regPC + 2;
            break;
        case 0x22:
            LDI_addr_r(mmap, cpu->regH, cpu->regL, cpu->regA);
            break;
        case 0x31:
            LD_SP_nn(cpu, mmap);
            cpu->regPC = cpu->regPC + 2;
            break;
        case 0x32:
            LDD_addr_r(mmap, cpu->regH, cpu->regL, cpu->regA);
            break;
        case 0x16:
            LD_r_n(cpu->regD, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x1E:
            LD_r_n(cpu->regE, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x26:
            LD_r_n(cpu->regH, cpu, mmap);
            cpu->regPC++;
            break;
        case 0x2E:
            LD_r_n(cpu->regL, cpu, mmap);
            cpu->regPC++;
            break;
        case 0xA8 ... 0xAD: 
            XOR_A(cpu->regA, get_reg_by_num(cpu, opcode & 0xF));
            break;
        case 0xAE:
            XOR_A(cpu->regA, REG_PAIR_VAL(cpu->regH, cpu->regL));
            break;
        case 0xAF:
            XOR_A(cpu->regA, cpu->regA);
            break;
        case 0xCB:
            opcode = mmap[cpu->regPC++];

            switch(opcode)
            {
                case 0x40 ... 0x45:
                    BIT(0, get_reg_by_num(cpu, opcode & 0xF), cpu->FLAG);
                    break;
                case 0x48 ... 0x4D:
                    BIT(1, get_reg_by_num(cpu, opcode & 0xF), cpu->FLAG);
                    break;
                case 0x50 ... 0x55:
                    BIT(2, get_reg_by_num(cpu, opcode & 0xF), cpu->FLAG);
                    break;
                case 0x58 ... 0x5D:
                    BIT(3, get_reg_by_num(cpu, opcode & 0xF), cpu->FLAG);
                    break;
                case 0x60 ... 0x65:
                    BIT(4, get_reg_by_num(cpu, opcode & 0xF), cpu->FLAG);
                    break;
                case 0x68 ... 0x6D:
                    BIT(5, get_reg_by_num(cpu, opcode & 0xF), cpu->FLAG);
                    break;
                case 0x70 ... 0x75:
                    BIT(6, get_reg_by_num(cpu, opcode & 0xF), cpu->FLAG);
                    break;
                case 0x78 ... 0x7D:
                    BIT(7, get_reg_by_num(cpu, opcode & 0xF), cpu->FLAG);
                    break;
                default:
                    break;
            }

        default:
            break;
    }
}

uint8_t get_reg_by_num(struct Cpu * cpu, uint8_t reg_num)
{
    if (reg_num == 0)
    {
        return cpu->regB;
    }
    else if (reg_num == 1)
    {
        return cpu->regC;
    }
    else if (reg_num == 2)
    {
        return cpu->regD;
    }
    else if (reg_num == 3)
    {
        return cpu->regE;
    }
    else if (reg_num == 4)
    {
        return cpu->regH;
    }
    else if (reg_num == 5)
    {
        return cpu->regL;
    }
    else if (reg_num == 6)
    {
        return cpu->regA;
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
    printf("    reg A = 0x%x \n", cpu->regA);
    printf("    sp = 0x%x \n", cpu->regSP);
    printf("    pc = 0x%x \n", cpu->regPC);
    // current instruction opcode
    printf("    current instruction : 0x%x \n", mmap[cpu->regPC]);

    printf("\n");
}