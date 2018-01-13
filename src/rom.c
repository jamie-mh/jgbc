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

    return 1;
}
