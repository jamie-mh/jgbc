#include "lxgbc.h"
#include "input.h"
#include "ram.h"
#include "cpu.h"

// Initialises the input struct
void init_input(gbc_input *input) {
    input->up = false;
    input->right = false;
    input->down = false;
    input->left = false;
    input->start = false;
    input->select = false;
    input->a = false;
    input->b = false;
}

// Updates the input struct with the value of the current button
void handle_input(gbc_input *input, SDL_KeyboardEvent key) {

    switch(key.keysym.scancode) {

        case SDL_SCANCODE_RETURN:
            input->start = KEY_STATE;
            break;

        case SDL_SCANCODE_BACKSPACE:
            input->select = KEY_STATE;
            break;

        case SDL_SCANCODE_A:
            input->a = KEY_STATE;
            break;

        case SDL_SCANCODE_S:
            input->b = KEY_STATE;
            break;

        case SDL_SCANCODE_UP:
            input->up = KEY_STATE;
            break;

        case SDL_SCANCODE_RIGHT:
            input->right = KEY_STATE;
            break;

        case SDL_SCANCODE_DOWN:
            input->down = KEY_STATE;
            break;

        case SDL_SCANCODE_LEFT:
            input->left = KEY_STATE;
            break;

        default: break; // gcc warning bypass 
    }
}

// Intercepts the joypad register reads and returns the current values
// Everything is inversed, this is how the games read it
unsigned char joypad_state(gbc_system *gbc) {
    unsigned char joypad = read_byte(gbc, JOYP, false);

    if((joypad & JOYP_DIR) == 0) {
        SET_KEY(KEY_UP_SELECT, gbc->input->up, joypad);
        SET_KEY(KEY_RIGHT_A, gbc->input->right, joypad);
        SET_KEY(KEY_DOWN_START, gbc->input->down, joypad);
        SET_KEY(KEY_LEFT_B, gbc->input->left, joypad);

    } else if((joypad & JOYP_BTN) == 0) {
        SET_KEY(KEY_UP_SELECT, gbc->input->select, joypad);
        SET_KEY(KEY_RIGHT_A, gbc->input->a, joypad);
        SET_KEY(KEY_DOWN_START, gbc->input->start, joypad);
        SET_KEY(KEY_LEFT_B, gbc->input->b, joypad);
    }
    
    write_byte(gbc, JOYP, joypad, false);
    return joypad;
}
