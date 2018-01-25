#include "lxgbc.h"
#include "gpu.h"

// Opens a SDL window, WIP
void init_gpu(gbc_gpu *gpu, char scale) {

    // Init video subsystem
    SDL_Init(SDL_INIT_VIDEO);

    // Create window
    SDL_CreateWindowAndRenderer(
        SCREEN_WIDTH * scale,
        SCREEN_HEIGHT * scale,
        0,
        &gpu->window,
        &gpu->renderer                            
    );

    // Set the title and rendering scale
    SDL_SetWindowTitle(gpu->window, MAIN_WINDOW_TITLE);
    SDL_RenderSetScale(gpu->renderer, scale, scale);
}
