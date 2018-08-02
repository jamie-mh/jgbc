#include "jgbc.h"
#include "ppu.h"
#include "input.h"
#include "cpu.h"
#include "mmu.h"

static void init_hw_registers(GameBoy *gb);


void init(GameBoy *gb) {
    init_cpu(gb);
    init_mmu(gb);
    init_ppu(gb);
    init_input(gb);
    init_hw_registers(gb);
}

static void init_hw_registers(GameBoy *gb) {
    SWRITE8(JOYP, 0x1F);
    SWRITE8(IF, 0xE0);
    SWRITE8(TIMA, 0x00);
    SWRITE8(TMA, 0x00);
    SWRITE8(TAC, 0x00);
    SWRITE8(LCDC, 0x91);
    SWRITE8(SCY, 0x00);
    SWRITE8(SCX, 0x00);
    SWRITE8(LYC, 0x00);
    SWRITE8(BGP, 0xFC);
    SWRITE8(OBP0, 0xFF);
    SWRITE8(OBP1, 0xFF);
    SWRITE8(WY, 0x00);
    SWRITE8(WX, 0x00);
    SWRITE8(IE, 0x00);
}