#pragma once

#define SCREEN_SCALE 4
#define WINDOW_TITLE "jgbc"

typedef struct Display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
}
Display;

void render(GameBoy *gb);
void check_event(GameBoy *gb);