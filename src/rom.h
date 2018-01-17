#define ROM_BANK_SIZE 16384

#define ROM_HEADER_START 0x100
#define ROM_HEADER_END 0x14F

struct gbc_rom {
    char *title; // Uppercase ASCII Game Name
    char cgb_flag; // Color Support Flag
    unsigned char cart_type; // Cartridge Type Code
    unsigned char rom_size; // Number of ROM banks
    unsigned char ram_size; // Number of EXT RAM banks
    unsigned char dest_code; // Destination Code (0: Japan, 1: World)
    unsigned char ver_no; // ROM Version Number

    unsigned char **rom_banks;
    unsigned char **ram_banks;
};

char load_rom(struct gbc_ram *, struct gbc_rom *, const char *);
static void get_rom_info(unsigned char *, struct gbc_rom *);
void print_rom_info(struct gbc_rom *);
