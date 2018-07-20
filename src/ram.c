#include "jgbc.h"
#include "ppu.h"
#include "cpu.h"
#include "ram.h"
#include "mbc.h"
#include "input.h"

static uint8_t *get_memory_location(gbc_system *, uint16_t *);
static void DMA_transfer(gbc_system *, uint8_t);


// Allocates memory
void init_ram(gbc_system *gbc) {

    gbc->ram->rom00 = NULL;
    gbc->ram->romNN = NULL;
    gbc->ram->extram = NULL;

    gbc->ram->vram = calloc(VRAM_SIZE, sizeof(uint8_t));
    gbc->ram->wram_banks = malloc(sizeof(uint8_t *) * WRAM_BANK_COUNT); 

    for(uint8_t i = 0; i < WRAM_BANK_COUNT; i++) {
        gbc->ram->wram_banks[i] = calloc(WRAM_BANK_SIZE, sizeof(uint8_t));
    }

    // Point the WRAM to the default banks
    gbc->ram->wram00 = gbc->ram->wram_banks[0];
    gbc->ram->wramNN = gbc->ram->wram_banks[1];
    
    gbc->ram->oam = calloc(OAM_SIZE, sizeof(uint8_t));
    gbc->ram->io = calloc(IO_SIZE, sizeof(uint8_t));
    gbc->ram->hram = calloc(HRAM_SIZE, sizeof(uint8_t));
    gbc->ram->ier = calloc(1, sizeof(uint8_t));
}

// Returns a pointer to the memory location of the specified address
// and changes the input address to the relative location inside this section.
static uint8_t *get_memory_location(gbc_system *gbc, uint16_t *address) {

	// 16KB ROM Bank 00 
	if(*address >= ROM00_START && *address <= ROM00_END) {
		if(*address <= PROGRAM_START && !read_byte(gbc, BOOTROM_DISABLE, false)) {
			return gbc->ram->bootrom;
		} else {
			return gbc->ram->rom00;
		}
	}
	// 16KB ROM Bank NN
	else if(*address >= ROMNN_START && *address <= ROMNN_END) {
		*address -= ROMNN_START;
		return gbc->ram->romNN;
	}
	// 8KB Video RAM
	else if(*address >= VRAM_START && *address <= VRAM_END) {
		*address -= VRAM_START;
		return gbc->ram->vram;
	}
	// 8KB External RAM (in cartridge)
	else if(*address >= EXTRAM_START && *address <= EXTRAM_END) {
		*address -= EXTRAM_START;
		return gbc->ram->extram;
	}
	// 4KB Work RAM Bank 00
	else if(*address >= WRAM00_START && *address <= WRAM00_END) {
		*address -= WRAM00_START;
		return gbc->ram->wram00;
	}
	// 4KB Work RAM Bank NN
	else if(*address >= WRAMNN_START && *address <= WRAMNN_END) {
		*address -= WRAMNN_START;
		return gbc->ram->wramNN;
	}
	// Mirror of Work RAM (wram 00)
	else if(*address >= WRAM00_MIRROR_START && *address <= WRAM00_MIRROR_END) {
		*address -= WRAM00_MIRROR_START;
		return gbc->ram->wram00;
	}
	// Mirror of Work RAM (wram nn)
	else if(*address >= WRAMNN_MIRROR_START && *address <= WRAMNN_MIRROR_END) {
		*address -= WRAMNN_MIRROR_START;
		return gbc->ram->wramNN;
	}
	// Sprite Attibute Table
	else if(*address >= OAM_START && *address <= OAM_END) {
		*address -= OAM_START;
		return gbc->ram->oam;
	}
	// IO Registers
	else if(*address >= IO_START && *address <= IO_END) {
		*address -= IO_START;
		return gbc->ram->io;
	}
	// High RAM
	else if(*address >= HRAM_START && *address <= HRAM_END) {
		*address -= HRAM_START;
		return gbc->ram->hram;
	}
	// Interrupt Enable Register
	else if(*address == IE_START_END) {
		*address = 0;
		return gbc->ram->ier;
	}

	return NULL;
}

// Checks if the ram address is valid and can be read / written
bool is_valid_ram(gbc_ram *ram, const uint16_t address) {

    // If the memory is unusable
    if(address >= UNUSABLE_START && address <= UNUSABLE_END) {
        return false;
    }

    // If the memory is in extram and it is not available
    if(address >= EXTRAM_START && address <= EXTRAM_END && ram->extram == NULL) {
        return false;
    }

    return true;
}

// Reads a byte from the specified location in memory
uint8_t read_byte(gbc_system *gbc, const uint16_t address, const bool is_program) {

    if(is_program && address == JOYP) {
        return joypad_state(gbc);  
    }

	if(address == DIV) {
		return (gbc->cpu->timer & 0xFF00) >> 8;
	}

    if(!is_valid_ram(gbc->ram, address)) {
        return 0x0;
    }

    // Get the memory location and the relative address inside this memory
    uint16_t rel_address = address;
    uint8_t *mem = get_memory_location(gbc, &rel_address);

    // Return the byte from memory
    return mem[rel_address];
}

// Reads a short (2 bytes) from the specified location in memory
// Returns the LS byte first
uint16_t read_short(gbc_system *gbc, const uint16_t address, const bool is_program) {

    const uint8_t byte_a = read_byte(gbc, address, is_program);
    const uint8_t byte_b = read_byte(gbc, address + 1, is_program);

    return byte_b << 8 | byte_a;
}

// Writes a byte in memory at the address
void write_byte(gbc_system *gbc, const uint16_t address, uint8_t value, const bool is_program) {
    mbc_check(gbc, address, value);

    // If the timer divider register is written to, it is reset
    if(address == DIV) {
		gbc->cpu->timer = 0;
		return;
    }

    if(!is_valid_ram(gbc->ram, address)) {
        return;
    }

    // Serial bus
    // if(address == SB) {
    //     printf("%c", value);
    // }

    // If the memory is ROM
    if(is_program && address <= ROMNN_END) {
        return; 
    }

    if(address == DMA) {
        DMA_transfer(gbc, value); 
    }

    // If the vertical line register is written to, it is reset
    if(is_program && address == LY) {
        value = 0x0;
    }

    // Get the memory location with the relative address
    uint16_t rel_address = address;
    uint8_t *mem = get_memory_location(gbc, &rel_address);
    
    mem[rel_address] = value;

	if(is_program && address == TAC) {
		update_timer(gbc, 0);
		update_timer(gbc, 0);
	}
}

// Writes two bytes in memory starting at the address
void write_short(gbc_system *gbc, const uint16_t address, const uint16_t value, const bool is_program) {

    // Split the short into two bytes
    const uint8_t byte_a = (value & 0x00FF);
    const uint8_t byte_b = (value & 0xFF00) >> 8;

    write_byte(gbc, address, byte_a, is_program);
    write_byte(gbc, address + 1, byte_b, is_program);
}

// Writes to memory register at a certain location
void write_register(gbc_system *gbc, const uint16_t address, const uint8_t bit, const uint8_t value) {
    
    uint8_t byte = read_byte(gbc, address, false);
    byte ^= (-value ^ byte) & (1 << bit);
    write_byte(gbc, address, byte, false);
}

// Reads a memory register at a certain location
uint8_t read_register(gbc_system *gbc, const uint16_t address, const uint8_t bit) {
   
    // Return the nth bit of the register
    const uint8_t byte = read_byte(gbc, address, false);
    return (byte >> bit) & 1;
}

// Launches a DMA transfer from ROM or RAM to OAM memory (sprite data)
static void DMA_transfer(gbc_system *gbc, uint8_t value) {

    const uint16_t address = value * 0x100;

    for(int i = 0; i <= 0x9F; i++) {
        write_byte(gbc, 0xFE00 + i, read_byte(gbc, address + i, false), false); 
    }
}