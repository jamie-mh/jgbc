#define DBOX_COUNT 4

#define DEBUG_WIDTH 64

#define DBOX_INSTR_ROWS 13
#define DBOX_INSTR_WIDTH 28

#define DBOX_REGIS_ROWS 12
#define DBOX_REGIS_WIDTH 8

#define DBOX_FLAGS_ROWS 4
#define DBOX_FLAGS_WIDTH 4

#define DBOX_INFO_ROWS 4
#define DBOX_INFO_WIDTH 7

#define CNRM "\x1B[0m"
#define CRED "\x1B[31m"
#define CGRN "\x1B[32m"
#define CYEL "\x1B[33m"
#define CBLU "\x1B[34m"
#define CMAG "\x1B[35m"
#define CCYN "\x1B[36m"
#define CWHT "\x1B[37m"

struct gbc_debugger {
    unsigned short *breakpoints;
};

struct debug_box {
    int width;
    int height;
    char **rows;
};

static void print_debug(struct gbc_system **, struct gbc_debugger **);
void debug(struct gbc_system **, struct gbc_debugger **);
