#include "lxgbc.h"
#include "ram.h"
#include "ppu.h"
#include "rom.h"
#include "cpu.h"
#include "alu.h"
#include "instr.h"
#include "emu.h"

// Initialises the system
void init_system(gbc_system *gbc, const char *rom_path) {

    gbc->cpu = malloc(sizeof(gbc_cpu));
    gbc->ppu = malloc(sizeof(gbc_ppu));
    gbc->ram = malloc(sizeof(gbc_ram));
    gbc->rom = malloc(sizeof(gbc_rom));

    init_cpu(gbc->cpu);
    init_ram(gbc);
    init_ppu(gbc->ppu);

    if(!load_rom(gbc, rom_path)) {
        fprintf(stderr, "ERROR: Could not load rom file!\n");
        exit(EXIT_FAILURE);
    }

    gbc->is_running = true;
}

// Sets the window title to the game title
void set_window_title(SDL_Window *window, const char *game_title) {
    char title[TITLE_LENGTH + strlen(MAIN_WINDOW_TITLE " - ") + 1];
    snprintf(title, TITLE_LENGTH + 1, "%s - %s", MAIN_WINDOW_TITLE, game_title);
    title[strlen(title)] = '\0';

    SDL_SetWindowTitle(window, title);
}