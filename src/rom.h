#define WRAM_BANK_COUNT 7
#define ROM_BANK_COUNT 7

struct gbc_rom {
    unsigned short title;
    unsigned char indicator;
    unsigned char cartridge_type;

    unsigned char **wram_banks; // Up to 7 x 4KB RAM banks
    unsigned char **rom_banks; // Up to 7 x 16KB ROM banks
};

int load_rom(struct gbc_ram **, struct gbc_rom **, char *);