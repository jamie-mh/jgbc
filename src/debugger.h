#pragma once

#define DBOX_COUNT 4

#define DEBUG_WIDTH 64

#define DBOX_INSTR_ROWS 14
#define DBOX_INSTR_WIDTH 28

#define DBOX_REGIS_ROWS 14
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
void debug(gbc_system *);
