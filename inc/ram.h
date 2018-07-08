#pragma once

#define ROM_BANK_SIZE 16384
#define VRAM_SIZE 8192
#define EXTRAM_BANK_SIZE 8192
#define WRAM_BANK_SIZE 4096
#define OAM_SIZE 160
#define IO_SIZE 128
#define HRAM_SIZE 128

#define WRAM_BANK_COUNT 8

// typedef unsigned short ADDRESS;
// typedef unsigned char BYTE;

// Addressable Regions
#define ROM00_START 0X0
#define ROM00_END 0x3FFF
#define ROMNN_START 0x4000
#define ROMNN_END 0x7FFF
#define VRAM_START 0x8000
#define VRAM_END 0x9FFF
#define EXTRAM_START 0xA000
#define EXTRAM_END 0xBFFF
#define WRAM00_START 0xC000
#define WRAM00_END 0xCFFF
#define WRAMNN_START 0xD000
#define WRAMNN_END 0xDFFF
#define WRAM00_MIRROR_START 0xE000
#define WRAM00_MIRROR_END 0xEFFF
#define WRAMNN_MIRROR_START 0xF000
#define WRAMNN_MIRROR_END 0xFDFF
#define OAM_START 0xFE00
#define OAM_END 0xFE9F
#define UNUSABLE_START 0xFEA0
#define UNUSABLE_END 0xFEFF
#define IO_START 0xFF00
#define IO_END 0xFF7F
#define HRAM_START 0xFF80
#define HRAM_END 0xFFFE
#define IE_START_END 0xFFFF

// Sound Registers
// TODO: Implement


// GBC Only Registers
#define KEY1 0xFF4D
#define SVBK 0xFF70

// Serial
#define SB 0xFF01
#define BOOTROM_DISABLE 0xFF50


void init_ram(gbc_system *);

bool is_valid_ram(gbc_ram *, const unsigned short);

unsigned char read_byte(gbc_ram *, const unsigned short);
unsigned short read_short(gbc_ram *, const unsigned short);

void write_byte(gbc_system *, const unsigned short, unsigned char, const bool);
void write_short(gbc_system *, const unsigned short, const unsigned short);

void write_register(gbc_system *, const unsigned short, const unsigned char, const unsigned char);
unsigned char read_register(gbc_ram *, const unsigned short, const unsigned char);
