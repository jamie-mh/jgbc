#pragma once

#define CPU_STEP 4

// Shortcut Macros
#define TICK(T) gb->cpu.ticks += CPU_STEP * T
#define REG(N) gb->cpu.reg.N

#define READ8(addr) read_byte(gb, (addr), true)
#define WRITE8(addr, value) write_byte(gb, (addr), (value), true)
#define READ16(addr) read_short(gb, (addr), true)
#define WRITE16(addr, value) write_short(gb, (addr), (value), true)

#define SET_FLAG(flag, value) set_flag(flag, value, &REG(F)) 
#define GET_FLAG(flag) get_flag(flag, REG(F)) 

#define PUSH8(val) stack_push_byte(gb, val)
#define POP8() stack_pop_byte(gb)
#define PUSH16(val) stack_push_short(gb, val)
#define POP16() stack_pop_short(gb)
#define PEEK8() stack_peek_byte(gb)
#define PEEK16() stack_peek_short(gb)

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

// Instruction Set
#define INSTRUCTION_COUNT 256
#define CB_INSTRUCTION_COUNT 256

// F Register Flags
#define FLAG_ZERO 7 
#define FLAG_SUBTRACT 6 
#define FLAG_HALFCARRY 5 
#define FLAG_CARRY 4 


typedef struct {
    char *disassembly;
    uint8_t length;
    bool signed_operand;
    bool extended;
    void *execute;
} 
Instruction;

void init_cpu(GameBoy *gb);
Instruction find_instr(GameBoy *gb, const uint16_t address);
void execute_instr(GameBoy *);

void stack_push_byte(GameBoy *gb, const uint8_t value);
void stack_push_short(GameBoy *gb, const uint16_t value);
uint8_t stack_pop_byte(GameBoy *gb);
uint16_t stack_pop_short(GameBoy *gb);
uint8_t stack_peek_byte(GameBoy *gb);
uint16_t stack_peek_short(GameBoy *gb);

void set_flag(const uint8_t flag, const uint8_t value, uint8_t *regis);
uint8_t get_flag(const uint8_t flag, const uint8_t regis);

void check_interrupts(GameBoy *gb);
void update_timer(GameBoy *gb);