#include "jgbc.h"
#include "ppu.h"
#include "cpu.h"
#include "mmu.h"
#include "mbc.h"
#include "input.h"

static uint8_t *get_memory(GameBoy *gb, uint16_t *address);
static bool is_accessible(GameBoy *gb, const uint16_t address);
static void DMA_transfer(GameBoy *gb, const uint8_t value);


void init_mmu(GameBoy *gb) {
    gb->mmu.ram = malloc(RAM_SIZE);
    gb->mmu.ram_bank = 0;
    gb->mmu.rom_bank = 1;
}

static uint8_t *get_memory(GameBoy *gb, uint16_t *address) {

    if(*address <= ROM00_END) {
        return gb->cart.rom_banks[0];
    }
    else if(*address >= ROMNN_START && *address <= ROMNN_END) {
        *address -= ROMNN_START;
        return gb->cart.rom_banks[gb->mmu.rom_bank];
    }

    return gb->mmu.ram;
}

static bool is_accessible(GameBoy *gb, const uint16_t address) {

    //if(address >= UNUSABLE_START && address <= UNUSABLE_END) {
    //    return false;
    //}

    if(address >= EXTRAM_START && address <= EXTRAM_END && gb->mmu.ram_bank == -1) {
        return false;
    }

    return true;
}

uint8_t read_byte(GameBoy *gb, uint16_t address, const bool is_program) {

    const uint16_t old = address;
    if(is_program && address == JOYP) {
        return joypad_state(gb);
    }

    if(!is_accessible(gb, address)) {
        return 0xFF;
    }
    
    uint8_t *mem = get_memory(gb, &address);
    return mem[address];
}

uint16_t read_short(GameBoy *gb, const uint16_t address, const bool is_program) {
    const uint8_t byte_a = read_byte(gb, address, is_program);
    const uint8_t byte_b = read_byte(gb, address + 1, is_program);

    return byte_b << 8 | byte_a;
}

void write_byte(GameBoy *gb, uint16_t address, uint8_t value, const bool is_program) {

    if(address <= ROMNN_END) {
        mbc_check(gb, address, value);
        return;
    }

    if(is_program && address == DIV) {
        gb->cpu.div_clock = 0;
        gb->cpu.cnt_clock = 0;

        value = 0x0;
		return;
    }

    if(!is_accessible(gb, address)) {
        return;
    }

    if(address == DMA) {
        DMA_transfer(gb, value); 
        return;
    }

    if(is_program && address == LY) {
        value = 0x0;
    }

    uint8_t *mem = get_memory(gb, &address);
    mem[address] = value;
}

void write_short(GameBoy *gb, const uint16_t address, const uint16_t value, const bool is_program) {
    const uint8_t byte_a = (value & 0x00FF);
    const uint8_t byte_b = (value & 0xFF00) >> 8;

    write_byte(gb, address, byte_a, is_program);
    write_byte(gb, address + 1, byte_b, is_program);
}

void write_register(GameBoy *gb, const uint16_t address, const uint8_t bit, const uint8_t value) {
    uint8_t byte = read_byte(gb, address, false);
    byte ^= (-value ^ byte) & (1 << bit);

    write_byte(gb, address, byte, false);
}

uint8_t read_register(GameBoy *gb, const uint16_t address, const uint8_t bit) {
    const uint8_t byte = read_byte(gb, address, false);
    return GET_BIT(byte, bit);
}

static void DMA_transfer(GameBoy *gb, const uint8_t value) {
    const uint16_t address = value * 0x100;

    for(uint8_t i = 0; i <= 0x9F; ++i) {
        write_byte(gb, 0xFE00 + i, read_byte(gb, address + i, false), false); 
    }
    get_sprites(gb);
}