#include "lxgbc.h"
#include "mbc.h"
#include "cart.h"
#include "ram.h"

static void mbc1(gbc_system *, const unsigned short, const unsigned char);
static void mbc2(gbc_system *, const unsigned short, const unsigned char);
static void mbc3(gbc_system *, const unsigned short, const unsigned char);


// Checks if the memory being written to is attempting to switch banks
void mbc_check(gbc_system *gbc, const unsigned short address, const unsigned char value) {

    switch(gbc->cart->mbc_type) {
        case 1:
            mbc1(gbc, address, value);
            break;

        case 2:
            mbc2(gbc, address, value);
            break;

        case 3:
            mbc3(gbc, address, value);
    }
}

static void mbc1(gbc_system *gbc, const unsigned short address, const unsigned char value) {

    static bool rom_mode = true;
    static bool ram_enabled = false;

    unsigned char rom_bank = gbc->cart->curr_rom_bank;
    unsigned char ram_bank = gbc->cart->curr_ram_bank;

    switch(address) {
    
        case 0 ... 0x1FFF:
            ram_enabled = (value == 0xA) ? true : false;
            break;
   
        // Select the lower 5 bits of the rom bank
        case 0x2000 ... 0x3FFF:
            rom_bank &= 0xE0;
            rom_bank |= (value & 0x1F); 

            if(rom_bank == 0) {
                rom_bank = 1; 
            }
            break;

        // Select the upper 2 bits of the rom or ram bank (5 and 6) (not bit 7) 
        case 0x4000 ... 0x5FFF:

            if(rom_mode) {
                rom_bank &= 0x9F;
                rom_bank |= (value & 0x60);

                // Banks 20h, 40h, and 60h are unselectable
                switch(rom_bank) {
                    case 0x20:
                    case 0x40:
                    case 0x60:
                        rom_bank++;
                }
            } else {
                ram_bank = value; 
            }
            break;

        case 0x6000 ... 0x7FFF:
            rom_mode = (value == 0x1) ? false : true;
            break;
    }

    gbc->cart->curr_rom_bank = rom_bank % gbc->cart->rom_size;
    gbc->ram->romNN = gbc->cart->rom_banks[rom_bank];

    if(ram_enabled && gbc->cart->ram_size > 0) {
        gbc->cart->curr_ram_bank = ram_bank % gbc->cart->ram_size;
        gbc->ram->extram = gbc->cart->ram_banks[ram_bank];
    } else {
        gbc->ram->extram = NULL; 
    }
}

static void mbc2(gbc_system *gbc, const unsigned short address, const unsigned char value) {
    // TODO: Implement
    mbc1(gbc, address, value);
}

static void mbc3(gbc_system *gbc, const unsigned short address, const unsigned char value) {
    // TODO: Implement RTC
    mbc1(gbc, address, value);
}
