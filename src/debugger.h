#define DBOX_COUNT 4

#define DEBUG_WIDTH 64

#define DBOX_INSTR_ROWS 13
#define DBOX_INSTR_WIDTH 28

#define DBOX_REGIS_ROWS 12
#define DBOX_REGIS_WIDTH 8

#define DBOX_FLAGS_ROWS 12
#define DBOX_FLAGS_WIDTH 8

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

struct debug_box {
    int width;
    int height;
    char **rows;
};

void print_debug(struct gbc_system **);
void print_separator(int);