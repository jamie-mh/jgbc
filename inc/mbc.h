#pragma once

#define MBC1_RAM_ENABLE_END 0x1FFF
#define MBC1_RAM_ENABLE_NIBBLE 0xA

#define MBC1_ROM_CHANGE_START 0x2000
#define MBC1_ROM_CHANGE_END 0x3FFF
#define MBC1_ROM_CHANGE 0x1F

#define MBC1_ROM_RAM_CHANGE_START 0x4000
#define MBC1_ROM_RAM_CHANGE_END 0x5FFF
#define MBC1_ROM_RAM_CHANGE 0x60

#define MBC1_MODE_CHANGE_START 0x6000 
#define MBC1_MODE_CHANGE_END 0x7FFF

typedef enum {
    RomBanking = 0, RamBanking = 1
}
MBC1Mode;

void mbc1_handler(GameBoy *, uint16_t, uint8_t);
void mbc2_handler(GameBoy *, uint16_t, uint8_t);
void mbc3_handler(GameBoy *, uint16_t, uint8_t);
void mbc4_handler(GameBoy *, uint16_t, uint8_t);
void mbc5_handler(GameBoy *, uint16_t, uint8_t);

