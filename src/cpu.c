#include "cpu.h"
#include "instr.h"
#include "macro.h"
#include "mmu.h"
#include <assert.h>

static void service_interrupt(GameBoy *, uint8_t);
static void increment_tima(GameBoy *);

void reset_cpu(GameBoy *gb) {
    REG(AF) = 0x11B0;
    REG(BC) = 0x0013;
    REG(DE) = 0x00D8;
    REG(HL) = 0x014D;
    REG(PC) = PROGRAM_START;
    REG(SP) = 0xFFFE;
    REG(IME) = false;

    gb->cpu.is_halted = false;
    gb->cpu.is_double_speed = false;

    gb->cpu.div = 0;
    gb->cpu.div_overflow = false;
    gb->cpu.div_overflow_ticks = 0;
}

/*
    Instructions
*/

Instruction find_instr(GameBoy *gb, const uint16_t address) {
    const uint8_t opcode = SREAD8(address);

    if (opcode == 0xCB) {
        const uint8_t next_opcode = SREAD8(address + 1);
        return cb_instructions[next_opcode];
    } else {
        return instructions[opcode];
    }
}

void update_cpu(GameBoy *gb) {
    gb->cpu.ticks = CPU_STEP;
    tick_timer(gb, CPU_STEP);

    if (gb->cpu.is_halted) {
        return;
    }

    const Instruction instruction = find_instr(gb, REG(PC));
    uint8_t operand_len = instruction.length - 1;

    if (instruction.extended) {
        operand_len--;
        TICK(1);
    }

    const uint16_t instr_start = REG(PC);
    REG(PC) += instruction.length;

    switch (operand_len) {
    case 0: {
        const void (*opcode_function)(GameBoy *) = instruction.execute;
        opcode_function(gb);
        break;
    }

    case 1: {
        const uint8_t operand = SREAD8(instr_start + 1);
        TICK(1);

        const void (*opcode_function)(GameBoy *, const uint8_t) = instruction.execute;
        opcode_function(gb, operand);
        break;
    }

    case 2: {
        const uint16_t operand = SREAD16(instr_start + 1);
        TICK(2);

        const void (*opcode_function)(GameBoy *, const uint16_t) = instruction.execute;
        opcode_function(gb, operand);
        break;
    }

    default:
        ASSERT_NOT_REACHED();
    }
}

/*
    F Register Flags
*/

void set_flag(GameBoy *gb, const uint8_t flag, const uint8_t value) {
    assert(value <= 1);

    if (value == 0) {
        REG(F) &= ~(1 << flag);
    } else {
        REG(F) |= 1 << flag;
    }
}

uint8_t get_flag(GameBoy *gb, const uint8_t flag) { return GET_BIT(REG(F), flag); }

/*
    Stack
*/

void stack_push_byte(GameBoy *gb, const uint8_t value) {
    REG(SP)--;
    WRITE8(REG(SP), value);
}

void stack_push_short(GameBoy *gb, const uint16_t value) {
    PUSH8((value & 0xFF00) >> 8);
    PUSH8(value & 0x00FF);
}

uint8_t stack_pop_byte(GameBoy *gb) {
    const uint8_t value = READ8(REG(SP));
    REG(SP)++;

    return value;
}

uint16_t stack_pop_short(GameBoy *gb) {
    const uint8_t byte_a = POP8();
    const uint8_t byte_b = POP8();

    return byte_a | (byte_b << 8);
}

uint8_t stack_peek_byte(GameBoy *gb) { return SREAD8(REG(SP)); }

uint16_t stack_peek_short(GameBoy *gb) {
    const uint8_t byte_a = SREAD8(REG(SP));
    const uint8_t byte_b = SREAD8(REG(SP) + 1);

    return byte_a | (byte_b << 8);
}

/*
    Interrupts
*/

void check_interrupts(GameBoy *gb) {
    const uint8_t enable = SREAD8(IE);
    const uint8_t request = SREAD8(IF);

    for (uint8_t i = 0; i < 5; i++) {
        if (GET_BIT(enable, i) && GET_BIT(request, i)) {
            if (REG(IME)) {
                service_interrupt(gb, i);
            }

            gb->cpu.is_halted = false;
        }
    }
}

static void service_interrupt(GameBoy *gb, const uint8_t number) {
    static const uint16_t interrupt[5] = {INT_VBLANK, INT_LCD_STAT, INT_TIMER, INT_SERIAL, INT_JOYPAD};

    assert(number < 5);
    PUSH16(REG(PC));

    WREG(IF, number, 0);
    REG(IME) = false;
    REG(PC) = interrupt[number];
}

/*
    Timer
*/

void set_div(GameBoy *gb, const uint16_t value) {
    const uint8_t speed_index = SREAD8(TAC) & 0x3;
    static const uint8_t div_bit_pos[] = { 9, 3, 5, 7 };

    const uint8_t old_mult_val = GET_BIT(gb->cpu.div, div_bit_pos[speed_index]);
    const uint8_t new_mult_val = GET_BIT(value, div_bit_pos[speed_index]);

    const uint8_t tac_state = RREG(TAC, TAC_STOP);
    const bool old_output = old_mult_val & tac_state;
    const bool current_output = new_mult_val & tac_state;

    // Falling edge of DIV bit, increment TIMA
    if (old_output && !current_output) {
        increment_tima(gb);
    }

    // The divider register updates at one 256th of the clock speed (aka 256 clocks)
    // Write the top half of the 16 bit internal divider register
    SWRITE8(DIV, (value & 0xFF00) >> 8);

    gb->cpu.div = value;
}

static void increment_tima(GameBoy *gb) {
    const uint8_t tima = SREAD8(TIMA);

    if (tima == 255) {
        gb->cpu.div_overflow = true;
        gb->cpu.div_overflow_ticks = 0;
        SWRITE8(TIMA, 0);
    } else {
        SWRITE8(TIMA, tima + 1);
    }
}

void tick_timer(GameBoy *gb, const uint8_t ticks) {
    for (uint8_t i = 0; i < ticks; ++i) {
        set_div(gb, gb->cpu.div + 1);

        if (gb->cpu.div_overflow) {
            if (gb->cpu.div_overflow_ticks >= CPU_STEP) {
                const uint8_t tma = SREAD8(TMA);
                SWRITE8(TIMA, tma);
                WREG(IF, IEF_TIMER, 1);
                gb->cpu.div_overflow = false;
            }

            gb->cpu.div_overflow_ticks++;
        }
    }
}
