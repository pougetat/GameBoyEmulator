// SDL2 Hello, World!
// This should display a white screen for 2 seconds
// compile with: clang++ main.cpp -o hello_sdl2 -lSDL2
// run with: ./hello_sdl2
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "gui.h"

#undef main

int main(int argc, char* args[])
{
    Gui * gui_ptr = gui_init();

    for (int i = 0; i < GUI_SCREEN_PIXEL_HEIGHT; i++)
    {
        for (int j = 0; j < GUI_SCREEN_PIXEL_WIDTH; j++)
        {
            gui_ptr->frame_data[i][j] = 2;
        }
    }
    gui_render_frame(gui_ptr);

    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = true;
            }
        }
    }
    return 0;
}