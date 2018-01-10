#include "lxgbc.h"
#include "ram.h"

// Allocates memory
void init_ram(struct gbc_ram **ram) {

    *ram = malloc(sizeof(**ram));

    (*ram)->vram = calloc(VRAM_SIZE, sizeof(char));
    (*ram)->wram00 = calloc(WRAM_BANK_SIZE, sizeof(char));

    // Allocate the switchable ram bank directly as we are not emulating a SGB
    (*ram)->wramNN = calloc(WRAM_BANK_SIZE, sizeof(char));

    (*ram)->oam = calloc(OAM_SIZE, sizeof(char));
    (*ram)->io = calloc(IO_SIZE, sizeof(char));
    (*ram)->hram = calloc(HRAM_SIZE, sizeof(char));
    (*ram)->ier = calloc(1, sizeof(char));

    (*ram)->wram_bank = 1;
    (*ram)->rom_bank = 1;
}

// Returns a pointer to the memory location of the specified address
// and changes the input address to the relative location inside this section.
unsigned char **get_memory_location(struct gbc_ram **ram, unsigned short *address) {

    // 16KB ROM Bank 00 
    if(*address >= 0x0 && *address <= 0x3FFF) {
        return (*ram)->rom00;
    } 
    // 16KB ROM Bank NN
    else if(*address >= 0x4000 && *address <= 0x7FFF) {
        *address -= 0x4000;
        return (*ram)->romNN;
    } 
    // 8KB Video RAM
    else if(*address >= 0x8000 && *address <= 0x9FFF) {
        *address -= 0x8000;
        return &(*ram)->vram;
    } 
    // 8KB External RAM (in cartridge)
    else if(*address >= 0xA000 && *address <= 0xBFFF) {
        *address -= 0xC000;
        return &(*ram)->extram;
    }
    // 4KB Work RAM Bank 00
    else if(*address >= 0xC000 && *address <= 0xCFFF) {
        *address -= 0xC000;
        return &(*ram)->wram00;
    } 
    // 4KB Work RAM Bank NN
    else if(*address >= 0xD000 && *address <= 0xDFFF) {
        *address -= 0xD000;
        return &(*ram)->wramNN;
    } 
    // Mirror of Work RAM
    else if(*address >= 0xE000 && *address <= 0xFDFF) {
        *address -= 0x2000;
        return &(*ram)->wram00;
    }
    // Sprite Attibute Table
    else if(*address >= 0xFE00 && *address <= 0xFE9F) {
        *address -= 0xFE00;
        return &(*ram)->oam;
    }
    // IO Registers
    else if(*address >= 0xFF00 && *address <= 0xFF7F) {
        *address -= 0xFF00;
        return &(*ram)->io;
    }
    // High RAM
    else if(*address >= 0xFF80 && *address <= 0xFFFE) {
        *address -= 0xFF80;
        return &(*ram)->hram;
    } 
    // Interrupt Enable Register
    else if(*address == 0xFFFF) {
        *address = 0;
        return &(*ram)->ier;
    }

}

// Reads a byte from the specified location in memory
unsigned char read_byte(struct gbc_ram **ram, unsigned short address) {

    // Get the memory location and the relative address inside this memory
    unsigned short rel_address = address;
    unsigned char **mem = get_memory_location(ram, &rel_address);

    // Return the byte from memory
    return (*mem)[rel_address];
}

// Reads a short (2 bytes) from the specified location in memory
// Returns the LS byte first
unsigned short read_short(struct gbc_ram **ram, unsigned short address) {

    // Get the two consecutive bytes
    unsigned char byte_a = read_byte(ram, address);
    unsigned char byte_b = read_byte(ram, address + 1);

    // Combine and return the result
    return byte_b << 8 | byte_a;
}

// Writes a byte in memory at the address
void write_byte(struct gbc_ram **ram, unsigned short address, unsigned char value) {
    
    // Get the memory location with the relative address
    unsigned short rel_address = address;

    unsigned char **mem = get_memory_location(ram, &rel_address);

    // Write the byte in memory
    (*mem)[rel_address] = value;
}

// Writes two bytes in memory starting at the address
void write_short(struct gbc_ram **ram, unsigned short address, unsigned short value) {

    // Split the short into two bytes
    unsigned char byte_a = (value & 0x00FF);
    unsigned char byte_b = (value & 0xFF00) >> 8;

    // Write the two bytes
    write_byte(ram, address, byte_a);
    write_byte(ram, address + 1, byte_b);
}
