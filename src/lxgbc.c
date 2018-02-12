#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "mbc.h"
#include "rom.h"
#include "debugger.h"
#include "ppu.h"
#include "sound.h"

int main(int argc, char **argv) {

    printf(CGRN LOGO CNRM);
    SDL_Event event;

    // Allocate memory for the gbc
    gbc_system *gbc = malloc(sizeof(gbc_system));
    gbc->cpu = malloc(sizeof(gbc_cpu));
    gbc->ppu = malloc(sizeof(gbc_ppu));
    gbc->ram = malloc(sizeof(gbc_ram));
    gbc->rom = malloc(sizeof(gbc_rom));
    gbc->debugger = malloc(sizeof(gbc_debugger));

    // Get the command line arguments
    cmd_options *cmd = malloc(sizeof(cmd_options));
    cmd->rom_path = malloc(sizeof(char) * ROM_PATH_LENGTH);

    if(!get_cl_arguments(argc, argv, cmd)) {
        exit(0);
    }

    // Initialize the system
    init_cpu(gbc->cpu);
    init_ram(gbc->ram);
    init_ppu(gbc->ppu, cmd->scale);

    gbc->is_running = 1;

    // Load the rom into memory
    if(!load_rom(gbc, cmd->rom_path)) {
        printf("ERROR: Could not load rom file!\n\n");
        exit(0);
    }

    // Show a message and initialise the debugger 
    if(cmd->debug) {
        print_rom_info(gbc->rom);

        printf("\nLXGBC DEBUGGER RUNNING\nType 'h' for information on the available commands.\n");
        init_debugger(gbc->debugger);
    }
       
    // Append the game title to the window title
    char *title = malloc(sizeof(24 * sizeof(char)));
    strcpy(title, MAIN_WINDOW_TITLE);
    strcat(title, " - ");
    strcat(title, gbc->rom->title);

    SDL_SetWindowTitle(gbc->ppu->window, title);

    // Main endless loop 
    // One execution of this loop represents one cpu clock in non debug mode
    while(gbc->is_running) {

        unsigned char lcd_on = read_register(gbc->ram, LCDC, LCDC_LCD_ENABLE);

        // In debug mode, execute instruction by instruction 
        if(cmd->debug) {
            debug(gbc);

            // Execute the instruction at the program counter 
            simulate_cpu(gbc);
                
            // Run the same amount of ppu clocks as cpu clocks
            // Also check for interrupts
            for(int i = 0; i < gbc->cpu->run_for; i++) {
                if(lcd_on) {
                    ppu_do_clock(gbc);
                }
                check_interrupt(gbc);
            }
            
            // Reset the count
            gbc->cpu->run_for = 0;
        }
        // In normal mode, do a cpu and ppu clock
        else {
            cpu_do_clock(gbc);

            if(lcd_on) {
                ppu_do_clock(gbc);
            }

            check_interrupt(gbc);

            // TODO: Remove this
            SDL_PollEvent(&event);
            if(event.type == SDL_QUIT) {
                gbc->is_running = 0; 
            }
        }
    }

    return 0;
}

// Populates the cmd_options struct with the selected options from the command line
static char get_cl_arguments(int argc, char **argv, cmd_options *cmd) {

    int option;
    int f_flag, s_flag = 0;

    while((option = getopt(argc, argv, "f:gs:")) != -1) {
        switch (option) {
            case 'f':
                cmd->rom_path = optarg;
                f_flag = 1;
                break;
            
            case 'g':
                cmd->debug = 1;
                break;

            case 's':
                cmd->scale = atoi(optarg);
                s_flag = 1;
                break;

            default: 
                printf("Usage: -f rom [-g debug mode] [-s screen scale]\n"); 
                return 0;
        }
    }

    if(!f_flag) {
        printf("%s: missing -f option\n", argv[0]);
        return 0;
    }

    if(!s_flag) {
        cmd->scale = 1;
    }

    return 1;
}
