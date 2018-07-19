#include "jgbc.h"
#include "ram.h"
#include "cpu.h"
#include "mbc.h"
#include "cart.h"
#include "ppu.h"
#include "apu.h"
#include "input.h"
#include "emu.h"

static void handle_event(SDL_Event, gbc_system *);
static void print_rom_info(gbc_cart *);


int main(int argc, char **argv) {

    gbc_system *gbc = malloc(sizeof(gbc_system));

    if(argc != 2) {
        fprintf(stderr, "ERROR: Please specify a rom file\n");
        return EXIT_FAILURE; 
    }

    init_system(gbc, argv[1]);
    init_window(gbc->ppu);

    set_window_title(gbc->ppu->window, gbc->cart->title, false);
    print_rom_info(gbc->cart);

    // TEMP: skip logo
    gbc->cpu->registers->PC = 0x100;
    write_byte(gbc, 0xff50, 1, false);

    SDL_Event event;
    static const uint32_t max_clocks = CLOCK_SPEED / FRAMERATE;
    uint32_t frame_clocks = 0;

    // Main endless loop 
    while(gbc->is_running) {

        // Run the clocks for this frame
        while(frame_clocks < max_clocks) {

            gbc->clocks = 0;

            execute_instr(gbc);
            check_interrupts(gbc);

            update_timer(gbc);
            update_ppu(gbc);

            frame_clocks += gbc->clocks;
        }

        frame_clocks -= max_clocks;

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
static void print_rom_info(gbc_cart *cart) {
    printf("Title: %s\n", cart->title);
    printf("CGB Flag: %02X\n", cart->cgb_flag);
    printf("Cartridge Type: %02X\n", cart->type);
    printf("ROM Size: %d x %d KB\n", cart->rom_size, ROM_BANK_SIZE);
    printf("RAM Size: %d x %d KB\n", cart->ram_size, EXTRAM_BANK_SIZE);
    printf("Destination Code: %02X\n", cart->dest_code);
    printf("Version Number: %02X\n\n", cart->ver_no);
}