#include "lxgbc.h"
#include "ppu.h"
#include "cpu.h"
#include "ram.h"
#include "mbc.h"

static unsigned char *get_memory_location(gbc_ram *, unsigned short *);
static void DMA_transfer(gbc_system *, unsigned char);


// Allocates memory
void init_ram(gbc_system *gbc) {

    gbc->ram->rom00 = NULL;
    gbc->ram->romNN = NULL;
    gbc->ram->extram = NULL;

    gbc->ram->vram = calloc(VRAM_SIZE, sizeof(char));
    gbc->ram->wram_banks = malloc(sizeof(char *) * WRAM_BANK_COUNT); 

    for(int i = 0; i < WRAM_BANK_COUNT; i++) {
        gbc->ram->wram_banks[i] = calloc(WRAM_BANK_SIZE, sizeof(char));
    }

    // Point the WRAM to the default banks
    gbc->ram->wram00 = gbc->ram->wram_banks[0];
    gbc->ram->wramNN = gbc->ram->wram_banks[1];
    
    gbc->ram->oam = calloc(OAM_SIZE, sizeof(char));
    gbc->ram->io = calloc(IO_SIZE, sizeof(char));
    gbc->ram->hram = calloc(HRAM_SIZE, sizeof(char));
    gbc->ram->ier = calloc(1, sizeof(char));

    write_byte(gbc, LCDC, DEFAULT_LCDC, 0);
}

// Returns a pointer to the memory location of the specified address
// and changes the input address to the relative location inside this section.
static unsigned char *get_memory_location(gbc_ram *ram, unsigned short *address) {

    switch(*address) {

        // 16KB ROM Bank 00 
        case ROM00_START ... ROM00_END:
            if(*address <= PROGRAM_START && !read_byte(ram, BOOTROM_DISABLE)) {
                return ram->bootrom;
            } else {
                return ram->rom00;
            }

        // 16KB ROM Bank NN
        case ROMNN_START ... ROMNN_END:
            *address -= ROMNN_START;
            return ram->romNN;

        // 8KB Video RAM
        case VRAM_START ... VRAM_END:
            *address -= VRAM_START;
            return ram->vram;

        // 8KB External RAM (in cartridge)
        case EXTRAM_START ... EXTRAM_END:
            *address -= EXTRAM_START;
            return ram->extram;

        // 4KB Work RAM Bank 00
        case WRAM00_START ... WRAM00_END:
            *address -= WRAM00_START;
            return ram->wram00;

        // 4KB Work RAM Bank NN
        case WRAMNN_START ... WRAMNN_END:
            *address -= WRAMNN_START;
            return ram->wramNN;

        // Mirror of Work RAM (wram 00)
        case WRAM00_MIRROR_START ... WRAM00_MIRROR_END:
            *address -= WRAM00_MIRROR_START;
            return ram->wram00;

        // Mirror of Work RAM (wram nn)
        case WRAMNN_MIRROR_START ... WRAMNN_MIRROR_END:
            *address -= WRAMNN_MIRROR_START;
            return ram->wramNN;

        // Sprite Attibute Table
        case OAM_START ... OAM_END:
            *address -= OAM_START;
            return ram->oam;

        // IO Registers
        case IO_START ... IO_END:
            *address -= IO_START;
            return ram->io;

        // High RAM
        case HRAM_START ... HRAM_END:
            *address -= HRAM_START;
            return ram->hram;
        
        // Interrupt Enable Register
        case IME_START_END:
            *address = 0;
            return ram->ier;

        default:
            return NULL;
    }
}

// Checks if the ram address is valid and can be read / written
bool is_valid_ram(gbc_ram *ram, const unsigned short address) {

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
void write_byte(gbc_system *gbc, const unsigned short address, unsigned char value, const bool is_program) {
    mbc_check(gbc, address, value);

    if(!is_valid_ram(gbc->ram, address)) {
        return;
    }

    // Serial bus
    // if(address == SB) {
    //     printf("%c", value);
    // }

    if(address == DMA) {
        DMA_transfer(gbc, value); 
    }

    // If the timer divider register is written to, it is reset
    // Unless we are forcing the write
    if(address == DIV && is_program) {
        value = 0x0;
    }

    // If the vertical line register is written to, it is reset
    if(address == LY && is_program) {
        value = 0x0;
    }

    // Get the memory location with the relative address
    unsigned short rel_address = address;
    unsigned char *mem = get_memory_location(gbc->ram, &rel_address);
    
    mem[rel_address] = value;
}

// Writes two bytes in memory starting at the address
void write_short(gbc_system *gbc, const unsigned short address, const unsigned short value) {

    // Split the short into two bytes
    const unsigned char byte_a = (value & 0x00FF);
    const unsigned char byte_b = (value & 0xFF00) >> 8;

    write_byte(gbc, address, byte_a, true);
    write_byte(gbc, address + 1, byte_b, true);
}

// Writes to memory register at a certain location
void write_register(gbc_system *gbc, const unsigned short address, const unsigned char bit, const unsigned char value) {
    
    unsigned char byte = read_byte(gbc->ram, address);
    byte ^= (-value ^ byte) & (1 << bit);
    write_byte(gbc, address, byte, 0);
}

// Reads a memory register at a certain location
unsigned char read_register(gbc_ram *ram, const unsigned short address, const unsigned char bit) {
   
    // Return the nth bit of the register
    const unsigned char byte = read_byte(ram, address);
    return (byte >> bit) & 1;
}

// Launches a DMA transfer from ROM or RAM to OAM memory (sprite data)
static void DMA_transfer(gbc_system *gbc, unsigned char value) {

    const unsigned short address = value * 0x100;

    for(int i = 0; i <= 0x9F; i++) {
        write_byte(gbc, 0xFE00 + i, read_byte(gbc->ram, address + i), false); 
    }
}
