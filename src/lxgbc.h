#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<unistd.h>
#include<SDL2/SDL.h>

#ifndef max
	#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
	#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define ROM_PATH_LENGTH 256
#define LOGO "    __   _  ____________  ______\n   / /  | |/ / ____/ __ )/ ____/\n  / /   |   / / __/ __  / /    \n / /___/   / /_/ / /_/ / /___  \n/_____/_/|_\\____/_____/\\____/   \n                                \n"

// GameBoy Specification
struct gbc_system {
    char is_running;
    char interrupts_enabled;
    struct gbc_registers *registers;
    struct gbc_ram *ram;
};

struct cmd_options {
    char *rom_path;
    char debug;
};

static char get_cl_arguments(int, char **, struct cmd_options *); 
