#include "lxgbc.h"
#include "ram.h"
#include "rom.h"

static void get_rom_info(unsigned char *, gbc_rom *);
void load_bootrom(gbc_ram *);


// Load a ROM file into the RAM memory location
bool load_rom(gbc_system *gbc, const char *path) {

    FILE *file;
    file = fopen(path, "r");

    if(!file) {
        return false;
    }

    // Allocate temp memory and copy just the header 
    unsigned char *header = malloc(sizeof(char) * (ROM_HEADER_END - ROM_HEADER_START)); 
    fseek(file, ROM_HEADER_START, SEEK_SET);
    int character;

    for(int i = ROM_HEADER_START; i < ROM_HEADER_END; i++) {

        // If we cannot read this location, then the file is invalid
        if((character = fgetc(file)) == EOF) {
            return false;
        }
        
        header[i - ROM_HEADER_START] = (unsigned char) character;
    }

    // Read the header and see how many ROM banks are required
    get_rom_info(header, gbc->rom);
    free(header);

    gbc->rom->rom_banks = malloc(sizeof(char *) * gbc->rom->rom_size); 
    for(int i = 0; i < gbc->rom->rom_size; i++) {
        gbc->rom->rom_banks[i] = calloc(ROM_BANK_SIZE, sizeof(char)); 
    }
    
    gbc->rom->ram_banks = malloc(sizeof(char *) * gbc->rom->ram_size);
    for(int i = 0; i < gbc->rom->ram_size; i++) {
        gbc->rom->ram_banks[i] = calloc(EXTRAM_BANK_SIZE, sizeof(char));
    }
    
    // Return to the start of the file
    fseek(file, 0, SEEK_SET);
    int position = 0;

    // Copy the full ROM file to memory
    while((character = fgetc(file)) != EOF) {
        
        unsigned char bank = floor(position / ROM_BANK_SIZE);
        
        gbc->rom->rom_banks[bank][position - (ROM_BANK_SIZE * bank)] = character;
        position++; 
    }
    fclose(file);
    
    // Point the cartridge ram locations to the first rom banks
    // With ROM0 being fixed at the first rom bank
    // And romNN starting with the second one
    gbc->rom->curr_rom_bank = 1;
    gbc->rom->curr_ram_bank = 0;

    gbc->ram->rom00 = gbc->rom->rom_banks[0];
    gbc->ram->romNN = gbc->rom->rom_banks[1];

    // Point the extram to the first bank if applicable
    if(gbc->rom->ram_size > 0) {
        gbc->ram->extram = gbc->rom->ram_banks[0];
    }

    gbc->ram->bootrom = malloc(sizeof(char) * 0x100);
    load_bootrom(gbc->ram);

    return true;
}

// Parse the ROM header and get its information
static void get_rom_info(unsigned char *header, gbc_rom *rom) {

    // Read the title from the header    
    // The title is 16 characters maximum, uppercase ASCII
    rom->title = calloc(TITLE_LENGTH + 1, sizeof(char));

    for(int i = 0; i < TITLE_LENGTH; i++) {
        rom->title[i] = header[0x134 - ROM_HEADER_START + i];
    }

    rom->title[strlen(rom->title)] = '\0';

    rom->cgb_flag = header[0x143 - ROM_HEADER_START]; 
    rom->cart_type = header[0x147 - ROM_HEADER_START];

    switch(rom->cart_type) {
        case 1:
        case 2:
        case 3:
            rom->mbc_type = 1;
            break;
        case 5:
        case 6:
            rom->mbc_type = 2;
            break;
    }

    rom->rom_size = (2*ROM_BANK_SIZE << header[0x148 - ROM_HEADER_START]) / ROM_BANK_SIZE;
    
    switch(header[0x149 - ROM_HEADER_START]) {
        case 0x0: rom->ram_size = 0; break;
        case 0x1: case 0x2: rom->ram_size = 1; break;
        case 0x3: rom->ram_size = 4; break;
    }
    
    rom->dest_code = header[0x14A - ROM_HEADER_START];
    rom->ver_no = header[0x14C - ROM_HEADER_START];
}

// Display the ROM information in the console
void print_rom_info(gbc_rom *rom) {
    
    printf(CMAG "Title: " CNRM "%s\n", rom->title);
    printf(CMAG "CGB Flag: " CNRM "%02X\n", rom->cgb_flag);
    printf(CMAG "Cartridge Type: " CNRM "%02X\n", rom->cart_type);
    printf(CMAG "ROM Size: " CNRM "%d x %d KB\n", rom->rom_size, ROM_BANK_SIZE);
    printf(CMAG "RAM Size: " CNRM "%d x %d KB\n", rom->ram_size, EXTRAM_BANK_SIZE);
    printf(CMAG "MBC Type: " CNRM "MBC %d\n", rom->mbc_type);
    printf(CMAG "Destination Code: " CNRM "%02X\n", rom->dest_code);
    printf(CMAG "Version Number: " CNRM "%02X\n", rom->ver_no);
}

void load_bootrom(gbc_ram *ram) {

    // Modified DMG bootrom with the cartridge check removed
    static const unsigned char bootrom[0x100] = {
        0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
        0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
        0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
        0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
        0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
        0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
        0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
        0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
        0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
        0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
        0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
        0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
        0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
        0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C,
        0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x01, 0xE0, 0x50
    };

    memcpy(ram->bootrom, bootrom, 0x100);
}