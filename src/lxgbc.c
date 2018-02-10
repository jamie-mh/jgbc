#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "mbc.h"
#include "rom.h"
#include "debugger.h"
#include "gpu.h"
#include "sound.h"

int main(int argc, char **argv) {

    printf(CGRN LOGO CNRM);

    // Allocate memory for the gbc
    gbc_system *gbc = malloc(sizeof(gbc_system));
    gbc->cpu = malloc(sizeof(gbc_cpu));
    gbc->gpu = malloc(sizeof(gbc_gpu));
    gbc->ram = malloc(sizeof(gbc_ram));
    gbc->rom = malloc(sizeof(gbc_rom));
    gbc->debugger = malloc(sizeof(gbc_debugger));

    gbc->is_running = 1;

    // Get the command line arguments
    cmd_options *cmd = malloc(sizeof(cmd_options));
    cmd->rom_path = malloc(sizeof(char) * ROM_PATH_LENGTH);

    if(!get_cl_arguments(argc, argv, cmd)) {
        exit(0);
    }

    // Initialize the system
    init_cpu(gbc->cpu);
    init_ram(gbc->ram);

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
    } else {
        
        // If the debugger is not running, show the display
        init_gpu(gbc->gpu, cmd->scale);
    }

    // Main endless loop 
    // One execution of this loop represents one cpu clock in non debug mode
    while(gbc->is_running) {

        // In debug mode, execute instruction by instruction 
        if(cmd->debug) {
            debug(gbc);

            // Execute the instruction at the program counter 
            execute_instr(gbc);
    
            // Simulate scan line rendering of the gpu
            simulate_gpu(gbc);
        }
        // In normal mode, do a cpu and gpu clock
        else {
            cpu_do_clock(gbc);
            gpu_do_clock(gbc);
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
