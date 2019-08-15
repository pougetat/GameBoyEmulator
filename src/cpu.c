#include "mmap.h"
#include "cpu.h"
#include "cpu_instructions/ld_immediate_8bit.h"

struct Cpu init_cpu() {

}


/*
    An 8 bit opcode can be broken down in the following way :
    [ 7, 6, 5, 4, 3, 2, 1, 0]
    |  x  |    y   |    z   |
*/
void execute_instruction(uint8_t * mmap, struct Cpu * cpu)
{
    uint8_t hex_instr = mmap[cpu->pc];

    int x = hex_instr >> 6 & 0b11;
    int y = hex_instr >> 3 & 0b111;
    int z = hex_instr & 0b111;

    cpu->pc++;

    if (x == 0)
    {
        if (z == 6)
        {
            ld_immediate_8bit(y, cpu, mmap);
        }
    }

    // execute the instruction
}