#include "../mmap.h"
#include "ld_immediate_8bit.h"

void ld_immediate_16bit(int reg, struct Cpu * cpu, uint8_t * mmap)
{
    /*
        The opcodes define the following things :
            reg = 0 -> BC
            reg = 1 -> DE
            reg = 2 -> HL
            reg = 3 -> SP
    */

    uint16_t value = FETCH_16BIT_VAL(mmap, cpu->pc);
    cpu->pc = cpu->pc + 2;

    if (reg == 3)
    {
        cpu->sp = value;
    }
    else
    {
        cpu->registers[2*reg] = value >> 8;
        cpu->registers[2*reg+1] = value && 0xFF;    
    }
}