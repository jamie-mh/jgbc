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
    gb->mmu.rom00 = NULL;
    gb->mmu.romNN = NULL;
    gb->mmu.extram = NULL;

    gb->mmu.vram_banks = malloc(sizeof(uint8_t *) * VRAM_BANK_COUNT);
    for(uint8_t i = 0; i < VRAM_BANK_COUNT; ++i) {
        gb->mmu.vram_banks[i] = calloc(VRAM_BANK_SIZE, sizeof(uint8_t));
    }

    gb->mmu.wram_banks = malloc(sizeof(uint8_t *) * WRAM_BANK_COUNT); 
    for(uint8_t i = 0; i < WRAM_BANK_COUNT; ++i) {
        gb->mmu.wram_banks[i] = calloc(WRAM_BANK_SIZE, sizeof(uint8_t));
    }

    gb->mmu.vram = gb->mmu.vram_banks[0];
    gb->mmu.wram00 = gb->mmu.wram_banks[0];
    gb->mmu.wramNN = gb->mmu.wram_banks[1];
    
    gb->mmu.oam = calloc(OAM_SIZE, sizeof(uint8_t));
    gb->mmu.io = calloc(IO_SIZE, sizeof(uint8_t));
    gb->mmu.hram = calloc(HRAM_SIZE, sizeof(uint8_t));
    gb->mmu.ier = calloc(1, sizeof(uint8_t));
}

static uint8_t *get_memory(GameBoy *gb, uint16_t *address) {

    // 16KB ROM Bank 00 
    if(*address <= ROM00_END) {
        return gb->mmu.rom00;
    }
    // 16KB ROM Bank NN
    else if(*address >= ROMNN_START && *address <= ROMNN_END) {
        *address -= ROMNN_START;
        return gb->mmu.romNN;
    }
    // 8KB Video RAM
    else if(*address >= VRAM_START && *address <= VRAM_END) {
        *address -= VRAM_START;
        return gb->mmu.vram;
    }
    // 8KB External RAM (in cartridge)
    else if(*address >= EXTRAM_START && *address <= EXTRAM_END) {
        *address -= EXTRAM_START;
        return gb->mmu.extram;
    }
    // 4KB Work RAM Bank 00
    else if(*address >= WRAM00_START && *address <= WRAM00_END) {
        *address -= WRAM00_START;
        return gb->mmu.wram00;
    }
    // 4KB Work RAM Bank NN
    else if(*address >= WRAMNN_START && *address <= WRAMNN_END) {
        *address -= WRAMNN_START;
        return gb->mmu.wramNN;
    }
    // Mirror of Work RAM (wram 00)
    else if(*address >= WRAM00_MIRROR_START && *address <= WRAM00_MIRROR_END) {
        *address -= WRAM00_MIRROR_START;
        return gb->mmu.wram00;
    }
    // Mirror of Work RAM (wram nn)
    else if(*address >= WRAMNN_MIRROR_START && *address <= WRAMNN_MIRROR_END) {
        *address -= WRAMNN_MIRROR_START;
        return gb->mmu.wramNN;
    }
    // Sprite Attibute Table
    else if(*address >= OAM_START && *address <= OAM_END) {
        *address -= OAM_START;
        return gb->mmu.oam;
    }
    // IO Registers
    else if(*address >= IO_START && *address <= IO_END) {
        *address -= IO_START;
        return gb->mmu.io;
    }
    // High RAM
    else if(*address >= HRAM_START && *address <= HRAM_END) {
        *address -= HRAM_START;
        return gb->mmu.hram;
    }
    // Interrupt Enable Register
    else if(*address == IE_START_END) {
        *address = 0;
        return gb->mmu.ier;
    }

    return NULL;
}

static bool is_accessible(GameBoy *gb, const uint16_t address) {

    if(address >= UNUSABLE_START && address <= UNUSABLE_END) {
        return false;
    }

    if(address >= EXTRAM_START && address <= EXTRAM_END && gb->mmu.extram == NULL) {
        return false;
    }

    return true;
}

uint8_t read_byte(GameBoy *gb, uint16_t address, const bool is_program) {

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
    uint8_t byte = SREAD8(address);
    byte ^= (-value ^ byte) & (1 << bit);

    SWRITE8(address, byte);
}

uint8_t read_register(GameBoy *gb, const uint16_t address, const uint8_t bit) {
    const uint8_t byte = SREAD8(address);
    return GET_BIT(byte, bit);
}

static void DMA_transfer(GameBoy *gb, const uint8_t value) {
    const uint16_t address = value * 0x100;

    for(uint8_t i = 0; i <= 0x9F; ++i) {
		SWRITE8(0xFE00 + i, SREAD8(address + i));
    }
    get_sprites(gb);
}
