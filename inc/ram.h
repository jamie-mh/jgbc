#pragma once

#define VRAM_SIZE 8192
#define EXTRAM_BANK_SIZE 8192
#define WRAM_BANK_SIZE 4096
#define OAM_SIZE 160
#define IO_SIZE 128
#define HRAM_SIZE 128

#define WRAM_BANK_COUNT 8

// Sound Registers
// TODO: Implement


// GBC Only Registers
#define KEY1 0xFF4D
#define SVBK 0xFF70

// Serial
#define SB 0xFF01


void init_ram(gbc_system *);

bool is_valid_ram(gbc_ram *, const unsigned short);

unsigned char read_byte(gbc_ram *, const unsigned short);
unsigned short read_short(gbc_ram *, const unsigned short);

void write_byte(gbc_system *, const unsigned short, unsigned char, const bool);
void write_short(gbc_system *, const unsigned short, const unsigned short);

void write_register(gbc_system *, const unsigned short, const unsigned char, const unsigned char);
unsigned char read_register(gbc_ram *, const unsigned short, const unsigned char);
