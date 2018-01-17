#include "lxgbc.h"
#include "cpu.h"
#include "ram.h"
#include "rom.h"
#include "debugger.h"
#include "display.h"

int main(int argc, char **argv) {

    printf(LOGO);

    // Allocate memory for the gbc
    struct gbc_system *gbc = malloc(sizeof(*gbc));
    gbc->ram = malloc(sizeof(struct gbc_ram));
    struct gbc_rom *rom = malloc(sizeof(*rom));
    struct gbc_display *display = malloc(sizeof(*display));
    struct gbc_debugger *debugger = NULL;

    // Get the command line arguments
    struct cmd_options *cmd = calloc(1, sizeof(*cmd));
    cmd->rom_path = malloc(sizeof(char) * ROM_PATH_LENGTH);

    if(!get_cl_arguments(argc, argv, cmd)) {
        exit(0);
    }

    // Initialize the system
    init_cpu(gbc);
    init_ram(gbc->ram);
    //init_display(&display);

    // TODO: Remove temporary 
    gbc->is_running = 1;
    gbc->interrupts_enabled = 1;

    // Load the rom into memory
    if(!load_rom(gbc->ram, rom, cmd->rom_path)) {
        printf("ERROR: Could not load rom file!\n\n");
        exit(0);
    }

    print_rom_info(rom); 
    
    // Show a message and initialise the debugger 
    if(cmd->debug) {
        printf("\nLXGBC "CRED"DEBUGGER RUNNING"CNRM"\nType 'h' for information on the available commands.\n");
        debugger = malloc(sizeof(*debugger));
        init_debugger(debugger);
    }

    // Main endless loop 
    while(gbc->is_running) {

        // Execute the instruction at the program counter 
        execute_instr(gbc);

        // Run the debugger if specified
        if(cmd->debug) debug(gbc, debugger);
    }

    return 0;
}

static char get_cl_arguments(int argc, char **argv, struct cmd_options *cmd) {

    int option;
    int f_flag = 0;

    while((option = getopt(argc, argv, "f:d")) != -1) {
        switch (option) {
            case 'f':
                cmd->rom_path = optarg;
                f_flag = 1;
                break;
            
            case 'd':
                cmd->debug = 1;
                break;

            default: 
                printf("Usage: -f rom [-d debug mode]\n"); 
                return 0;
        }
    }

    if(!f_flag) {
        printf("%s: missing -f option\n", argv[0]);
        return 0;
    }

    return 1;
}
