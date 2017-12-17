#define DEFAULT_PC 0x100
#define DEFAULT_SP 0xFFFE

#define INSTRUCTION_COUNT 256
#define CB_INSTRUCTION_COUNT 256

struct gbc_registers {

    // Combined register idea thanks to
    // https://cturt.github.io/cinoop.html
    union {
        struct {
            unsigned char A;
            unsigned char F;
        };
        unsigned short AF;
    };

    union {
        struct {
            unsigned char B;
            unsigned char C;
        };
        unsigned short BC;
    };

    union {
        struct {
            unsigned char D;
            unsigned char E;
        };
        unsigned short DE;
    };

    union {
        struct {
            unsigned char H;
            unsigned char L;
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
struct gbc_instr find_instr(unsigned char opcode, struct gbc_system **);
void execute_instr(struct gbc_system **);

void incr_byte(unsigned char *, unsigned char *);
void decr_byte(unsigned char *, unsigned char *);