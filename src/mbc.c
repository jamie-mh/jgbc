#include "lxgbc.h"
#include "mbc.h"
#include "rom.h"
#include "ram.h"

static void switch_rom_bank(gbc_system *, const unsigned short, const unsigned char);
static void switch_extram_mode(gbc_system *, const unsigned short, const unsigned char);

// Checks if the memory being written to is attempting to switch banks
void mbc_check(gbc_system *gbc, const unsigned short address, const unsigned char value) {

    // Cartridge extram on / off 
    if(address <= 0x1FFF) {
        /*switch_extram_mode(gbc, address, value);*/
    }
    // Writing to rom, this indicates a rom bank NN switch
    else if(address <= 0x3FFF || (address >= 0x4000 && address <= 0x7FFF)) {
        switch_rom_bank(gbc, address, value); 
    }
}

static void switch_rom_bank(gbc_system *gbc, const unsigned short address, const unsigned char value) {

    const char mbc_type = gbc->rom->mbc_type;

    if(mbc_type == 1) {

        // Writing to rom 00 changes the lower 5 bits of the bank number
        if(address <= 0x3FFF) {
        
            const unsigned char lower_5_bits = value & 0x1F;

            // Replace the lower 5 bits in the current rom bank
            // with the lower 5 bits of the written value
            gbc->rom->curr_rom_bank &= 0xE0;
            gbc->rom->curr_rom_bank |= lower_5_bits;
        } 
        // Writing to rom NN changes bits 5 and 6 of the bank number
        else if(address >= 0x4000 && address <= 0x7FFF) {
       
            const unsigned char upper_3_bits = value & 0xE0;

            // Replace the bits 5 and 6 with those in the written value
            gbc->rom->curr_rom_bank &= 0x1F;
            gbc->rom->curr_rom_bank |= upper_3_bits;
        }

    } else if(mbc_type == 2) {
        gbc->rom->curr_rom_bank = value & 0xF;
    }

    // Never point rom NN to rom 00
    if(gbc->rom->curr_rom_bank == 0) {
        gbc->rom->curr_rom_bank = 1;
    }

    // Wrap around if bank doesn't exist
    gbc->rom->curr_rom_bank %= gbc->rom->rom_size;

    gbc->ram->romNN = gbc->rom->rom_banks[gbc->rom->curr_rom_bank];
}

static void switch_extram_mode(gbc_system *gbc, const unsigned short address, const unsigned char value) {

    // In MBC 2, bit 4 of the address must be 0 to turn RAM on / off
    if(gbc->rom->mbc_type == 2 && ((address >> 4) & 1)) {
        return; 
    }

    const unsigned char lower_nibble = value & 0xF;

    // RAM Enabled
    if(lower_nibble == 0xA) {
        gbc->ram->extram = gbc->rom->ram_banks[gbc->rom->curr_ram_bank];
    }
    // RAM Disabled
    else if(lower_nibble == 0x0) {
        gbc->ram->extram = NULL;
    }
}
