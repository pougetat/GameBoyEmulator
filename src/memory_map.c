#include "memory_map.h"
#include <string.h>
#include <stdlib.h>

#define MEMORY_MAP_SIZE 65536 // 0x0000 - 0xFFFF
#define MAX_ROM_SIZE 16384 // 0x0000 - 0x3FFF (only handling 32 KB roms for now)

uint8_t * init_memory_map(FILE * rom_file)
{    
    uint8_t * memory_map = malloc(MEMORY_MAP_SIZE);

    int byte;
    uint16_t cur_pos = 0;
    while ((byte = fgetc(rom_file)) != EOF & cur_pos < MAX_ROM_SIZE)
    {
        memory_map[cur_pos++] = (uint8_t) byte;
    }
    fclose(rom_file);

    for (int i = 0; i < 256; i++) {
        printf("%i : %x \n", i, memory_map[i]);
    }
    printf("\n");

    return memory_map;
}