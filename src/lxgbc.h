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

typedef struct gbc_registers {

    // Combined register idea thanks to
    // https://cturt.github.io/cinoop.html
    union {
        struct {
            unsigned char F;
            unsigned char A;
        };
        unsigned short AF;
    };

    union {
        struct {
            unsigned char C;
            unsigned char B;
        };
        unsigned short BC;
    };

    union {
        struct {
            unsigned char E;
            unsigned char D;
        };
        unsigned short DE;
    };

    union {
        struct {
            unsigned char L;
            unsigned char H;
        };
        unsigned short HL;
    };

    unsigned short PC; // Program Counter
    unsigned short SP; // Stack Pointer
    unsigned char IME; // Interrupt Master Enable
} gbc_registers;

typedef struct gbc_cpu {
    gbc_registers *registers;
    unsigned char clock;
} gbc_cpu;

typedef struct gbc_ppu {
    SDL_Window *window;
    SDL_Renderer *renderer;
    unsigned char clock;
} gbc_ppu;

typedef struct gbc_ram {
    unsigned char *rom00; // 16KB ROM Bank
    unsigned char *romNN; // 16KB Switchable ROM Bank
    unsigned char *vram; // 8KB Video RAM
    unsigned char *extram; // 8KB External Ram (cartridge)
    unsigned char *wram00; // 4KB Work RAM bank 0
    unsigned char *wramNN; // 4KB Work RAM bank 1-7 (switchable) 
    unsigned char *oam; // 1.59KB Sprite Attribute Table
    unsigned char *io; // 128B IO Ports
    unsigned char *hram; // 128B High RAM
    unsigned char *ier; // 1B Interrupt Enable Register

    unsigned char **wram_banks; // 8x4KB WRAM Banks (CGB Only)
} gbc_ram;

typedef struct gbc_rom {
    char *title; // Uppercase ASCII Game Name
    char cgb_flag; // Color Support Flag
    unsigned char cart_type; // Cartridge Type Code
    unsigned char rom_size; // Number of ROM banks
    unsigned char ram_size; // Number of EXT RAM banks
    unsigned char dest_code; // Destination Code (0: Japan, 1: World)
    unsigned char ver_no; // ROM Version Number

    unsigned char **rom_banks;
    unsigned char **ram_banks;
} gbc_rom;

typedef struct gbc_breakpoint {
    unsigned short address;
    struct gbc_breakpoint *next;
} gbc_breakpoint;

typedef struct gbc_debugger {
    gbc_breakpoint *breakpoint_head;
    char skip_instr;
    char running;
    char print;
} gbc_debugger;

typedef struct gbc_system {
    char is_running;
    gbc_cpu *cpu;
    gbc_ppu *ppu;
    gbc_ram *ram;
    gbc_rom *rom;
    gbc_debugger *debugger;
} gbc_system;

typedef struct cmd_options {
    char *rom_path;
    char debug;
    char scale;
} cmd_options;

static char get_cl_arguments(int, char **, cmd_options *); 
