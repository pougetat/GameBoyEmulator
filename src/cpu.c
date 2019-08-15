#include "mmap.h"
#include "cpu.h"

struct Cpu init_cpu() {

}

void execute_instruction(uint8_t * mmap, struct Cpu cpu)
{
    uint8_t hex_instr = mmap[cpu.pc_reg];

    // execute the instruction
}