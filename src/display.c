#include "lxgbc.h"
#include "display.h"

void init_display(struct gbc_display *display) {

    // Init video subsystem
    SDL_Init(SDL_INIT_VIDEO);

    // Create window
    SDL_CreateWindowAndRenderer(
        SCREEN_WIDTH * SCREEN_SCALE,
        SCREEN_HEIGHT * SCREEN_SCALE,
        0,
        &display->window,
        &display->renderer                            
    );

    // Set the title and rendering scale
    SDL_SetWindowTitle(display->window, MAIN_WINDOW_TITLE);
    SDL_RenderSetScale(display->renderer, SCREEN_SCALE, SCREEN_SCALE);

}
