#include "jgbc.h"
#include "input.h"
#include "mmu.h"
#include "cpu.h"


void init_input(GameBoy *gb) {
    gb->input.up = false;
    gb->input.right = false;
    gb->input.down = false;
    gb->input.left = false;
    gb->input.start = false;
    gb->input.select = false;
    gb->input.a = false;
    gb->input.b = false;
}

uint8_t joypad_state(GameBoy *gb) {
    uint8_t joypad = read_byte(gb, JOYP, false);

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
    
    write_byte(gb, JOYP, joypad, false);
    return joypad;
}