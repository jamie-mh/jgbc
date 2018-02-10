#define DBOX_COUNT 4

#define DEBUG_WIDTH 64

#define DBOX_INSTR_ROWS 13
#define DBOX_INSTR_WIDTH 28

#define DBOX_REGIS_ROWS 13
#define DBOX_REGIS_WIDTH 8

#define DBOX_FLAGS_ROWS 4
#define DBOX_FLAGS_WIDTH 4

#define DBOX_INFO_ROWS 4
#define DBOX_INFO_WIDTH 7

#define RAM_DUMP_FILENAME_SIZE 128

#ifndef max
    #define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
    #define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef struct debug_box {
    int width;
    int height;
    char **rows;
} debug_box;

void init_debugger(gbc_debugger *);
static void print_debug(gbc_system *);
void debug(gbc_system *);

static char add_breakpoint(const unsigned short, gbc_debugger *);
static char remove_breakpoint(const unsigned short, gbc_debugger *);
static breakpoint *find_breakpoint(const unsigned short, gbc_debugger *);
static char dump_ram(gbc_ram *, const char *);
