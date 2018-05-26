#include "lxgbc.h"
#include "ppu.h"
#include "cpu.h"
#include "ram.h"

static unsigned char *get_memory_location(gbc_ram *, unsigned short *);


// Allocates memory
void init_ram(gbc_ram *ram) {

    ram->rom00 = NULL;
    ram->romNN = NULL;
    ram->extram = NULL;

    ram->vram = calloc(VRAM_SIZE, sizeof(char));
    ram->wram_banks = malloc(sizeof(char *) * WRAM_BANK_COUNT); 

    for(int i = 0; i < WRAM_BANK_COUNT; i++) {
        ram->wram_banks[i] = calloc(WRAM_BANK_SIZE, sizeof(char));
    }

    // Point the WRAM to the default banks
    ram->wram00 = ram->wram_banks[0];
    ram->wramNN = ram->wram_banks[1];
    
    ram->oam = calloc(OAM_SIZE, sizeof(char));
    ram->io = calloc(IO_SIZE, sizeof(char));
    ram->hram = calloc(HRAM_SIZE, sizeof(char));
    ram->ier = calloc(1, sizeof(char));

    write_byte(ram, LCDC, DEFAULT_LCDC, 0);
}

// Returns a pointer to the memory location of the specified address
// and changes the input address to the relative location inside this section.
static unsigned char *get_memory_location(gbc_ram *ram, unsigned short *address) {

    // 16KB ROM Bank 00 
    if(*address <= 0x3FFF) {
        return ram->rom00;
    } 
    // 16KB ROM Bank NN
    else if(*address >= 0x4000 && *address <= 0x7FFF) {
        *address -= 0x4000;
        return ram->romNN;
    } 
    // 8KB Video RAM
    else if(*address >= 0x8000 && *address <= 0x9FFF) {
        *address -= 0x8000;
        return ram->vram;
    } 
    // 8KB External RAM (in cartridge)
    else if(*address >= 0xA000 && *address <= 0xBFFF) {
        *address -= 0xA000;
        return ram->extram;
    }
    // 4KB Work RAM Bank 00
    else if(*address >= 0xC000 && *address <= 0xCFFF) {
        *address -= 0xC000;
        return ram->wram00;
    } 
    // 4KB Work RAM Bank NN
    else if(*address >= 0xD000 && *address <= 0xDFFF) {
        *address -= 0xD000;
        return ram->wramNN;
    } 
    // Mirror of Work RAM (wram 00)
    else if(*address >= 0xE000 && *address <= 0xEFFF) {
        *address -= 0xE000;
        return ram->wram00;
    }
    // Mirror of Work RAM (wram nn)
    else if(*address >= 0xF000 && *address <= 0xFDFF) {
        *address -= 0xF000;
        return ram->wramNN;
    }
    // Sprite Attibute Table
    else if(*address >= 0xFE00 && *address <= 0xFE9F) {
        *address -= 0xFE00;
        return ram->oam;
    }
    // IO Registers
    else if(*address >= 0xFF00 && *address <= 0xFF7F) {
        *address -= 0xFF00;
        return ram->io;
    }
    // High RAM
    else if(*address >= 0xFF80 && *address <= 0xFFFE) {
        *address -= 0xFF80;
        return ram->hram;
    }
    // Interrupt Enable Register
    else if(*address == 0xFFFF) {
        *address = 0;
        return ram->ier;
    }

    return NULL;
}

// Checks if the ram address is valid and can be read / written
bool is_valid_ram(gbc_ram *ram, const unsigned short address) {

    // If the memory is unusable
    if(address >= 0xFEA0 && address <= 0xFEFF) {
        return false;
    }

    // If the memory is in extram and it is not available
    if(address >= 0xA000 && address <= 0xBFFF && ram->extram == NULL) {
        return false;
    }

    return true;
}

// Reads a byte from the specified location in memory
unsigned char read_byte(gbc_ram *ram, const unsigned short address) {

    if(!is_valid_ram(ram, address)) {
        return 0x0;
    }

    // Get the memory location and the relative address inside this memory
    unsigned short rel_address = address;
    unsigned char *mem = get_memory_location(ram, &rel_address);

    // Return the byte from memory
    return mem[rel_address];
}

// Reads a short (2 bytes) from the specified location in memory
// Returns the LS byte first
unsigned short read_short(gbc_ram *ram, const unsigned short address) {

    const unsigned char byte_a = read_byte(ram, address);
    const unsigned char byte_b = read_byte(ram, address + 1);

    return byte_b << 8 | byte_a;
}

// Writes a byte in memory at the address
void write_byte(gbc_ram *ram, const unsigned short address, unsigned char value, const bool is_program) {

    if(!is_valid_ram(ram, address)) {
        return;
    }

    // Serial bus
    // if(address == SB) {
    //     printf("%c", value);
    // }

    // If the timer divider register is written to, it is reset
    // Unless we are forcing the write
    if(address == DIV && is_program) {
        value = 0x0;
    }

    // Get the memory location with the relative address
    unsigned short rel_address = address;
    unsigned char *mem = get_memory_location(ram, &rel_address);

    // TODO: Implement bank switching
    
    // Write the byte to memory
    mem[rel_address] = value;
}

// Writes two bytes in memory starting at the address
void write_short(gbc_ram *ram, const unsigned short address, const unsigned short value) {

    // Split the short into two bytes
    unsigned char byte_a = (value & 0x00FF);
    unsigned char byte_b = (value & 0xFF00) >> 8;

    write_byte(ram, address, byte_a, true);
    write_byte(ram, address + 1, byte_b, true);
}

// Writes to memory register at a certain location
void write_register(gbc_ram *ram, const unsigned short address, const unsigned char bit, const unsigned char value) {
    
    unsigned char byte = read_byte(ram, address);
    byte ^= (-value ^ byte) & (1 << bit);
    write_byte(ram, address, byte, 0);
}

// Reads a memory register at a certain location
unsigned char read_register(gbc_ram *ram, const unsigned short address, const unsigned char bit) {
   
    // Return the nth bit of the register
    const unsigned char byte = read_byte(ram, address);
    return (byte >> bit) & 1;
}
