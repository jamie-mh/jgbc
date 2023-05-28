#pragma once

#include "gameboy.h"

#define JOYP 0xFF00

// Joypad Button Bits
#define JOYP_BTN 0x20
#define JOYP_DIR 0x10
#define KEY_DOWN_START 0x8
#define KEY_UP_SELECT 0x4
#define KEY_LEFT_B 0x2
#define KEY_RIGHT_A 0x1

// Shortcut Macros
#define KEY_STATE (key.state == SDL_PRESSED) ? true : false
#define SET_KEY(mask, value, input) ((value) ? ((input) ^= (mask)) : ((input) |= (mask)))

void reset_input(GameBoy *);
void set_key(GameBoy *, SDL_Scancode, bool);
uint8_t joypad_state(GameBoy *);
