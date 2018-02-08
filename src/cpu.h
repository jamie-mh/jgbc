#define DEFAULT_PC 0x100
#define DEFAULT_SP 0xFFFE
#define DEFAULT_AF 0x1B0
#define DEFAULT_BC 0x13
#define DEFAULT_DE 0xD8
#define DEFAULT_HL 0x14D

#define INSTRUCTION_COUNT 256
#define CB_INSTRUCTION_COUNT 256

#define FLAG_ZERO 'Z'
#define FLAG_SUBTRACT 'N'
#define FLAG_HALFCARRY 'H'
#define FLAG_CARRY 'C'

typedef struct gbc_registers {

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
    unsigned char IME; // Interrupt Master Enable
} gbc_registers;

typedef struct gbc_instr {
    char *disassembly;
    char length;
    char cycles;
    char increment_PC;
    char signed_operand;
    void *execute;
} gbc_instr;

void init_cpu(gbc_system *);
gbc_instr find_instr(const unsigned char, gbc_system *);
void execute_instr(gbc_system *);

static void set_flag(const char, const unsigned char, unsigned char *);
static char get_flag(const char, const unsigned char);

static void stack_push(const unsigned char, gbc_ram *, unsigned short **);
static unsigned char stack_pop(gbc_ram *, unsigned short **);
