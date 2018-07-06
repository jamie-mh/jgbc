#pragma once

#define BG_COLOUR 0.45f, 0.55f, 0.60f, 1.00f

typedef struct gbc_debugger {
    SDL_Window *window;
    SDL_GLContext gl_context;
} gbc_debugger;