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

    while (cpu->regPC != 0x95)
    {
        execute_instruction(memory_map, cpu);
    }

    printf("%x \n", memory_map[0xff12]);
    printf("%x \n", memory_map[0xff25]);
    printf("%x \n", memory_map[0xff24]);
    printf("%x \n", memory_map[0xff47]);
    printf("%x \n", memory_map[0xfffd]);
    printf("%x \n", memory_map[0xfffe]);

    return 0;
}