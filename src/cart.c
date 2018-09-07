#include "jgbc.h"
#include "cart.h"
#include "mmu.h"

static void parse_header(GameBoy *gb, uint8_t *header);
static bool read_header(FILE *file, uint8_t *header);
static void alloc_banks(GameBoy *gb);
static void copy_data(GameBoy *gb, FILE *file);
static void set_banks(GameBoy *gb);


bool load_rom(GameBoy *gb, const char *path) {

    FILE *file = NULL;

    if(!(file = fopen(path, "rb"))) {
        return false;
    }

    uint8_t *header = malloc(CART_HEADER_SIZE * sizeof(uint8_t));

    if(!read_header(file, header)) {
        free(header);
        return false;
    }

    parse_header(gb, header);
    alloc_banks(gb);
    copy_data(gb, file);
    set_banks(gb);

    free(header);
    fclose(file);
    return true;
}

void print_cart_info(GameBoy *gb) {
    printf("Title: %s\n", gb->cart.title);
    printf("GBC Flag: %02X\n", gb->cart.gbc_flag);
    printf("Cartridge Type: %02X\n", gb->cart.type);
    printf("ROM Size: %d x %d KB\n", gb->cart.rom_size, ROM_BANK_SIZE);
    printf("RAM Size: %d x %d KB\n", gb->cart.ram_size, EXTRAM_BANK_SIZE);
}

static bool read_header(FILE *file, uint8_t *header) {
    assert(file != NULL);

    fseek(file, CART_HEADER_START, SEEK_SET);
    int32_t byte;

    for(uint16_t i = CART_HEADER_START; i < CART_HEADER_END; ++i) {
        if((byte = fgetc(file)) == EOF) {
            return false;
        }

        header[i - CART_HEADER_START] = (uint8_t) byte;
    }

    fseek(file, 0, SEEK_SET);
    return true;
}

static void parse_header(GameBoy *gb, uint8_t *header) {
    #define HEADER(addr) header[(addr) - CART_HEADER_START]

    strncpy(gb->cart.title, &HEADER(CART_HEADER_TITLE), 16);
    gb->cart.title[16] = '\0';

    gb->cart.gbc_flag = HEADER(CART_HEADER_GBC_FLAG);
    gb->cart.type = HEADER(CART_HEADER_TYPE);
    gb->cart.rom_size = (2 * ROM_BANK_SIZE << HEADER(CART_HEADER_ROM_SIZE)) / ROM_BANK_SIZE; // 32KB sl N

    switch(HEADER(CART_HEADER_RAM_SIZE)) {
        case 0x0: gb->cart.ram_size = 0; break;
        case 0x1: 
        case 0x2: gb->cart.ram_size = 1; break;
        case 0x3: gb->cart.ram_size = 4; break;
        case 0x4: gb->cart.ram_size = 16; break;
        case 0x5: gb->cart.ram_size = 8; break;
    }

    #undef HEADER
}

static void alloc_banks(GameBoy *gb) {

    gb->cart.rom_banks = malloc(sizeof(uint8_t *) * gb->cart.rom_size);
    for(uint8_t i = 0; i < gb->cart.rom_size; ++i) {
        gb->cart.rom_banks[i] = malloc(ROM_BANK_SIZE * sizeof(uint8_t));
    }

    gb->cart.ram_banks = NULL;

    if(gb->cart.ram_size > 0) {
        gb->cart.ram_banks = malloc(sizeof(uint8_t *) * gb->cart.ram_size);
        for(uint8_t i = 0; i < gb->cart.ram_size; ++i) {
            gb->cart.ram_banks[i] = malloc(EXTRAM_BANK_SIZE * sizeof(uint8_t));
        }
    }
}

static void copy_data(GameBoy *gb, FILE *file) {
    assert(file != NULL);
    int32_t byte;
    uint32_t position = 0;

    while((byte = fgetc(file)) != EOF) {
        uint8_t bank = position / ROM_BANK_SIZE;
        gb->cart.rom_banks[bank][position - (ROM_BANK_SIZE * bank)] = (uint8_t) byte;
        position++;
    }
}

static void set_banks(GameBoy *gb) {
    gb->mmu.ram_bank = -1;
    gb->mmu.rom_bank = 1;

    gb->mmu.rom00 = gb->cart.rom_banks[0];
    gb->mmu.romNN = gb->cart.rom_banks[1];
}
