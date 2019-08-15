#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MMAP_SIZE 65536 // 0x0000 - 0xFFFF
#define MAX_ROM_SIZE 16384 // 0x0000 - 0x3FFF (only handling 32 KB roms for now)

void init_memory_map(FILE * rom_file)
{    
    uint8_t * mmap = malloc(MMAP_SIZE);

    int byte;
    uint16_t cur_pos = 0;
    while ((byte = fgetc(rom_file)) != EOF & cur_pos < MAX_ROM_SIZE)
    {
        mmap[cur_pos++] = (uint8_t) byte;
    }
    fclose(rom_file);
}