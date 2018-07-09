#pragma once

#define JOYP 0xFF00
#define DEFAULT_JOYP 0x1F

#define JOYP_BTN 0x20 
#define JOYP_DIR 0x10 
#define KEY_DOWN_START 0x8 
#define KEY_UP_SELECT 0x4 
#define KEY_LEFT_B 0x2 
#define KEY_RIGHT_A 0x1 

#define KEY_STATE (key.state == SDL_PRESSED) ? true : false
#define SET_KEY(mask, value, input) ((value) ? ((input) ^= (mask)) : ((input) |= (mask))) 

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
      (byte & 0x80 ? '1' : '0'), \
        (byte & 0x40 ? '1' : '0'), \
          (byte & 0x20 ? '1' : '0'), \
            (byte & 0x10 ? '1' : '0'), \
              (byte & 0x08 ? '1' : '0'), \
                (byte & 0x04 ? '1' : '0'), \
                  (byte & 0x02 ? '1' : '0'), \
                    (byte & 0x01 ? '1' : '0') 

void init_input(gbc_input *);
void handle_input(gbc_input *, SDL_KeyboardEvent);
unsigned char joypad_state(gbc_system *);
