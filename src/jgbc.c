#include "jgbc.h"
#include "ppu.h"
#include "apu.h"
#include "input.h"
#include "cpu.h"
#include "mmu.h"

static void reset_hw_registers(GameBoy *);


void init(GameBoy *gb) {
    init_mmu(gb);
    init_ppu(gb);
    init_apu(gb);

    reset(gb);
}

void reset(GameBoy *gb) {
    reset_cpu(gb);
    reset_mmu(gb);
    reset_ppu(gb);
    reset_input(gb);
    reset_apu(gb);
    reset_hw_registers(gb);
}

static void reset_hw_registers(GameBoy *gb) {
    SWRITE8(JOYP, 0x1F);
    SWRITE8(IF, 0xE0);
    SWRITE8(TIMA, 0x00);
    SWRITE8(TMA, 0x00);
    SWRITE8(TAC, 0x00);
    SWRITE8(LCDC, 0x91);
    SWRITE8(SCY, 0x00);
    SWRITE8(SCX, 0x00);
    SWRITE8(LYC, 0x00);
    SWRITE8(LY, 0x00);
    SWRITE8(BGP, 0xFC);
    SWRITE8(OBP0, 0xFF);
    SWRITE8(OBP1, 0xFF);
    SWRITE8(WY, 0x00);
    SWRITE8(WX, 0x00);
    SWRITE8(IE, 0x00);
}
