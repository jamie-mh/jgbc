#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_SCALE 6.0
#define MAIN_WINDOW_TITLE "LXGBC"

struct gbc_display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event *event;
};

void init_display(struct gbc_display **);