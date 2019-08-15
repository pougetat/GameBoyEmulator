#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MMAP_SIZE 65536 // 0x0000 - 0xFFFF
#define MAX_ROM_SIZE 16384 // 0x0000 - 0x3FFF (only handling 32kb roms for now)

int * mmap;

void init_memory_map(FILE * rom_file)
{    
    // reading rom file contents
    int rom_data[MAX_ROM_SIZE];
    int byte;
    int cur_pos = 0;
    while ((byte = fgetc(rom_file)) != EOF)
    {
        rom_data[cur_pos++] = byte;
    }
    fclose(rom_file);

    // copying rom file contents to memory map
    mmap = malloc(MMAP_SIZE);
    memcpy(mmap, rom_data, MAX_ROM_SIZE);
}