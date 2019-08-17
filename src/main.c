#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "mmap.h"
#include "cpu.h"

int main(void)
{
    FILE * rom_file = fopen("simple.gb", "r");

    uint8_t * mmap = init_memory_map(rom_file);
    struct Cpu * cpu = init_cpu();

    while (REG_PAIR_VAL(cpu->regH, cpu->regL) != 0x8000)
    {
        execute_instruction(mmap, cpu);
    }
    while (true)
    {
        execute_instruction(mmap, cpu);
    }

    return 0;
}