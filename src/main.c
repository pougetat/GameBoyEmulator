#include <stdio.h>
#include <stdbool.h>
#include "mmap.h"
#include "cpu.h"

int main(void)
{
    FILE * rom_file = fopen("simple.gb", "r");

    init_memory_map(rom_file);

    return 0;
}