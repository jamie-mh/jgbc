#define VRAM_SIZE 8192
#define EXTRAM_BANK_SIZE 8192
#define WRAM_BANK_SIZE 4096
#define OAM_SIZE 159
#define IO_SIZE 128
#define HRAM_SIZE 128

#define WRAM_BANK_COUNT 8

// LCD Registers
#define REG_LDCD 0xFF40
#define REG_STAT 0xFF41
#define REG_SCY 0xFF42
#define REG_SCX 0xFF43
#define REG_LY 0xFF44
#define REG_LYC 0xFF45
#define REG_WY 0xFF4A
#define REG_WX 0xFF4B
#define REG_BGP 0xFF47
#define REG_OBP0 0xFF48
#define REG_OBP1 0xFF49
#define DMA 0xFF46

#define REG_BCPS 0xFF68
#define REG_BCPD 0xFF69
#define REG_OCPS 0xFF6A
#define REG OCPD 0xFF6B
#define VBK 0xFF4F
#define HDMA1 0xFF51
#define HDMA2 0xFF52
#define HDMA3 0xFF53
#define HDMA4 0xFF54
#define HDMA5 0xFF55

// Sound Registers
// TODO: Implement

// Joypad Register
#define JOYP 0xFF00

// Timer and Divider Registers
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07

// GBC Only Registers
#define KEY1 0xFF4D
#define SVBK 0xFF70

// Interrupt Registers
#define REG_IE 0xFFFF
#define REG_IF 0xFF0F

struct gbc_ram {
    unsigned char *rom00; // 16KB ROM Bank
    unsigned char *romNN; // 16KB Switchable ROM Bank
    unsigned char *vram; // 8KB Video RAM
    unsigned char *extram; // 8KB External Ram (cartridge)
    unsigned char *wram00; // 4KB Work RAM bank 0
    unsigned char *wramNN; // 4KB Work RAM bank 1-7 (switchable) 
    unsigned char *oam; // 1.59KB Sprite Attribute Table
    unsigned char *io; // 128B IO Ports
    unsigned char *hram; // 128B High RAM
    unsigned char *ier; // 1B Interrupt Enable Register

    unsigned char **wram_banks; // 8x4KB WRAM Banks (CGB Only)
};

void init_ram(struct gbc_ram *);
static unsigned char *get_memory_location(struct gbc_ram *, unsigned short *);

unsigned char read_byte(struct gbc_ram *, const unsigned short);
unsigned short read_short(struct gbc_ram *, const unsigned short);

void write_byte(struct gbc_ram *, const unsigned short, const unsigned char);
void write_short(struct gbc_ram *, const unsigned short, const unsigned short);
