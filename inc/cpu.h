#pragma once

// Default Register Values (Original GameBoy)
#define DEFAULT_PC 0x0
#define PROGRAM_START 0x100
#define DEFAULT_SP 0xFFFE
#define DEFAULT_AF 0x1B0
#define DEFAULT_BC 0x13
#define DEFAULT_DE 0xD8
#define DEFAULT_HL 0x14D

// F Register Flags
#define FLAG_ZERO 'Z'
#define FLAG_SUBTRACT 'N'
#define FLAG_HALFCARRY 'H'
#define FLAG_CARRY 'C'

// Interupt Addresses
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

// CPU Timer
#define CLOCK_SPEED 4194304

// Interrupt Registers
#define IE 0xFFFF
#define IF 0xFF0F
#define IEF_VBLANK 0
#define IEF_LCD_STAT 1
#define IEF_TIMER 2
#define IEF_SERIAL 3
#define IEF_JOYPAD 4

#define DEFAULT_IF 0xE0

// Instruction Set
#define INSTRUCTION_COUNT 256
#define CB_INSTRUCTION_COUNT 256


typedef struct gbc_instruction {
    char *disassembly;
    uint8_t length;
    uint8_t clocks;
    bool signed_operand;
    void *execute;
} gbc_instruction;

void init_cpu(gbc_cpu *);
gbc_instruction find_instr(const uint8_t, const uint16_t, gbc_system *);
uint8_t execute_instr(gbc_system *);

void set_flag(const char, const uint8_t, uint8_t *);
char get_flag(const char, const uint8_t);

void stack_push_byte(gbc_system *, uint16_t *, const uint8_t);
void stack_push_short(gbc_system *, uint16_t *, const uint16_t);
uint8_t stack_pop_byte(gbc_system *, uint16_t *);
uint16_t stack_pop_short(gbc_system *, uint16_t *);

void check_interrupts(gbc_system *);
void update_timer(gbc_system *, uint8_t);