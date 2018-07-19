#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <SDL.h>

#define ROM_PATH_LENGTH 256
#define GET_BIT(val, i) (((val) >> (i)) & 1)


typedef struct gbc_registers {

    // Combined register idea thanks to
    // https://cturt.github.io/cinoop.html
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

    uint16_t PC; // Program Counter
    uint16_t SP; // Stack Pointer
    bool IME; // Interrupt Master Enable
} gbc_registers;

typedef struct gbc_cpu {
    gbc_registers *registers;
    uint16_t div_clock; // Divider Timer Clock
    uint16_t cnt_clock; // Timer Counter Clock
    bool is_halted;
} gbc_cpu;

typedef struct gbc_sprite {
    uint8_t y;
    uint8_t x;
    uint8_t tile;
    uint8_t attributes;
} gbc_sprite;

typedef struct gbc_ppu {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    uint8_t *framebuffer;
    uint16_t scan_clock;
    uint16_t frame_clock;
    gbc_sprite *sprite_buffer; // Only fetch the sprites once per frame    
} gbc_ppu;

typedef struct gbc_ram {
    uint8_t *bootrom;
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

    uint8_t **wram_banks; // 8x4KB WRAM Banks (CGB Only)
} gbc_ram;

typedef struct gbc_cart {
    char *title; // Uppercase ASCII Game Name
    uint8_t cgb_flag; // Color Support Flag
    uint8_t type; // Cartridge Type Code
    uint8_t mbc_type; // MBC Type (1, 2, 3)
    uint8_t rom_size; // Number of ROM banks
    uint8_t ram_size; // Number of EXT RAM banks
    uint8_t dest_code; // Destination Code (0: Japan, 1: World)
    uint8_t ver_no; // ROM Version Number

    uint8_t curr_rom_bank;
    uint8_t curr_ram_bank;

    uint8_t **rom_banks;
    uint8_t **ram_banks;
} gbc_cart;

typedef struct gbc_input {
    bool up;
    bool right;
    bool down;
    bool left;
    bool start;
    bool select;
    bool a;
    bool b;
} gbc_input;

typedef struct gbc_system {
    bool is_running;
    uint8_t clocks;

    gbc_cpu *cpu;
    gbc_ppu *ppu;
    gbc_ram *ram;
    gbc_cart *cart;
    gbc_input *input;
} gbc_system;