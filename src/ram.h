#define ROM_BANK_SIZE 16384
#define VRAM_SIZE 8192
#define EXRAM_SIZE 8192
#define WRAM_BANK_SIZE 4096
#define OAM_SIZE 159
#define IO_SIZE 128
#define HRAM_SIZE 128

struct gbc_ram {
    unsigned char **rom00; // 16KB ROM Bank
    unsigned char **romNN; // 16KB Switchable ROM Bank
    unsigned char *vram; // 8KB Video RAM
    unsigned char *extram; // 8KB External Ram (cartridge)
    unsigned char *wram00; // 4KB Work RAM bank 0
    unsigned char *wramNN; // 4KB Work RAM bank 1-7 (switchable SGB only) 
    //echo 8KB Same as C000-DDFF
    unsigned char *oam; // 1.59KB Sprite Attribute Table
    unsigned char *io; // 128B IO Ports
    unsigned char *hram; // 128B High RAM
    unsigned char *ier; // 1B Interrupt Enable Register

    unsigned char wram_bank;
    unsigned char rom_bank;
};

void init_ram(struct gbc_ram **);
unsigned char **get_memory_location(struct gbc_ram **, unsigned short *);

unsigned char read_byte(struct gbc_ram **, unsigned short);
unsigned short read_short(struct gbc_ram **, unsigned short);

void write_byte(struct gbc_ram **, unsigned short, unsigned char);
void write_short(struct gbc_ram **, unsigned short, unsigned short);
