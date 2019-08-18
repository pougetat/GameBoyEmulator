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

    while (cpu->regPC != 0x13)
    {
        execute_instruction(mmap, cpu);
    }

    execute_instruction(mmap, cpu);
    execute_instruction(mmap, cpu);
    execute_instruction(mmap, cpu);
    execute_instruction(mmap, cpu);
    execute_instruction(mmap, cpu);
    execute_instruction(mmap, cpu);
    execute_instruction(mmap, cpu);
    execute_instruction(mmap, cpu);

    printf("%x \n", mmap[0xff26]);
    printf("%x \n", mmap[0xff11]);

    return 0;
}