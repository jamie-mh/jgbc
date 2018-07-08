#pragma once

#define BG_COLOUR 0.45f, 0.55f, 0.60f, 1.00f

#define DEBUGGER_WIDTH 1280
#define DEBUGGER_HEIGHT 720
#define DEBUGGER_SCALE 2


typedef struct gbc_breakpoint {
    unsigned int address;
    struct gbc_breakpoint *next;
} gbc_breakpoint;

typedef struct gbc_debugger {
    SDL_Window *window;
    SDL_GLContext gl_context;

    unsigned short next_addr;

    bool is_paused;
    bool modal_rom_open;
    
    gbc_breakpoint *breakpoint_head;
    unsigned long long clocks;
} gbc_debugger;


bool add_breakpoint(const unsigned int, gbc_debugger *);
bool remove_breakpoint(const unsigned int, gbc_debugger *);
gbc_breakpoint *find_breakpoint(const unsigned int, gbc_debugger *);
