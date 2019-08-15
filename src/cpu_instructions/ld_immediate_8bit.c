#include "../mmap.h"
#include "ld_immediate_8bit.h"

void ld_immediate_8bit(int reg, struct Cpu * cpu, uint8_t * mmap)
{
    cpu->registers[reg] = FETCH_8BIT_VAL(mmap, cpu->pc);
    cpu->pc++;
}
