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
static bool get_cl_arguments(int, char **, cmd_options *);


int main(int argc, char **argv) {

    gbc_system *gbc = malloc(sizeof(gbc_system));
    cmd_options *cmd = malloc(sizeof(cmd_options));

    if(!get_cl_arguments(argc, argv, cmd)) {
        return EXIT_FAILURE; 
    }

    init_system(gbc, cmd);
    /*free(cmd);*/

    SDL_Event event;
    int last_time = 0;

    // Main endless loop 
    // One execution of this loop represents one cpu clock
    while(gbc->is_running) {

        static const int max_clocks = CLOCK_SPEED / FRAMERATE;
        int frame_clocks = 0;

        last_time = SDL_GetTicks();

        // Run the clocks for this frame
        while(frame_clocks < max_clocks) {

            unsigned char clocks = 0;

            if(gbc->debugger->is_debugging) {
                debug(gbc);
            }

            if(gbc->cpu->is_halted == false) {
                clocks = execute_instr(gbc);                
            } else {
                clocks = 1;
            }

            frame_clocks += clocks;

            // Run the ppu and timer for the same amount of clocks
            for(unsigned char i = 0; i < clocks; i++) {
                ppu_do_clock(gbc);
                update_timer(gbc);
            }

            check_interrupt(gbc);

            if(SDL_PollEvent(&event)) {
                handle_event(event, gbc);
            }
        }

        render(gbc->ppu);

        if(!cmd->no_limit) {

            // Run at the framerate and remove the time it took to compute this frame
            int execute_time = last_time - SDL_GetTicks();
            SDL_Delay((1 / FRAMERATE) * 1000 - execute_time);
        }

        // Temp: Simulate no buttons pressed
        write_byte(gbc->ram, 0xFF00, 0xEF, 0);
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}

// Initialises the system
static void init_system(gbc_system *gbc, cmd_options *cmd) {

    gbc->cpu = malloc(sizeof(gbc_cpu));
    gbc->ppu = malloc(sizeof(gbc_ppu));
    gbc->ram = malloc(sizeof(gbc_ram));
    gbc->rom = malloc(sizeof(gbc_rom));
    gbc->debugger = malloc(sizeof(gbc_debugger));

    init_cpu(gbc->cpu);
    init_ram(gbc->ram);
    init_ppu(gbc->ppu, cmd->scale);
    init_debugger(gbc->debugger);

    if(!load_rom(gbc, cmd->rom_path)) {
        fprintf(stderr, "ERROR: Could not load rom file!\n");
        exit(EXIT_FAILURE);
    }

    // Append the game title to the window title
    char title[TITLE_LENGTH + strlen(MAIN_WINDOW_TITLE " - ") + 1];
    snprintf(title, TITLE_LENGTH + 1, "%s - %s", MAIN_WINDOW_TITLE, gbc->rom->title);
    title[strlen(title)] = '\0';

    SDL_SetWindowTitle(gbc->ppu->window, title);

    if(cmd->debug) {
        printf(CGRN LOGO CNRM);
        print_rom_info(gbc->rom);
        printf("\nLXGBC DEBUGGER RUNNING\nType 'h' for information on the available commands.\n");
        printf(DEBUG_PROMPT);

        gbc->debugger->is_debugging = true;
    }

    gbc->is_running = true;
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

                if(gbc->debugger->is_debugging == false) {
                    gbc->debugger->is_debugging = true;
                    printf(DEBUG_PROMPT);
                }
            }

            break;
    }
}

// Populates the cmd_options struct with the selected options from the command line
static bool get_cl_arguments(int argc, char **argv, cmd_options *cmd) {

    int option;
    int f_flag, s_flag = 0;

    cmd->debug = false;
    cmd->no_limit = false;

    while((option = getopt(argc, argv, "f:gs:n")) != -1) {
        switch (option) {
            case 'f':
                cmd->rom_path = optarg;
                f_flag = 1;
                break;
            
            case 'g':
                cmd->debug = true;
                break;

            case 's':
                cmd->scale = atoi(optarg);
                s_flag = 1;
                break;

            case 'n':
                cmd->no_limit = true;
                break;

            default: 
                printf("Usage: -f rom [-g debug mode] [-s screen scale] [-n no speed limit]\n"); 
                return 0;
        }
    }

    if(!f_flag) {
        printf("%s: missing -f option\n", argv[0]);
        return false;
    }

    if(!s_flag) {
        cmd->scale = 1;
    }

    return true;
}
