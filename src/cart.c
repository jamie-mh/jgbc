#include "cart.h"
#include "mbc.h"
#include "mmu.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_COPY_APPEND(buffer, filename, ext)                                                                         \
    {                                                                                                                  \
        strcpy((buffer), (filename));                                                                                  \
        strcat((buffer), (ext));                                                                                       \
    }

static void parse_header(GameBoy *, const uint8_t *);
static bool read_header(FILE *file, uint8_t *);
static void alloc_banks(GameBoy *);
static void copy_data(GameBoy *, FILE *);
static void set_banks(GameBoy *);

bool load_rom(GameBoy *gb, const char *path) {
    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        return false;
    }

    uint8_t header[CART_HEADER_SIZE];

    if (!read_header(file, header)) {
        fclose(file);
        return false;
    }

    parse_header(gb, header);
    alloc_banks(gb);
    copy_data(gb, file);
    set_banks(gb);
    fclose(file);

#ifdef _WIN32
    const char sep = '\\';
#else
    const char sep = '/';
#endif

    char *last_sep = strrchr(path, sep);

    if (last_sep != NULL) {
        strcpy(gb->cart.filename, strrchr(path, sep) + sizeof(char));
    } else {
        strcpy(gb->cart.filename, path);
    }

    load_ram(gb);
    return true;
}

bool load_ram(GameBoy *gb) {
    if (gb->cart.ram_size == 0) {
        return true;
    }

    char filename[256 + 5];
    STR_COPY_APPEND(filename, gb->cart.filename, ".save");
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        return true;
    }

    uint8_t *buffer = malloc(EXTRAM_BANK_SIZE * sizeof(uint8_t));
    size_t bytes_read = fread(buffer, sizeof(uint8_t), EXTRAM_BANK_SIZE, file);
    fclose(file);

    if (bytes_read != EXTRAM_BANK_SIZE) {
        free(buffer);
        return false;
    }

    memcpy(gb->cart.ram_banks[0], buffer, EXTRAM_BANK_SIZE);
    free(buffer);
    return true;
}

void save_ram(GameBoy *gb) {

    if (gb->cart.ram_size == 0) {
        return;
    }

    char filename[256 + 5];
    STR_COPY_APPEND(filename, gb->cart.filename, ".save");
    FILE *file = fopen(filename, "wb");

    if (file != NULL) {
        fwrite(gb->cart.ram_banks[0], sizeof(uint8_t), EXTRAM_BANK_SIZE, file);
        fclose(file);
    }
}

void print_cart_info(GameBoy *gb) {
    printf("Title: %s\n", gb->cart.title);
    printf("Colour: %s\n", gb->cart.is_colour ? "yes" : "no");
    printf("Cartridge Type: %02X\n", gb->cart.type);
    printf("ROM Size: %d x %d KB\n", gb->cart.rom_size, ROM_BANK_SIZE);
    printf("RAM Size: %d x %d KB\n", gb->cart.ram_size, EXTRAM_BANK_SIZE);
}

static bool read_header(FILE *file, uint8_t *header) {
    assert(file != NULL);

    fseek(file, CART_HEADER_START, SEEK_SET);
    int32_t byte;

    for (uint16_t i = CART_HEADER_START; i < CART_HEADER_END; ++i) {
        if ((byte = fgetc(file)) == EOF) {
            return false;
        }

        header[i - CART_HEADER_START] = (uint8_t)byte;
    }

    fseek(file, 0, SEEK_SET);
    return true;
}

static void parse_header(GameBoy *gb, const uint8_t *header) {
#define HEADER(addr) header[(addr)-CART_HEADER_START]

    memcpy(gb->cart.title, &HEADER(CART_HEADER_TITLE), 16);
    gb->cart.title[16] = '\0';

    const uint8_t gbc_flag = HEADER(CART_HEADER_GBC_FLAG);
    gb->cart.is_colour = gbc_flag == CART_HEADER_GBC_ONLY;

    gb->cart.type = HEADER(CART_HEADER_TYPE);
    gb->cart.rom_size = (2 * ROM_BANK_SIZE << HEADER(CART_HEADER_ROM_SIZE)) / ROM_BANK_SIZE; // 32KB sl N

    switch (HEADER(CART_HEADER_RAM_SIZE)) {
    case 0x0:
        gb->cart.ram_size = 0;
        break;
    case 0x1:
    case 0x2:
        gb->cart.ram_size = 1;
        break;
    case 0x3:
        gb->cart.ram_size = 4;
        break;
    case 0x4:
        gb->cart.ram_size = 16;
        break;
    case 0x5:
        gb->cart.ram_size = 8;
        break;
    }

#undef HEADER

    switch (gb->cart.type) {
    case 0x1:
    case 0x2:
    case 0x3:
        gb->mmu.mbc_handler = &mbc1_handler;
        break;

    case 0x5:
    case 0x6:
        gb->mmu.mbc_handler = &mbc2_handler;
        break;

    case 0xF:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
        gb->mmu.mbc_handler = &mbc3_handler;
        break;

    case 0x15:
    case 0x16:
    case 0x17:
        gb->mmu.mbc_handler = &mbc4_handler;
        break;

    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
        gb->mmu.mbc_handler = &mbc5_handler;
        break;

    default:
        gb->mmu.mbc_handler = NULL;
        break;
    }
}

static void alloc_banks(GameBoy *gb) {
    gb->cart.rom_banks = malloc(sizeof(uint8_t *) * gb->cart.rom_size);

    for (uint16_t i = 0; i < gb->cart.rom_size; ++i) {
        gb->cart.rom_banks[i] = calloc(ROM_BANK_SIZE, sizeof(uint8_t));
    }

    gb->cart.ram_banks = NULL;

    if (gb->cart.ram_size > 0) {
        gb->cart.ram_banks = malloc(sizeof(uint8_t *) * gb->cart.ram_size);

        for (uint8_t i = 0; i < gb->cart.ram_size; ++i) {
            gb->cart.ram_banks[i] = calloc(EXTRAM_BANK_SIZE, sizeof(uint8_t));
        }
    }
}

static void copy_data(GameBoy *gb, FILE *file) {
    assert(file != NULL);
    int32_t byte;
    uint32_t position = 0;

    while ((byte = fgetc(file)) != EOF) {
        const uint16_t bank = position / ROM_BANK_SIZE;
        gb->cart.rom_banks[bank][position - (ROM_BANK_SIZE * bank)] = (uint8_t)byte;
        position++;
    }
}

static void set_banks(GameBoy *gb) {
    gb->mmu.ram_bank = -1;
    gb->mmu.rom_bank = 1;

    gb->mmu.rom00 = gb->cart.rom_banks[0];
    gb->mmu.romNN = gb->cart.rom_banks[1];
}
