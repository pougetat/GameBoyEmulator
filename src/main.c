#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "mmap.h"
#include "cpu.h"

int main(void)
{
    FILE * rom_file = fopen("simple.gb", "r");

    uint8_t * mmap = init_memory_map(rom_file);

    return 0;
}