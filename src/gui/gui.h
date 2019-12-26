#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define GUI_SCREEN_PIXEL_HEIGHT 144
#define GUI_SCREEN_PIXEL_WIDTH 160

typedef struct Gui {
    uint8_t ** frame_data;
} Gui;

Gui * gui_init();
void gui_render_frame();

#endif