#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "mbc.h"
#include "rom.h"
#include "debugger.h"
#include "ppu.h"
#include "sound.h"

static void init_system(gbc_system *, cmd_options *);
static void handle_event(SDL_Event, gbc_system *);
static char get_cl_arguments(int, char **, cmd_options *);


int main(int argc, char **argv) {

    gbc_system *gbc = malloc(sizeof(gbc_system));
    cmd_options *cmd = malloc(sizeof(cmd_options));

    // Check that the command line arguments are valid
    if(!get_cl_arguments(argc, argv, cmd)) {
        return -1;
    }

    init_system(gbc, cmd);
    gbc->is_running = 1;
    free(cmd);

    SDL_Event event;

    // Main endless loop 
    // One execution of this loop represents one cpu clock
    while(gbc->is_running) {

        unsigned char lcd_on = read_register(gbc->ram, LCDC, LCDC_LCD_ENABLE);

        if(gbc->cpu->is_halted == 0) {
            cpu_do_clock(gbc);
        }

        // In debug mode, we need to run as many ppu clocks as cpu clocks
        // and instantly run the instruction
        if(gbc->debugger->is_debugging) {

            debug(gbc);
            int clocks;

            // Still run the ppu when the cpu is halted
            if(gbc->cpu->is_halted) {
                clocks = 1;
            } else {
                clocks = gbc->cpu->run_for;
            }

            // Run as many ppu clocks as cpu clocks
            for(int i = 0; i < clocks; i++) {
                if(lcd_on) {
                    ppu_do_clock(gbc);
                }

                check_interrupt(gbc);
                update_timer(gbc);
            }

            gbc->cpu->run_for = gbc->cpu->clock;                
        }
        // In normal mode, respect clock cycles
        else {

            if(lcd_on) {
                ppu_do_clock(gbc);
            }

            check_interrupt(gbc);
            update_timer(gbc);
        }
        
        // Handle SDL events
        if(SDL_PollEvent(&event)) {
            handle_event(event, gbc);
        }

        // Temp: Simulate no buttons pressed
        write_byte(gbc->ram, 0xFF00, 0xEF, 0);
    }

    SDL_Quit();
    return 0;
}

// Initialises the system
static void init_system(gbc_system *gbc, cmd_options *cmd) {

    // Allocate memory    
    gbc->cpu = malloc(sizeof(gbc_cpu));
    gbc->ppu = malloc(sizeof(gbc_ppu));
    gbc->ram = malloc(sizeof(gbc_ram));
    gbc->rom = malloc(sizeof(gbc_rom));
    gbc->debugger = malloc(sizeof(gbc_debugger));

    init_cpu(gbc->cpu);
    init_ram(gbc->ram);
    init_ppu(gbc->ppu, cmd->scale);
    init_debugger(gbc->debugger);

    // Load the rom into memory
    if(!load_rom(gbc, cmd->rom_path)) {
        fprintf(stderr, "ERROR: Could not load rom file!\n");
        exit(-1);
    }

    // Append the game title to the window title
    char title[TITLE_LENGTH + strlen(MAIN_WINDOW_TITLE " - ") + 1];
    sprintf(title, "%s - %s", MAIN_WINDOW_TITLE, gbc->rom->title);

    SDL_SetWindowTitle(gbc->ppu->window, title);

    // Show a message and initialise the debugger 
    if(cmd->debug) {
        printf(CGRN LOGO CNRM);
        print_rom_info(gbc->rom);
        printf("\nLXGBC DEBUGGER RUNNING\nType 'h' for information on the available commands.\n");
        printf(DEBUG_PROMPT);

        gbc->debugger->is_debugging = 1;
    }
}

// Handles events from SDL
static void handle_event(SDL_Event event, gbc_system *gbc) {

    switch(event.type)
    {
        case SDL_QUIT:
            gbc->is_running = 0;
            break;

        case SDL_KEYUP:
            if(event.key.keysym.sym == SDLK_ESCAPE) {
                gbc->debugger->is_running = 0;

                if(gbc->debugger->is_debugging == 0) {
                    gbc->debugger->is_debugging = 1;
                    printf(DEBUG_PROMPT);
                }
            }

            break;
    }
}

// Populates the cmd_options struct with the selected options from the command line
static char get_cl_arguments(int argc, char **argv, cmd_options *cmd) {

    int option;
    int f_flag, s_flag = 0;

    cmd->debug = 0;

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
