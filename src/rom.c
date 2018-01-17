#include "lxgbc.h"
#include "ram.h"
#include "rom.h"

// Load a ROM file into the RAM memory location
char load_rom(struct gbc_ram *ram, struct gbc_rom *rom, const char *path) {

    // Open the file at the specified path
    FILE *file;
    file = fopen(path, "r");
    if(!file) return 0;

    // Allocate copy just the header to temporary memory
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
    get_rom_info(header, rom);
    free(header);

    // Allocate memory for all the rom banks
    rom->rom_banks = malloc(sizeof(char *) * rom->rom_size); 
    for(int i = 0; i < rom->rom_size; i++) {
        rom->rom_banks[i] = calloc(ROM_BANK_SIZE, sizeof(char)); 
    }
    
    // Allocate memory for all the ext ram banks
    rom->ram_banks = malloc(sizeof(char *) * rom->ram_size);
    for(int i = 0; i < rom->ram_size; i++) {
        rom->ram_banks[i] = calloc(EXTRAM_BANK_SIZE, sizeof(char));
    }
    
    // Point the cartridge ram locations to the first rom banks
    // With ROM0 being fixed at the first rom bank
    // And romNN starting with the second one
    ram->rom00 = rom->rom_banks[0];
    ram->romNN = rom->rom_banks[1];

    // Return to the start of the file
    fseek(file, 0, SEEK_SET);
    unsigned short address = 0;

    // Copy the full ROM file to memory
    while((character = fgetc(file)) != EOF) {
        
        // TODO: Implement bank switching based on MBC type
        
        // Write the character to ram
        write_byte(ram, address, (unsigned char) character);
        address++; 
    }
    fclose(file);

    return 1;
}

// Parse the ROM header and get its information
static void get_rom_info(unsigned char *header, struct gbc_rom *rom) {

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
void print_rom_info(struct gbc_rom *rom) {
    
    // Print the rom information
    printf(CYEL "Title: " CNRM "%s\n", rom->title);
    printf(CYEL "CGB Flag: " CNRM "%02X\n", rom->cgb_flag);
    printf(CYEL "Cartridge Type: " CNRM "%02X\n", rom->cart_type);
    printf(CYEL "ROM Size: " CNRM "%d x %d KB\n", rom->rom_size, ROM_BANK_SIZE);
    printf(CYEL "RAM Size: " CNRM "%d x %d KB\n", rom->ram_size, EXTRAM_BANK_SIZE);
    printf(CYEL "Destination Code: " CNRM "%02X\n", rom->dest_code);
    printf(CYEL "Version Number: " CNRM "%02X\n", rom->ver_no);
}
