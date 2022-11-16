#include "mbc.h"

void mbc1_handler(GameBoy *gb, const uint16_t address, const uint8_t value) {

    static MBC1Mode mode = RomBanking;
    static bool ram_enabled = false;

    uint16_t rom_bank = gb->mmu.rom_bank;
    uint8_t ram_bank = gb->mmu.ram_bank;

    if(address <= MBC1_RAM_ENABLE_END)
        ram_enabled = (value & 0xF) == MBC1_RAM_ENABLE_NIBBLE ? true : false;

    // Select the lower 5 bits of the rom bank
    else if(address >= MBC1_ROM_CHANGE_START && address <= MBC1_ROM_CHANGE_END) {
        uint8_t lower = value & MBC1_ROM_CHANGE;

        // Banks 0x0, 0x20, 0x40, and 0x60 are unselectable
        if(lower == 0)
            lower++;

        rom_bank = (rom_bank & ~MBC1_ROM_CHANGE) | lower;
    }
    // Select the upper 2 bits of the rom or ram bank (5 and 6) (not bit 7) 
    else if(address >= MBC1_ROM_RAM_CHANGE_START && address <= MBC1_ROM_RAM_CHANGE_END) {

        rom_bank = (rom_bank & ~MBC1_ROM_RAM_CHANGE) | ((value & 0x3) << 5);
        ram_bank = value;
    }
    else if(address >= MBC1_MODE_CHANGE_START && address <= MBC1_MODE_CHANGE_END)
        mode = value;
    
    // Only ram bank 0 can be used in rom mode
    if(mode == RomBanking) {
        gb->mmu.rom00 = gb->cart.rom_banks[0];
        ram_bank = 0;
    }
    // Only rom banks 0-1F can be used in ram mode
    else if(mode == RamBanking) {
        uint8_t eff_rom_bank = rom_bank & MBC1_ROM_RAM_CHANGE;
        eff_rom_bank %= gb->cart.rom_size;
        gb->mmu.rom00 = gb->cart.rom_banks[eff_rom_bank];

        rom_bank &= MBC1_ROM_CHANGE;
    }

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

void mbc2_handler(GameBoy *gb, const uint16_t address, const uint8_t value) {
    mbc1_handler(gb, address, value);
}

void mbc3_handler(GameBoy *gb, const uint16_t address, const uint8_t value) {
    mbc1_handler(gb, address, value);
}

void mbc4_handler(GameBoy *gb, const uint16_t address, const uint8_t value) {
    mbc1_handler(gb, address, value);
}

void mbc5_handler(GameBoy *gb, const uint16_t address, const uint8_t value) {

    static bool ram_enabled = false;

    uint16_t rom_bank = gb->mmu.rom_bank;
    uint8_t ram_bank = gb->mmu.ram_bank;

    if(address <= MBC1_RAM_ENABLE_END)
        ram_enabled = (value & 0xF) == MBC1_RAM_ENABLE_NIBBLE ? true : false;

    else if(address >= MBC5_ROM_CHANGE_LOW_START && address <= MBC5_ROM_CHANGE_LOW_END)
        rom_bank = (rom_bank & 0x100) | value; // keep the top bit

    else if(address >= MBC5_ROM_CHANGE_HIGH_START && address <= MBC5_ROM_CHANGE_HIGH_END)
        rom_bank = (rom_bank & 0xFF) | ((value & 0x1) << 8);

    else if(address >= MBC5_RAM_CHANGE_START && address <= MBC5_RAM_CHANGE_END)
        ram_bank = value & 0xF;
    
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

