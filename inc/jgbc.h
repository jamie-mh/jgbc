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

typedef enum EnvelopeMode {
    Decrease = 0,
    Increase = 1
}
EnvelopeMode;

typedef struct ChannelEnvelope {
    uint8_t initial_volume;
    uint8_t current_volume;
    EnvelopeMode mode;
    uint8_t period;
}
ChannelEnvelope;

typedef struct ChannelLength {
    bool enabled;
    uint16_t clock;
}
ChannelLength;

typedef enum SweepMode {
    Addition = 0,
    Subtraction = 1
}
SweepMode;

typedef struct SquareWave {
    bool enabled;
    bool dac_enabled;

    struct {
        bool enabled;
        uint8_t period;
        SweepMode mode;
        uint8_t shift;
        uint16_t current_frequency;
    }
    sweep;

    struct {
        uint8_t mode;
        uint8_t step;
    }
    duty;

    ChannelEnvelope envelope;
    ChannelLength length;

    int32_t clock;
    uint16_t frequency;
}
SquareWave;

typedef struct Wave {
    bool enabled;
    uint8_t volume_code;
    ChannelLength length;

    uint8_t position;
    uint32_t clock;
    uint16_t frequency;
}
Wave;

typedef struct Noise {
    bool enabled;
    ChannelLength length;
    ChannelEnvelope envelope;
    uint8_t clock_shift;
    uint8_t width_mode;
    uint8_t divisor_code;
    uint16_t lfsr;
    uint32_t clock;
    uint8_t last_result;
}
Noise;

typedef struct APU {
    bool enabled;
    SDL_AudioDeviceID device_id;
    SDL_AudioSpec desired_spec;
    SDL_AudioSpec actual_spec;

    float *buffer;
    uint32_t buffer_position;

    struct {
        uint8_t step;
        uint16_t clock;
    }
    frame_sequencer;

    uint8_t downsample_clock;

    uint8_t channels[4];
    bool left_enabled[4];
    bool right_enabled[4];

    SquareWave square_waves[2];
    Wave wave;
    Noise noise;
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
