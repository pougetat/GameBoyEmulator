#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "memory_map.h"
#include "cpu.h"

int main(void)
{
    FILE * rom_file = fopen("../testRoms/simple.gb", "r");

    uint8_t * memory_map = init_memory_map(rom_file);
    struct Cpu * cpu = init_cpu();

    while (cpu->regPC != 0x70) // CURRENTLY INFINITE LOOP (not at 0x64)
    {
        execute_instruction(memory_map, cpu);
    }
    execute_instruction(memory_map, cpu);
    return 0;
}