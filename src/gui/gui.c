// SDL2 Hello, World!
// This should display a white screen for 2 seconds
// compile with: clang++ main.cpp -o hello_sdl2 -lSDL2
// run with: ./hello_sdl2

#include "gui.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface* screenSurface = NULL;

Gui * gui_init()
{
    Gui * gui_ptr = malloc(sizeof(Gui));
    gui_ptr->frame_data = malloc(GUI_SCREEN_PIXEL_HEIGHT * sizeof(uint8_t *));

    for (uint8_t i = 0; i < GUI_SCREEN_PIXEL_HEIGHT; i++)
    {
        gui_ptr->frame_data[i] = malloc(GUI_SCREEN_PIXEL_WIDTH * sizeof(uint8_t));
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        return;
    }

    SDL_CreateWindowAndRenderer(
        GUI_SCREEN_PIXEL_WIDTH,
        GUI_SCREEN_PIXEL_HEIGHT,
        0,
        &window,
        &renderer
    );
    return gui_ptr;
}

void gui_render_frame(Gui * gui_ptr)
{
    for (uint8_t i = 0; i < 144; i++)
    {
        for (uint8_t j = 0; j < 160; j++)
        {
            uint8_t r = (gui_ptr->frame_data[i][j] == 0b00);
            uint8_t g = (gui_ptr->frame_data[i][j] == 0b01);
            uint8_t b = (gui_ptr->frame_data[i][j] == 0b10);
            uint8_t a = (gui_ptr->frame_data[i][j] == 0b11);

            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
    SDL_RenderPresent(renderer);    
}