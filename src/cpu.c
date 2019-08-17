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

    uint8_t opcode = mmap[cpu->regPC];
    cpu->regPC++;

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
        case 0xA8:
            XOR_A(cpu->regA, cpu->regB);
            break;
        case 0xA9:
            XOR_A(cpu->regA, cpu->regC);
            break;
        case 0xAA:
            XOR_A(cpu->regA, cpu->regD);
            break;
        case 0xAB:
            XOR_A(cpu->regA, cpu->regE);
            break;
        case 0xAC:
            XOR_A(cpu->regA, cpu->regH);
            break;
        case 0xAD:
            XOR_A(cpu->regA, cpu->regL);
        case 0xAE:
            XOR_A(cpu->regA, REG_PAIR_VAL(cpu->regH, cpu->regL));
            break;
        case 0xAF:
            XOR_A(cpu->regA, cpu->regA);
            break;
        default:
            break;
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