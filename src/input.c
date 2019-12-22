#include "jgbc.h"
#include "input.h"
#include "mmu.h"


void reset_input(GameBoy *gb) {
    gb->input.up = false;
    gb->input.right = false;
    gb->input.down = false;
    gb->input.left = false;
    gb->input.start = false;
    gb->input.select = false;
    gb->input.a = false;
    gb->input.b = false;
}

void set_key(GameBoy *gb, const SDL_Scancode code, const bool is_pressed) {

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

uint8_t joypad_state(GameBoy *gb) {
    uint8_t joypad = SREAD8(JOYP);

    if((joypad & JOYP_DIR) == 0) {
        SET_KEY(KEY_UP_SELECT, gb->input.up, joypad);
        SET_KEY(KEY_RIGHT_A, gb->input.right, joypad);
        SET_KEY(KEY_DOWN_START, gb->input.down, joypad);
        SET_KEY(KEY_LEFT_B, gb->input.left, joypad);

    } else if((joypad & JOYP_BTN) == 0) {
        SET_KEY(KEY_UP_SELECT, gb->input.select, joypad);
        SET_KEY(KEY_RIGHT_A, gb->input.a, joypad);
        SET_KEY(KEY_DOWN_START, gb->input.start, joypad);
        SET_KEY(KEY_LEFT_B, gb->input.b, joypad);
    }
    
    SWRITE8(JOYP, joypad);
    return joypad;
}
