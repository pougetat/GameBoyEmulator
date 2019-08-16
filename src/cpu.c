#include <stdlib.h>

#include "mmap.h"
#include "cpu.h"
#include "cpu_instructions/ld_immediate_8bit.h"
#include "cpu_instructions/ld_immediate_16bit.h"
#include "cpu_instructions/alu.h"

struct Cpu * init_cpu() {
    struct Cpu * cpu = malloc(sizeof(struct Cpu));

    for (int i = 0; i < 7; i++)\
    {
        cpu->registers[i] = 0;
    }
    cpu->sp = 0;
    cpu->pc = 0;

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

    uint8_t hex_instr = mmap[cpu->pc];

    int x = hex_instr >> 6 & 0b11;
    int y = hex_instr >> 3 & 0b111;
    int z = hex_instr & 0b111;

    int p = hex_instr >> 4 & 0b11;
    int q = hex_instr >> 3 & 0b1;

    cpu->pc++;

    if (x == 0)
    {
        if (z == 1)
        {
            ld_immediate_16bit(p, cpu, mmap);
        }
        else if (z == 6)
        {
            ld_immediate_8bit(p, cpu, mmap);
        }
    }
    else if (x == 2)
    {
        alu_reg_operand(y, z, cpu);
    }
}

void debug_cpu(uint8_t * mmap, struct Cpu * cpu)
{
    printf("CPU state : \n \n");
    // register values
    printf("    reg B = 0x%x \n", cpu->registers[0]);
    printf("    reg C = 0x%x \n", cpu->registers[1]);
    printf("    reg D = 0x%x \n", cpu->registers[2]);
    printf("    reg E = 0x%x \n", cpu->registers[3]);
    printf("    reg H = 0x%x \n", cpu->registers[4]);
    printf("    reg L = 0x%x \n", cpu->registers[5]);
    printf("    reg A = 0x%x \n", cpu->registers[6]);
    printf("    sp = 0x%x \n", cpu->sp);
    printf("    pc = 0x%x \n", cpu->pc);
    // current instruction opcode
    printf("    current instruction : 0x%x \n", mmap[cpu->pc]);

    printf("\n");
}