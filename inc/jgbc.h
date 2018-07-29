#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define GET_BIT(data, bit) (((data) >> (bit)) & 1)

typedef struct Registers Registers;
typedef struct CPU CPU;
typedef struct Sprite Sprite;
typedef struct Display Display;
typedef struct PPU PPU;
typedef struct MMU MMU;
typedef struct APU APU;
typedef struct Cart Cart;
typedef struct Input Input;
typedef struct GameBoy GameBoy;


typedef struct Registers {
    union {
        struct {
            uint8_t F;
            uint8_t A;
        };
        uint16_t AF;
    };

    union {
        struct {
            uint8_t C;
            uint8_t B;
        };
        uint16_t BC;
    };

    union {
        struct {
            uint8_t E;
            uint8_t D;
        };
        uint16_t DE;
    };

    union {
        struct {
            uint8_t L;
            uint8_t H;
        };
        uint16_t HL;
    };

    uint16_t PC;
    uint16_t SP;
    bool IME;
}
Registers;

typedef struct CPU {
    bool is_halted;
    Registers reg;
    uint8_t ticks;
    uint16_t div_clock; // Divider Timer Clock
    uint16_t cnt_clock; // Timer Counter Clock
}
CPU;

typedef struct Sprite {
    uint8_t y;
    uint8_t x;
    uint8_t tile;
    uint8_t attributes;
}
Sprite;

typedef struct PPU {
    uint8_t *framebuffer;
    Sprite sprite_buffer[40];
    uint16_t scan_clock;
    uint16_t frame_clock;

    void (*render_fn)(GameBoy *);
    Display *display;
}
PPU;

typedef struct MMU {
    uint8_t *ram;
    uint8_t rom_bank;
    uint8_t ram_bank;
    uint8_t wram_bank;
    uint8_t vram_bank;

    uint8_t **vram_banks;
    uint8_t **wram_banks;
}
MMU;

typedef struct APU {
    bool enabled;
}
APU;

typedef struct Cart {
    char title[17]; // Uppercase ASCII Game Name
    uint8_t gbc_flag; // Color Support Flag
    uint8_t type;
    uint8_t mbc_type;
    uint8_t rom_size;
    uint8_t ram_size;

    uint8_t **rom_banks;
    uint8_t **ram_banks;
}
Cart;

typedef struct Input {
    bool up;
    bool right;
    bool down;
    bool left;
    bool start;
    bool select;
    bool a;
    bool b;
}
Input;

typedef struct GameBoy {
    bool is_running;
    void (*event_fn)(GameBoy *);

    CPU cpu;
    PPU ppu;
    MMU mmu;
    APU apu;
    Cart cart;
    Input input;
}
GameBoy;

void init(GameBoy *gb, void (*event_fn)(GameBoy *));
void run(GameBoy *gb);