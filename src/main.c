#include "jgbc.h"
#include "ppu.h"
#include "cpu.h"
#include "mmu.h"
#include "cart.h"
#include "input.h"

static void handle_event(GameBoy *gb, SDL_Event event);
static void set_window_title(GameBoy *gb);
static void run(GameBoy *gb);


int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: jgbc.exe <path to rom>\n");
        return EXIT_FAILURE; 
    }

    GameBoy *gb = malloc(sizeof(GameBoy));
    init(gb);

    if(!load_rom(gb, argv[1])) {
        fprintf(stderr, "ERROR: Cannot load rom file\n");
        return EXIT_FAILURE;
    }

    SDL_Init(SDL_INIT_VIDEO);
    init_window(gb);

    print_cart_info(gb);
    set_window_title(gb);

    run(gb);

    SDL_Quit();
    return EXIT_SUCCESS;
}

static void run(GameBoy *gb) {

    SDL_Event event;
    gb->is_running = true;

    while(gb->is_running) {

        static const uint32_t max_ticks = CLOCK_SPEED / FRAMERATE;
        uint32_t frame_ticks = 0;

        while(frame_ticks < max_ticks) {
            execute_instr(gb);

            update_ppu(gb);
            update_timer(gb);
            check_interrupts(gb);

            frame_ticks += gb->cpu.ticks;
        }

        frame_ticks -= max_ticks;

        while(SDL_PollEvent(&event)) {
            handle_event(gb, event);
        }
    }
}

static void handle_event(GameBoy *gb, SDL_Event event) {

    switch(event.type) {
        case SDL_QUIT:
            gb->is_running = false;
            break;

        case SDL_KEYDOWN:
            set_key(gb, event.key.keysym.scancode, true);
            break;

        case SDL_KEYUP:
            set_key(gb, event.key.keysym.scancode, false);
            break;
    }
}

static void set_window_title(GameBoy *gb) {
    char buffer[30];
    snprintf(buffer, 30, "%s - %s", WINDOW_TITLE, gb->cart.title);
    SDL_SetWindowTitle(gb->ppu.window, buffer);
}