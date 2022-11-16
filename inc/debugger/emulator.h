#pragma once

namespace Emulator 
{
    extern "C" {
        #include "gameboy.h"
        #include "mmu.h"
        #include "cpu.h"
        #include "mbc.h"
        #include "cart.h"
        #include "ppu.h"
        #include "apu.h"
        #include "input.h"
    }
};