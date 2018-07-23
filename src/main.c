#include <SDL.h>

#include "jgbc.h"
#include "main.h"
#include "ppu.h"
#include "cpu.h"
#include "mmu.h"
#include "cart.h"
#include "input.h"

static void init_window(Display *display);
static void set_key(GameBoy *gb, const SDL_Scancode code, const bool is_pressed);
static void handle_event(GameBoy *gb, SDL_Event event);
static void set_window_title(GameBoy *gb, Display *display);


int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: jgbc.exe <path to rom>\n");
        return EXIT_FAILURE; 
    }

    GameBoy *gb = malloc(sizeof(GameBoy));
    init(gb, &check_event);

    if(!load_rom(gb, argv[1])) {
        fprintf(stderr, "ERROR: Cannot load rom file\n");
        return EXIT_FAILURE;
    }

    SDL_Init(SDL_INIT_VIDEO);
    Display *display = malloc(sizeof(Display));
    init_window(display);
    use_external_renderer(gb, &render, display);

    print_cart_info(gb);
    set_window_title(gb, display);

    render(gb);
    run(gb);

    SDL_Quit();
    return EXIT_SUCCESS;
}

static void init_window(Display *display) {
    display->window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH * SCREEN_SCALE,
        SCREEN_HEIGHT * SCREEN_SCALE,
        SDL_WINDOW_SHOWN
    );

    display->renderer = SDL_CreateRenderer(
        display->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

	SDL_DisplayMode mode;
	mode.refresh_rate = FRAMERATE;
	SDL_SetWindowDisplayMode(display->window, &mode);

    display->texture = SDL_CreateTexture(
        display->renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    SDL_RenderSetScale(display->renderer, SCREEN_SCALE, SCREEN_SCALE);
}

void render(GameBoy *gb) {

    SDL_UpdateTexture(
        gb->ppu.display->texture,
        NULL,
        gb->ppu.framebuffer,
        SCREEN_WIDTH * 3 
    );

    SDL_RenderCopy(gb->ppu.display->renderer, gb->ppu.display->texture, NULL, NULL);
    SDL_RenderPresent(gb->ppu.display->renderer);
}

void check_event(GameBoy *gb) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        handle_event(gb, event);
    }
}

static void handle_event(GameBoy *gb, SDL_Event event) {

    switch(event.type) {
        case SDL_QUIT:
            gb->is_running = false;
            break;

        case SDL_KEYDOWN:
            set_key(gb, event.key.keysym.scancode, true);
            break;

        case SDL_KEYUP:
            set_key(gb, event.key.keysym.scancode, false);
            break;
    }
}

static void set_key(GameBoy *gb, const SDL_Scancode code, const bool is_pressed) {

    switch(code) {
        case SDL_SCANCODE_RETURN:
            gb->input.start = is_pressed; break;

        case SDL_SCANCODE_BACKSPACE:
            gb->input.select = is_pressed; break;

        case SDL_SCANCODE_A:
            gb->input.a = is_pressed; break;

        case SDL_SCANCODE_S:
            gb->input.b = is_pressed; break;

        case SDL_SCANCODE_UP:
            gb->input.up = is_pressed; break;

        case SDL_SCANCODE_RIGHT:
            gb->input.right = is_pressed; break;

        case SDL_SCANCODE_DOWN:
            gb->input.down = is_pressed; break;

        case SDL_SCANCODE_LEFT:
            gb->input.left = is_pressed; break;

        default: break;
    }
}

static void set_window_title(GameBoy *gb, Display *display) {
    char buffer[30];
    snprintf(buffer, 30, "%s - %s", WINDOW_TITLE, gb->cart.title);
    SDL_SetWindowTitle(display->window, buffer);
}