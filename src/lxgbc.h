#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<unistd.h>
#include<SDL2/SDL.h>

#define ROM_PATH_LENGTH 256
#define LOGO "    __   _  ____________  ______\n   / /  | |/ / ____/ __ )/ ____/\n  / /   |   / / __/ __  / /    \n / /___/   / /_/ / /_/ / /___  \n/_____/_/|_\\____/_____/\\____/   \n                                \n"

#define CNRM "\x1B[0m"
#define CRED "\x1B[31m"
#define CGRN "\x1B[32m"
#define CYEL "\x1B[33m"
#define CBLU "\x1B[34m"
#define CMAG "\x1B[35m"
#define CCYN "\x1B[36m"
#define CWHT "\x1B[37m"

// GameBoy Specification
struct gbc_system {
    char is_running;
    struct gbc_registers *registers;
    struct gbc_ram *ram;
};

struct cmd_options {
    char *rom_path;
    char debug;
};

static char get_cl_arguments(int, char **, struct cmd_options *); 
