#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "gameboy.h"

int main(void)
{
    printf("Hello \n");
    GameBoy * gameboy_ptr = gameboy_init();
    FILE * rom_file = fopen("../testRoms/simple.gb", "r"); 
    gameboy_read_rom(gameboy_ptr, rom_file);
    gameboy_run(gameboy_ptr);
    return 0;
}