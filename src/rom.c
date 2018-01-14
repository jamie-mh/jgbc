#include "lxgbc.h"
#include "ram.h"
#include "rom.h"

char load_rom(struct gbc_ram **ram, struct gbc_rom **rom, const char *path) {

    // Read the file at the specified path
    FILE *file;
    file = fopen(path, "r");
    (*rom)->rom_banks = NULL;

    int byte;
    int index = 0;
    char bank = -1;

    if(file) {

        // Read the file to the end
        while ((byte = fgetc(file)) != EOF) {

            // Switch banks and allocate sufficent memory
            // Usually cartidges hold 32KB, so there will be two banks of 16KB
            if(index == ROM_BANK_SIZE || index == 0) {
                bank++;
                index = 0;

                (*rom)->rom_banks = realloc((*rom)->rom_banks, bank + 1);
                (*rom)->rom_banks[bank] = calloc(ROM_BANK_SIZE, sizeof(char));
            }
            // Add the byte to memory
            (*rom)->rom_banks[bank][index] = (unsigned char) byte;

            index++;
        }

        fclose(file);
    } else {
        // Return false if the rom file could not be read
        return 0;
    }

    // Point the cartridge ram locations to the first rom banks
    // With ROM0 being fixed at the first rom bank
    // And romNN starting with the second one, if it exists
    (*ram)->rom00 = &(*rom)->rom_banks[0];

    if(bank >= 1) {
        (*ram)->romNN = &(*rom)->rom_banks[1];
    }
    
    // Parse the rom information
    get_rom_info(ram, rom);

    return 1;
}

static void get_rom_info(struct gbc_ram **ram, struct gbc_rom **rom) {

    // Read the title from the header    
    // The title is 16 characters maximum, uppercase ASCII
    char *title = calloc(sizeof(char), 16);
    int i;

    for(i = 0; i < 16; i++) {
        title[i] = read_byte(ram, 0x134 + i);
    }

    title[strlen(title)] = '\0';
    (*rom)->title = malloc(sizeof(char) * strlen(title) + 1);
    strcpy((*rom)->title, title);
    free(title);

    // Check if the rom has GB color features
    unsigned char cgb_byte = read_byte(ram, 0x143);
    (*rom)->is_cgb = (cgb_byte == 0x80) ? 1 : 0;
    
    // Read the cartridge type
    (*rom)->cartridge_type = read_byte(ram, 0x147);
}
