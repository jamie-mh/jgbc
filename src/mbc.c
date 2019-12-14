#include "jgbc.h"
#include "mbc.h"
#include "cart.h"
#include "mmu.h"

static void mbc1(GameBoy *, uint16_t, uint8_t);
static void mbc2(GameBoy *, uint16_t, uint8_t);
static void mbc3(GameBoy *, uint16_t, uint8_t);
static void mbc4(GameBoy *, uint16_t, uint8_t);
static void mbc5(GameBoy *, uint16_t, uint8_t);


// Checks if the memory being written to is attempting to switch banks
void mbc_check(GameBoy *gb, const uint16_t address, const uint8_t value) {

    switch(gb->cart.type) {
        case 0x1:
        case 0x2:
        case 0x3:
            mbc1(gb, address, value);
            break;

        case 0x5:
        case 0x6:
            mbc2(gb, address, value);
            break;

        case 0xF:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            mbc3(gb, address, value);
            break;

        case 0x15:
        case 0x16:
        case 0x17:
            mbc4(gb, address, value);
            break;

        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            mbc5(gb, address, value);
            break;
    }
}

static void mbc1(GameBoy *gb, const uint16_t address, const uint8_t value) {

    static bool rom_mode = true;
    static bool ram_enabled = false;

    uint8_t rom_bank = gb->mmu.rom_bank;
    uint8_t ram_bank = gb->mmu.ram_bank;

    if(address <= 0x1FFF)
        ram_enabled = ((value & 0xF) == 0xA) ? true : false;

    // Select the lower 5 bits of the rom bank
    else if(address >= 0x2000 && address <= 0x3FFF) {
        rom_bank &= 0xE0;
        rom_bank |= (value & 0x1F);

        // Banks 20h, 40h, and 60h are unselectable
        switch(rom_bank) {
            case 0x00:
            case 0x20:
            case 0x40:
            case 0x60:
                rom_bank++;
        }
    }
    // Select the upper 2 bits of the rom or ram bank (5 and 6) (not bit 7) 
    else if(address >= 0x4000 && address <= 0x5FFF) {

        if(rom_mode) {
            rom_bank &= 0x9F;
            rom_bank |= (value & 0x60);
        } else
            ram_bank = value;
    }
    else if(address >= 0x6000 && address <= 0x7FFF)
        rom_mode = (value == 0x1) ? false : true;

    // Only ram bank 0 can be used in rom mode
    if(rom_mode)
        ram_bank = 0;

    gb->mmu.rom_bank = rom_bank % gb->cart.rom_size;
    gb->mmu.romNN = gb->cart.rom_banks[gb->mmu.rom_bank];

    if(ram_enabled && gb->cart.ram_size > 0) {
        gb->mmu.ram_bank = ram_bank % gb->cart.ram_size;
        gb->mmu.extram = gb->cart.ram_banks[gb->mmu.ram_bank];
    } else {
        gb->mmu.ram_bank = -1;
        gb->mmu.extram = NULL;
    }
}

static void mbc2(GameBoy *gb, const uint16_t address, const uint8_t value) {
    // TODO: Implement
    mbc1(gb, address, value);
}

static void mbc3(GameBoy *gb, const uint16_t address, const uint8_t value) {
    // TODO: Implement RTC
    mbc1(gb, address, value);
}

static void mbc4(GameBoy *gb, const uint16_t address, const uint8_t value) {
    mbc1(gb, address, value);
}

static void mbc5(GameBoy *gb, const uint16_t address, const uint8_t value) {
    mbc1(gb, address, value);
}

