#include "jgbc.h"
#include "mmu.h"
#include "cpu.h"
#include "alu.h"
#include "instr.h"

static void service_interrupt(GameBoy *gb, const uint8_t number);
Instruction find_instr(GameBoy *gb, const uint16_t address);


void init_cpu(GameBoy *gb) {
    REG(AF) = 0x01B0;
    REG(BC) = 0x0013;
    REG(DE) = 0x00D8;
    REG(HL) = 0x014D;
    REG(PC) = 0x0100;
    REG(SP) = 0xFFFE;
    REG(IME) = false;

    gb->cpu.is_halted = false;
    gb->cpu.div_clock = 0;
    gb->cpu.cnt_clock = 0;
}

/*
    Instructions
*/

Instruction find_instr(GameBoy *gb, const uint16_t address) {
    const uint8_t opcode = SREAD8(address);

    if(opcode == 0xCB) {
        const uint8_t next_opcode = SREAD8(address + 1);
        return cb_instructions[next_opcode];
    } else {
        return instructions[opcode];
    }
}

void execute_instr(GameBoy *gb) {

    gb->cpu.ticks = CPU_STEP;

    if(gb->cpu.is_halted) {
        return;
    }

    Instruction instruction = find_instr(gb, REG(PC));

    void (*opcode_function)();
    opcode_function = instruction.execute;

    char operand_len = instruction.length - 1;
    uint16_t operand;

    if(instruction.extended) {
        operand_len--; 
        TICK(1);
    }

    uint16_t instr_start = REG(PC);
    REG(PC) += instruction.length;

    switch(operand_len) {

        case 0:
            opcode_function(gb);
            break;

        case 1:
            TICK(1);
            operand = read_byte(gb, instr_start + 1, false);
            opcode_function(gb, (uint8_t) operand);
            break;

        case 2:
            TICK(2);
            operand = read_short(gb, instr_start + 1, false);
            opcode_function(gb, operand);
            break;
    }
}

/*
    F Register Flags
*/

void set_flag(const uint8_t flag, const uint8_t value, uint8_t *regis) {
    assert(value <= 1);

    if(value == 0) {
        *regis &= ~(1 << flag);
    } else if(value == 1) {
        *regis |= 1 << flag;
    }
}

uint8_t get_flag(const uint8_t flag, const uint8_t regis) {
    return GET_BIT(regis, flag);
}

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

uint8_t stack_peek_byte(GameBoy *gb) {
	return SREAD8(REG(SP));
}

uint16_t stack_peek_short(GameBoy *gb) {
	const uint8_t byte_a = SREAD8(REG(SP));
	const uint8_t byte_b = SREAD8(REG(SP) + 1);

    return byte_a | (byte_b << 8);
}

/*
    Interrupts
*/

void check_interrupts(GameBoy *gb) {

    const uint8_t enable = read_byte(gb, IE, false);
    const uint8_t request = read_byte(gb, IF, false);

    for(int i = 0; i < 5; i++) {
        
        if(GET_BIT(enable, i) && GET_BIT(request, i)) {

            if(REG(IME)) {
                service_interrupt(gb, i);
            }
    
            gb->cpu.is_halted = false;
        }
    }
}

static void service_interrupt(GameBoy *gb, const uint8_t number) {
    
    static const uint16_t interrupt[5] = {
        INT_VBLANK,
        INT_LCD_STAT,
        INT_TIMER,
        INT_SERIAL,
        INT_JOYPAD
    };

    assert(number < 5);
	PUSH16(REG(PC));
    
    write_register(gb, IF, number, 0);
    REG(IME) = false;
    REG(PC) = interrupt[number];
}

/*
    Timer
*/

void update_timer(GameBoy *gb) {

    gb->cpu.div_clock += gb->cpu.ticks;

    // The divider register updates at one 256th of the clock speed (aka 256 clocks)
    // Reset the clock and update the register
    if(gb->cpu.div_clock >= 256) {
        const uint8_t curr_div = read_byte(gb, DIV, false);
        write_byte(gb, DIV, curr_div + 1, false);

        gb->cpu.div_clock = 0;
    }

    // The timer counter updates at the rate given in the control register
    // Although unlike the divider, it must be enabled in the control register
    // (0 == Stopped) (1 == Running)
    if(read_register(gb, TAC, TAC_STOP) == 1) {

        gb->cpu.cnt_clock += gb->cpu.ticks;
        const uint8_t speed_index = read_byte(gb, TAC, false) & 0x3;

        static const uint16_t timer_thresholds[4] = {
            CLOCK_SPEED / 4096,
            CLOCK_SPEED / 262144,
            CLOCK_SPEED / 65536,
            CLOCK_SPEED / 16384
        };

        const uint16_t threshold = timer_thresholds[speed_index];

        while(gb->cpu.cnt_clock >= threshold) {

            const uint8_t curr_cnt = read_byte(gb, TIMA, false);
            uint8_t new_cnt;

            if(curr_cnt + 1 == 256) {
                new_cnt = read_byte(gb, TMA, false);
                write_register(gb, IF, IEF_TIMER, 1);
            }
            else {
                new_cnt = curr_cnt + 1;
            }

            write_byte(gb, TIMA, new_cnt, false);
            gb->cpu.cnt_clock -= threshold;
        }
    }
}
