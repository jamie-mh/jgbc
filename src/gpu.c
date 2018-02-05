#include "lxgbc.h"
#include "ram.h"
#include "gpu.h"

// Opens a SDL window, WIP
void init_gpu(gbc_gpu *gpu, const char scale) {

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

void render(gbc_ram *ram, gbc_gpu *gpu) {
    
    // Increment the Y coordinate register
    unsigned char y = read_byte(ram, LY);
    if(y < 153) {
        write_byte(ram, LY, y + 1);
    } else {
        write_byte(ram, LY, 0x0);
    }

    
}

// Checks if we are in a vblank period
char is_vblank(gbc_ram *ram) {

    unsigned char y = read_byte(ram, LY);

    if(y >= 144 && y <= 153) {
        return 1; 
    } else {
        return 0; 
    }
}
