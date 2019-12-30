#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "gameboy.h"

int main(void)
{
    GameBoy * gameboy_ptr = gameboy_init();
    gameboy_setup_boot(gameboy_ptr);

    FILE * rom_file = fopen("../testRoms/tetris.gb", "r");
    gameboy_read_rom(gameboy_ptr, rom_file);
    gameboy_run(gameboy_ptr);
    return 0;
}