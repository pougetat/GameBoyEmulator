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
        case 0x01: LD_r_nn(cpu->regB, cpu->regC, cpu, mmap); break;
        case 0x11: LD_r_nn(cpu->regD, cpu->regE, cpu, mmap); break;
        case 0x21: LD_r_nn(cpu->regH, cpu->regL, cpu, mmap); break;
        case 0x31: LD_SP_nn(cpu, mmap); break;
        case 0x06: LD_r_n(cpu->regB, cpu, mmap); break;
        case 0x0E: LD_r_n(cpu->regC, cpu, mmap); break;
        case 0x16: LD_r_n(cpu->regD, cpu, mmap); break;
        case 0x1E: LD_r_n(cpu->regE, cpu, mmap); break;
        case 0x26: LD_r_n(cpu->regH, cpu, mmap); break;
        case 0x2E: LD_r_n(cpu->regL, cpu, mmap); break;
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