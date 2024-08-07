#include "mmu.h"
#include "apu.h"
#include "cpu.h"
#include "input.h"
#include "macro.h"
#include "ppu.h"
#include <assert.h>
#include <stdlib.h>

static uint8_t *get_memory(GameBoy *, uint16_t *);
static bool is_accessible(GameBoy *, uint16_t);

static void trigger_dma(GameBoy *gb, const uint8_t value);
static void hdma_write(GameBoy *, uint16_t, uint8_t);

void init_mmu(GameBoy *gb) {
    gb->mmu.rom00 = NULL;
    gb->mmu.romNN = NULL;
    gb->mmu.extram = NULL;

    gb->mmu.vram_banks = malloc(sizeof(uint8_t *) * VRAM_BANK_COUNT);

    for (uint8_t i = 0; i < VRAM_BANK_COUNT; ++i) {
        gb->mmu.vram_banks[i] = calloc(VRAM_BANK_SIZE, sizeof(uint8_t));
    }

    gb->mmu.wram_banks = malloc(sizeof(uint8_t *) * WRAM_BANK_COUNT);

    for (uint8_t i = 0; i < WRAM_BANK_COUNT; ++i) {
        gb->mmu.wram_banks[i] = calloc(WRAM_BANK_SIZE, sizeof(uint8_t));
    }

    gb->mmu.oam = calloc(OAM_SIZE, sizeof(uint8_t));
    gb->mmu.io = calloc(IO_SIZE, sizeof(uint8_t));
    gb->mmu.hram = calloc(HRAM_SIZE, sizeof(uint8_t));
    gb->mmu.ier = calloc(1, sizeof(uint8_t));

    gb->mmu.serial_write_handler = NULL;
}

void reset_mmu(GameBoy *gb) {
    gb->mmu.vram = gb->mmu.vram_banks[0];
    gb->mmu.wram00 = gb->mmu.wram_banks[0];
    gb->mmu.wramNN = gb->mmu.wram_banks[1];

    gb->mmu.dma.is_active = false;
    gb->mmu.dma.address = 0;
    gb->mmu.dma.clock = 0;

    gb->mmu.hdma.is_active = false;
    gb->mmu.hdma.source_addr = 0;
    gb->mmu.hdma.dest_addr = 0;
    gb->mmu.hdma.mode = GeneralPurposeDMA;
    gb->mmu.hdma.length = 0;
}

static uint8_t *get_memory(GameBoy *gb, uint16_t *address) {

    // 16KB ROM Bank 00
    if (*address <= ROM00_END) {
        return gb->mmu.rom00;
    }
    // 16KB ROM Bank NN
    else if (*address >= ROMNN_START && *address <= ROMNN_END) {
        *address -= ROMNN_START;
        return gb->mmu.romNN;
    }
    // 8KB Video RAM
    else if (*address >= VRAM_START && *address <= VRAM_END) {
        *address -= VRAM_START;
        return gb->mmu.vram;
    }
    // 8KB External RAM (in cartridge)
    else if (*address >= EXTRAM_START && *address <= EXTRAM_END) {
        *address -= EXTRAM_START;
        return gb->mmu.extram;
    }
    // 4KB Work RAM Bank 00
    else if (*address >= WRAM00_START && *address <= WRAM00_END) {
        *address -= WRAM00_START;
        return gb->mmu.wram00;
    }
    // 4KB Work RAM Bank NN
    else if (*address >= WRAMNN_START && *address <= WRAMNN_END) {
        *address -= WRAMNN_START;
        return gb->mmu.wramNN;
    }
    // Mirror of Work RAM (wram 00)
    else if (*address >= WRAM00_MIRROR_START && *address <= WRAM00_MIRROR_END) {
        *address -= WRAM00_MIRROR_START;
        return gb->mmu.wram00;
    }
    // Mirror of Work RAM (wram nn)
    else if (*address >= WRAMNN_MIRROR_START && *address <= WRAMNN_MIRROR_END) {
        *address -= WRAMNN_MIRROR_START;
        return gb->mmu.wramNN;
    }
    // Sprite Attibute Table
    else if (*address >= OAM_START && *address <= OAM_END) {
        *address -= OAM_START;
        return gb->mmu.oam;
    }
    // IO Registers
    else if (*address >= IO_START && *address <= IO_END) {
        *address -= IO_START;
        return gb->mmu.io;
    }
    // High RAM
    else if (*address >= HRAM_START && *address <= HRAM_END) {
        *address -= HRAM_START;
        return gb->mmu.hram;
    }
    // Interrupt Enable Register
    else if (*address == IE_START_END) {
        *address = 0;
        return gb->mmu.ier;
    }

    return NULL;
}

static bool is_accessible(GameBoy *gb, const uint16_t address) {
    if (address >= UNUSABLE_START && address <= UNUSABLE_END) {
        return false;
    }

    if (address >= EXTRAM_START && address <= EXTRAM_END && gb->mmu.extram == NULL) {
        return false;
    }

    return true;
}

uint8_t read_byte(GameBoy *gb, uint16_t address, const bool is_program) {
    if (is_program && address == JOYP) {
        return joypad_state(gb);
    }

    if (!is_accessible(gb, address)) {
        return 0xFF;
    }

    if (address == HDMA5) {
        return (!gb->mmu.hdma.is_active << 7) | (0x1F);
    }

    if (gb->mmu.dma.is_active && gb->mmu.dma.clock > 0 && address >= OAM_START && address <= OAM_END) {
        return 0xFF;
    }

    if (address >= UNUSED_AUDIO_START && address < WAVE_TABLE_START) {
        // Above the NR registers, all data is set to FF
        return 0xFF;
    }

    uint16_t relative_addr = address;
    const uint8_t *mem = get_memory(gb, &relative_addr);
    uint8_t data = mem[relative_addr];

    if (address >= NR10 && address <= NR52) {
        data = audio_register_read(gb, address, data);
    } else if (address == KEY1) {
        data = (data & 0x7F) | (gb->cpu.is_double_speed << 7);
    }

    return data;
}

uint16_t read_short(GameBoy *gb, const uint16_t address, const bool is_program) {
    const uint8_t byte_a = read_byte(gb, address, is_program);
    const uint8_t byte_b = read_byte(gb, address + 1, is_program);

    return byte_b << 8 | byte_a;
}

void write_byte(GameBoy *gb, uint16_t address, uint8_t value, const bool is_program) {
    if (address <= ROMNN_END) {
        if (gb->mmu.mbc_handler != NULL) {
            gb->mmu.mbc_handler(gb, address, value);
        }

        return;
    }

    if (!is_accessible(gb, address)) {
        return;
    }

    if (is_program) {
        if (address == SB && gb->mmu.serial_write_handler != NULL) {
            gb->mmu.serial_write_handler(value);
            return;
        }

        if (address == DMA) {
            trigger_dma(gb, value);
        }

        if (address == DIV) {
            set_div(gb, 0);
            value = 0x0;
        }

        if (address == LY) {
            value = 0x0;
        }

        if (address >= NR10 && address <= NR52) {
            // Ignore APU register writes when APU is disabled
            // Unless we're changing the control register
            if (!gb->apu.enabled && address != NR52) {
                return;
            }

            audio_register_write(gb, address, value);
        }

        if (address == VBK) {
            const uint8_t bank = GET_BIT(value, VBK_BANK);

            gb->mmu.vram_bank = bank;
            gb->mmu.vram = gb->mmu.vram_banks[bank];
        }

        if (address == BGPI || address == OBPI) {
            palette_index_write(gb, address, value);
        }

        if (address == BGPD || address == OBPD) {
            palette_data_write(gb, address, value);
        }

        if (address >= HDMA1 && address <= HDMA5) {
            hdma_write(gb, address, value);
        }
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

static void trigger_dma(GameBoy *gb, const uint8_t value) {
    gb->mmu.dma.is_active = true;
    gb->mmu.dma.clock = 0;
    gb->mmu.dma.address = value * 0x100;
}

void update_dma(GameBoy *gb) {
    if (!gb->mmu.dma.is_active) {
        return;
    }

    gb->mmu.dma.clock += gb->cpu.ticks;

    if (gb->mmu.dma.clock >= DMA_CLOCKS) {
        for (uint8_t i = 0; i <= 0x9F; ++i)  {
            SWRITE8(0xFE00 + i, SREAD8(gb->mmu.dma.address + i));
        }

        gb->mmu.dma.is_active = false;
    }
}

void update_hdma(GameBoy *gb) {
    if (!gb->mmu.hdma.is_active) {
        return;
    }

    for (uint8_t i = 0; i < gb->mmu.hdma.length; ++i) {
        SWRITE8(gb->mmu.hdma.dest_addr + i, SREAD8(gb->mmu.hdma.source_addr + i));
    }

    gb->mmu.hdma.is_active = false;
}

static void hdma_write(GameBoy *gb, const uint16_t addr, const uint8_t value) {
    switch (addr) {
    case HDMA1:
        gb->mmu.hdma.source_addr = (gb->mmu.hdma.source_addr & 0xFF) | (value << 8);
        break;

    case HDMA2:
        gb->mmu.hdma.source_addr = (gb->mmu.hdma.source_addr & 0xFF00) | value;
        gb->mmu.hdma.source_addr &= 0xFFF0; // Lower 4 bits are ignored
        break;

    case HDMA3:
        gb->mmu.hdma.dest_addr = (gb->mmu.hdma.dest_addr & 0xFF) | (value << 8);
        gb->mmu.hdma.dest_addr &= 0x1FFF; // Upper 4 bits are ignored
        break;

    case HDMA4:
        gb->mmu.hdma.dest_addr = (gb->mmu.hdma.dest_addr & 0xFF00) | value;
        gb->mmu.hdma.source_addr &= 0xFFF0;
        break;

    case HDMA5:
        gb->mmu.hdma.length = (value & HDMA5_LENGTH) * 0x10 + 1;
        gb->mmu.hdma.mode = (value & HDMA5_MODE) >> 7;
        gb->mmu.hdma.is_active = true;
        break;

    default:
        ASSERT_NOT_REACHED();
    }
}
