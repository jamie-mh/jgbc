#pragma once

#define BG_COLOUR 0.45f, 0.55f, 0.60f, 1.00f

#define DEBUGGER_WIDTH 1280
#define DEBUGGER_HEIGHT 720
#define DEBUGGER_SCALE 2


typedef struct gbc_breakpoint {
    uint16_t address;
    struct gbc_breakpoint *next;
} gbc_breakpoint;

typedef struct gbc_debugger {
    SDL_Window *window;
    SDL_GLContext gl_context;

    uint16_t next_addr;
    bool is_paused;
    bool modal_rom_open;
    
    gbc_breakpoint *breakpoint_head;
    uint64_t clocks;
} gbc_debugger;


bool add_breakpoint(const uint16_t, gbc_debugger *);
bool remove_breakpoint(const uint16_t, gbc_debugger *);
gbc_breakpoint *find_breakpoint(const uint16_t, gbc_debugger *);