#include "lxgbc.h"
#include "cpu.h"
#include "ram.h"
#include "rom.h"
#include "debugger.h"
#include "display.h"

int main(int argc, char **argv) {

    // Allocate memory for the gbc
    struct gbc_system *gbc = malloc(sizeof(*gbc));
    struct gbc_rom *rom = malloc(sizeof(*rom));
    struct gbc_display *display = malloc(sizeof(*display));

    // Initialize the system
    init_cpu(&gbc);
    init_ram(&gbc->ram);
    //init_display(&display);

    // Temp
    gbc->is_running = 1;
    gbc->is_debugging = 0;
    gbc->interrupts_enabled = 1;

    // Load the rom into memory
    if(!load_rom(&gbc->ram, &rom, argv[1])) {
        printf("ERROR: Could not load rom file!\n\n");
    }

    // Main Loop
    while(gbc->is_running) {

        // Debug: breakpoint
        if(gbc->registers->PC == 0x29a) {
            gbc->is_debugging = 1;
        }

        // Read in memory at the program counter
        execute_instr(&gbc);

        if(gbc->is_debugging) {
            print_debug(&gbc);
            getchar();
        }
    }

    // Free memory
    free(gbc);
    free(rom);

    return 0;
}
