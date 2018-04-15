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

// Timer and Divider Registers
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TAC 0xFF07

#define TAC_INPUT0 0
#define TAC_INPUT1 1
#define TAC_STOP 2

// Interrupt Registers
#define IE 0xFFFF
#define IF 0xFF0F
#define IEF_VBLANK 0
#define IEF_LCD_STAT 1
#define IEF_TIMER 2
#define IEF_SERIAL 3
#define IEF_JOYPAD 4

#define CLOCK_SPEED 4194304
#define TAC_THRESHOLD {CLOCK_SPEED / 4096, CLOCK_SPEED / 262144, CLOCK_SPEED / 65536, CLOCK_SPEED / 16384}

typedef struct gbc_instruction {
    char *disassembly;
    char length;
    char clocks;
    char signed_operand;
    void *execute;
} gbc_instruction;

void init_cpu(gbc_cpu *);
gbc_instruction find_instr(const unsigned char, const unsigned short, gbc_system *);
unsigned char execute_instr(gbc_system *);

void set_flag(const char, const unsigned char, unsigned char *);
char get_flag(const char, const unsigned char);

void stack_push_byte(gbc_ram *, unsigned short *, const unsigned char);
void stack_push_short(gbc_ram *, unsigned short *, const unsigned short);
unsigned char stack_pop_byte(gbc_ram *, unsigned short *);
unsigned short stack_pop_short(gbc_ram *, unsigned short *);

void check_interrupt(gbc_system *);
void update_timer(gbc_system *);