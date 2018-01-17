#define WRAM_BANK_COUNT 7
#define ROM_BANK_COUNT 7

struct gbc_rom {
    char *title;
    char is_cgb;
    unsigned char cartridge_type;
    unsigned char **rom_banks; // Up to 7 x 16KB ROM banks
};

char load_rom(struct gbc_ram *, struct gbc_rom *, const char *);
static void get_rom_info(struct gbc_ram *, struct gbc_rom *);
void print_rom_info(struct gbc_rom *);
