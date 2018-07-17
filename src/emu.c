#include "lxgbc.h"
#include "ram.h"
#include "ppu.h"
#include "cart.h"
#include "cpu.h"
#include "alu.h"
#include "instr.h"
#include "emu.h"
#include "input.h"


// Initialises the system
void init_system(gbc_system *gbc, const char *rom_path) {

    gbc->cpu = malloc(sizeof(gbc_cpu));
    gbc->ppu = malloc(sizeof(gbc_ppu));
    gbc->ram = malloc(sizeof(gbc_ram));
    gbc->cart = malloc(sizeof(gbc_cart));
    gbc->input = malloc(sizeof(gbc_input));

    init_cpu(gbc->cpu);
    init_ram(gbc);
    init_ppu(gbc->ppu);
    init_input(gbc->input);

    if(!load_rom(gbc, rom_path)) {
        fprintf(stderr, "ERROR: Could not load rom file!\n");
        exit(EXIT_FAILURE);
    }

    write_byte(gbc, LCDC, DEFAULT_LCDC, false);
    write_byte(gbc, IF, DEFAULT_IF, false);

    gbc->is_running = true;
    gbc->clocks = 0;
}

// Sets the window title to the game title
void set_window_title(SDL_Window *window, const char *game_title, const bool debugger) {

    char *title;

    if(debugger) {
        const unsigned char len = TITLE_LENGTH + strlen(MAIN_WINDOW_TITLE " -  (DEBUGGER)") + 1;
        title = malloc(sizeof(char) * len);

        snprintf(title, len, "(DEBUGGER) %s - %s", MAIN_WINDOW_TITLE, game_title);
    } else {
        const unsigned char len = TITLE_LENGTH + strlen(MAIN_WINDOW_TITLE " - ") + 1;
        title = malloc(sizeof(char) * len);

        snprintf(title, len, "%s - %s", MAIN_WINDOW_TITLE, game_title);
    }

    title[strlen(title)] = '\0';
    SDL_SetWindowTitle(window, title);
    free(title);
}
