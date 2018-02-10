#include "lxgbc.h"
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

void gpu_do_clock(gbc_system *gbc) {

}

// Simulates the renderering of the lcd in debug mode
// so that programs don't hang waiting for a v-blank or h-blank
void simulate_gpu(gbc_system *gbc) {
    
    // Increment the Y coordinate register
    unsigned char y = read_byte(gbc->ram, LY);
    if(y < 153) {
        write_byte(gbc->ram, LY, y + 1);
    } else {
        write_byte(gbc->ram, LY, 0x0);
    }
}
