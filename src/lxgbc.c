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
static void print_rom_info(gbc_rom *);


int main(int argc, char **argv) {

    gbc_system *gbc = malloc(sizeof(gbc_system));

    if(argc != 2) {
        fprintf(stderr, "ERROR: Please specify a rom file\n");
        return EXIT_FAILURE; 
    }

    init_system(gbc, argv[1]);
    init_window(gbc->ppu);

    set_window_title(gbc->ppu->window, gbc->rom->title, false);
    print_rom_info(gbc->rom);

    // TEMP: skip logo
    gbc->cpu->registers->PC = 0x100;
    write_byte(gbc, 0xff50, 1, false);

    SDL_Event event;

    // Main endless loop 
    while(gbc->is_running) {

        static const unsigned int max_clocks = CLOCK_SPEED / FRAMERATE;
        unsigned int frame_clocks = 0;

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
        }

        while(SDL_PollEvent(&event)) {
            handle_event(event, gbc);
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

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            handle_input(gbc->input, event.key); 
            break;
    }
}

// Prints the rom info to the terminal
static void print_rom_info(gbc_rom *rom) {
    printf("Title: %s\n", rom->title);
    printf("CGB Flag: %02X\n", rom->cgb_flag);
    printf("Cartridge Type: %02X\n", rom->cart_type);
    printf("ROM Size: %d x %d KB\n", rom->rom_size, ROM_BANK_SIZE);
    printf("RAM Size: %d x %d KB\n", rom->ram_size, EXTRAM_BANK_SIZE);
    printf("Destination Code: %02X\n", rom->dest_code);
    printf("Version Number: %02X\n\n", rom->ver_no);
}
