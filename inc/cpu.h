#pragma once

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

#define INT_VBLANK 0x40
#define INT_LCD_STAT 0x48
#define INT_TIMER 0x50
#define INT_SERIAL 0x58
#define INT_JOYPAD 0x60 

typedef struct gbc_instruction {
    char *disassembly;
    char length;
    char clocks;
    char signed_operand;
    void *execute;
} gbc_instruction;

void init_cpu(gbc_cpu *);
gbc_instruction find_instr(const unsigned char, const unsigned short, gbc_system *);
void cpu_do_clock(gbc_system *);

void set_flag(const char, const unsigned char, unsigned char *);
char get_flag(const char, const unsigned char);

void stack_push_byte(gbc_ram *, unsigned short *, const unsigned char);
void stack_push_short(gbc_ram *, unsigned short *, const unsigned short);
unsigned char stack_pop_byte(gbc_ram *, unsigned short *);
unsigned short stack_pop_short(gbc_ram *, unsigned short *);

void check_interrupt(gbc_system *);