#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "mbc.h"
#include "rom.h"
#include "ppu.h"
#include "sound.h"
#include "input.h"
#include "emu.h"

static void handle_event(SDL_Event, gbc_system *);

int main(int argc, char **argv) {

    gbc_system *gbc = malloc(sizeof(gbc_system));

    if(argc != 2) {
        fprintf(stderr, "ERROR: Please specify a rom file\n");
        return EXIT_FAILURE; 
    }

    init_system(gbc, argv[1]);
    init_window(gbc->ppu);

    set_window_title(gbc->ppu->window, gbc->rom->title, false);

    SDL_Event event;
    unsigned int last_time = 0;

    // Main endless loop 
    while(gbc->is_running) {

        static const unsigned int max_clocks = CLOCK_SPEED / FRAMERATE;
        static const unsigned char millis_per_frame = (1 / FRAMERATE) * 1000.0;

        unsigned int frame_clocks = 0;
        last_time = SDL_GetTicks();

        // Run the clocks for this frame
        while(frame_clocks < max_clocks) {

            unsigned char clocks = 0;

            if(gbc->cpu->is_halted == false) {
                clocks = execute_instr(gbc);                
            } else {
                clocks = 4;
            }

            check_interrupts(gbc);

            update_timer(gbc, clocks);
            update_ppu(gbc, clocks);

            frame_clocks += clocks;

            write_byte(gbc, JOYP, 0x1F, false); // Temp simulate no buttons
        }

        render_to_window(gbc->ppu);

        while(SDL_PollEvent(&event)) {
            handle_event(event, gbc);
        }

        // Run at the framerate and remove the time it took to compute this frame
        int execute_time = SDL_GetTicks() - last_time;

        if(execute_time < millis_per_frame) {
            SDL_Delay(millis_per_frame - execute_time);
        }
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}

// Handles events from SDL
static void handle_event(SDL_Event event, gbc_system *gbc) {

    switch(event.type) {
        case SDL_QUIT:
            gbc->is_running = 0;
            break;

        case SDL_KEYUP:
            handle_input(gbc, event.key); 
            break;
    }
}
