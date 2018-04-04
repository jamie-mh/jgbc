#include "lxgbc.h"
#include "ram.h"
#include "rom.h"

static void get_rom_info(unsigned char *, gbc_rom *);


// Load a ROM file into the RAM memory location
char load_rom(gbc_system *gbc, const char *path) {

    // Open the file at the specified path
    FILE *file;
    file = fopen(path, "r");
    if(!file) return 0;

    // Allocate temp memory and copy just the header 
    unsigned char *header = malloc(sizeof(char) * (ROM_HEADER_END - ROM_HEADER_START)); 
    fseek(file, ROM_HEADER_START, SEEK_SET);
    int character;

    for(int i = ROM_HEADER_START; i < ROM_HEADER_END; i++) {

        // If we cannot read this location, then the file is invalid
        if((character = fgetc(file)) == EOF) {
            return 0;
        }
        
        // Add the byte to the header memory
        header[i - ROM_HEADER_START] = (unsigned char) character;
    }

    // Read the header and see how many ROM banks are required
    get_rom_info(header, gbc->rom);
    free(header);

    // Allocate memory for all the rom banks
    gbc->rom->rom_banks = malloc(sizeof(char *) * gbc->rom->rom_size); 
    for(int i = 0; i < gbc->rom->rom_size; i++) {
        gbc->rom->rom_banks[i] = calloc(ROM_BANK_SIZE, sizeof(char)); 
    }
    
    // Allocate memory for all the ext ram banks
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
        
        // Write the character to ram
        gbc->rom->rom_banks[bank][position - (ROM_BANK_SIZE * bank)] = character;
        position++; 
    }
    fclose(file);
    
    // Point the cartridge ram locations to the first rom banks
    // With ROM0 being fixed at the first rom bank
    // And romNN starting with the second one
    gbc->ram->rom00 = gbc->rom->rom_banks[0];
    gbc->ram->romNN = gbc->rom->rom_banks[1];

    return 1;
}

// Parse the ROM header and get its information
static void get_rom_info(unsigned char *header, gbc_rom *rom) {

    // Read the title from the header    
    // The title is 16 characters maximum, uppercase ASCII
    char *title = calloc(sizeof(char), 16);
    int i;

    for(i = 0; i < 16; i++) {
        title[i] = header[0x134 - ROM_HEADER_START + i];
    }

    title[strlen(title)] = '\0';
    rom->title = malloc(sizeof(char) * strlen(title) + 1);
    strcpy(rom->title, title);
    free(title);

    // Get the rest of the cartridge information 
    rom->cgb_flag = header[0x143 - ROM_HEADER_START]; 
    rom->cart_type = header[0x147 - ROM_HEADER_START];
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
    
    // Print the rom information
    printf(CMAG "Title: " CNRM "%s\n", rom->title);
    printf(CMAG "CGB Flag: " CNRM "%02X\n", rom->cgb_flag);
    printf(CMAG "Cartridge Type: " CNRM "%02X\n", rom->cart_type);
    printf(CMAG "ROM Size: " CNRM "%d x %d KB\n", rom->rom_size, ROM_BANK_SIZE);
    printf(CMAG "RAM Size: " CNRM "%d x %d KB\n", rom->ram_size, EXTRAM_BANK_SIZE);
    printf(CMAG "Destination Code: " CNRM "%02X\n", rom->dest_code);
    printf(CMAG "Version Number: " CNRM "%02X\n", rom->ver_no);
}
