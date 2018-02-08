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
    gbc->ram = malloc(sizeof(gbc_ram));
    gbc_rom *rom = malloc(sizeof(gbc_rom));
    gbc_gpu *gpu = malloc(sizeof(gbc_gpu));
    gbc_debugger *debugger = malloc(sizeof(gbc_debugger));

    // Get the command line arguments
    cmd_options *cmd = malloc(sizeof(cmd_options));
    cmd->rom_path = malloc(sizeof(char) * ROM_PATH_LENGTH);

    if(!get_cl_arguments(argc, argv, cmd)) {
        exit(0);
    }

    // Initialize the system
    init_cpu(gbc);
    init_ram(gbc->ram);
    //init_gpu(gpu, cmd->scale);

    gbc->is_running = 1;

    // Load the rom into memory
    if(!load_rom(gbc->ram, rom, cmd->rom_path)) {
        printf("ERROR: Could not load rom file!\n\n");
        exit(0);
    }

    print_rom_info(rom);
    
    // Show a message and initialise the debugger 
    if(cmd->debug) {
        printf("\nLXGBC DEBUGGER RUNNING\nType 'h' for information on the available commands.\n");
        init_debugger(debugger);
    }

    // Main endless loop 
    while(gbc->is_running) {

        // Execute the instruction at the program counter 
        execute_instr(gbc);

        // Run the debugger if specified
        if(cmd->debug) debug(gbc, debugger);

        // Temp
        render(gbc->ram, gpu);
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
