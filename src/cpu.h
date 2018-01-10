#define DEFAULT_PC 0x100
#define DEFAULT_SP 0xFFFE

#define INSTRUCTION_COUNT 256
#define CB_INSTRUCTION_COUNT 256

#define FLAG_ZERO 'Z'
#define FLAG_SUBTRACT 'N'
#define FLAG_HALFCARRY 'H'
#define FLAG_CARRY 'C'

struct gbc_registers {

    // Combined register idea thanks to
    // https://cturt.github.io/cinoop.html
    union {
        struct {
            unsigned char F;
            unsigned char A;
        };
        unsigned short AF;
    };

    union {
        struct {
            unsigned char C;
            unsigned char B;
        };
        unsigned short BC;
    };

    union {
        struct {
            unsigned char E;
            unsigned char D;
        };
        unsigned short DE;
    };

    union {
        struct {
            unsigned char L;
            unsigned char H;
        };
        unsigned short HL;
    };

    unsigned short PC; // Program Counter
    unsigned short SP; // Stack Pointer
};

struct gbc_instr {
    char *disassembly;
    char length;
    char cycles;
    char increment_PC;
    void *execute;
};

void init_cpu(struct gbc_system **);
struct gbc_instr find_instr(unsigned char, struct gbc_system **);
void execute_instr(struct gbc_system **);

void incr_byte(unsigned char *, unsigned char *);
void decr_byte(unsigned char *, unsigned char *);

void set_flag(char, unsigned char, unsigned char *);
