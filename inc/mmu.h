#pragma once

// Macro Shortcuts
#define SREAD8(addr) read_byte(gb, (addr), false)
#define SWRITE8(addr, value) write_byte(gb, (addr), (value), false)
#define SREAD16(addr) read_short(gb, (addr), false)
#define SWRITE16(addr, value) write_short(gb, (addr), (value), false)

// Region Sizes
#define ROM_BANK_SIZE 16384
#define VRAM_BANK_SIZE 8192
#define EXTRAM_BANK_SIZE 8192
#define WRAM_BANK_SIZE 4096
#define OAM_SIZE 160
#define IO_SIZE 128
#define HRAM_SIZE 128

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

// GBC Banks
#define WRAM_BANK_COUNT 8
#define VRAM_BANK_COUNT 2


void init_mmu(GameBoy *gb);

uint8_t read_byte(GameBoy *gb, uint16_t address, const bool is_program);
uint16_t read_short(GameBoy *gb, const uint16_t address, const bool is_program);

void write_byte(GameBoy *gb, uint16_t address, uint8_t value, const bool is_program);
void write_short(GameBoy *gb, const uint16_t address, const uint16_t value, const bool is_program);

void write_register(GameBoy *gb, const uint16_t address, const uint8_t bit, const uint8_t value);
uint8_t read_register(GameBoy *, const uint16_t address, const uint8_t bit);