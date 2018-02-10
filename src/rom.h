#define ROM_BANK_SIZE 16384

#define ROM_HEADER_START 0x100
#define ROM_HEADER_END 0x14F

void load_bootstrap(gbc_ram *);
char load_rom(gbc_system *, const char *);
static void get_rom_info(unsigned char *, gbc_rom *);
void print_rom_info(gbc_rom *);
