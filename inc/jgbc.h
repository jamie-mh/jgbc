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
#include <SDL.h>

#define GET_BIT(data, bit) (((data) >> (bit)) & 1)

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

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
}
PPU;

typedef struct MMU {
    uint8_t rom_bank;
    uint8_t ram_bank;
    uint8_t wram_bank;
    uint8_t vram_bank;

    uint8_t *rom00; // 16KB ROM Bank
    uint8_t *romNN; // 16KB Switchable ROM Bank
    uint8_t *vram; // 8KB Video RAM
    uint8_t *extram; // 8KB External Ram (cartridge)
    uint8_t *wram00; // 4KB Work RAM bank 0
    uint8_t *wramNN; // 4KB Work RAM bank 1-7 (switchable) 
    uint8_t *oam; // 1.59KB Sprite Attribute Table
    uint8_t *io; // 128B IO Ports
    uint8_t *hram; // 128B High RAM
    uint8_t *ier; // 1B Interrupt Enable Register

    uint8_t **wram_banks; // 8x4KB WRAM Banks (GBC Only)
    uint8_t **vram_banks; // 2x8KB VRAM Banks (GBC Only)
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

    CPU cpu;
    PPU ppu;
    MMU mmu;
    APU apu;
    Cart cart;
    Input input;
}
GameBoy;

void init(GameBoy *gb);