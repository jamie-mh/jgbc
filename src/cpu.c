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
	gb->cpu.timer = 0;
}

/*
    Instructions
*/

Instruction find_instr(GameBoy *gb, const uint16_t address) {
    const uint8_t opcode = read_byte(gb, address, false);

    if(opcode == 0xCB) {
        const uint8_t next_opcode = read_byte(gb, address + 1, false);
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

void stack_push_byte(GameBoy *gb, uint16_t *sp, const uint8_t value) {
    *sp -= sizeof(uint8_t);
    write_byte(gb, *sp, value, true);
}

void stack_push_short(GameBoy *gb, uint16_t *sp, const uint16_t value) {
    stack_push_byte(gb, sp, (value & 0xFF00) >> 8);
    stack_push_byte(gb, sp, value & 0x00FF);
}

uint8_t stack_pop_byte(GameBoy *gb, uint16_t *sp) {
    const uint8_t value = read_byte(gb, *sp, false);
    *sp += sizeof(uint8_t);

    return value;
}

uint16_t stack_pop_short(GameBoy *gb, uint16_t *sp) {
    const uint8_t byte_a = stack_pop_byte(gb, sp); 
    const uint8_t byte_b = stack_pop_byte(gb, sp); 

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
    stack_push_short(gb, &REG(SP), REG(PC));
    
    write_register(gb, IF, number, 0);
    REG(IME) = false;
    REG(PC) = interrupt[number];
}

/*
    Timer
*/

void update_timer(GameBoy *gb) {

	static uint8_t old_result = 0;

	// See: http://gbdev.gg8.se/wiki/articles/Timer_Obscure_Behaviour

	// When the timer overflows, TIMA is set to 0 for 4 clocks.
	// Then an interrupt is called
	if(gb->cpu.timer_overflow) {
        gb->cpu.timer_overflow = false;

		if(read_byte(gb, TIMA, false) == 0x00) {
			write_byte(gb, TIMA, read_byte(gb, TMA, false), false);
			write_register(gb, IF, IEF_TIMER, 1);
		}
	}

	// Emulate the timer multiplexer
	const uint8_t frequency_id = read_byte(gb, TAC, false) & 0x3;
	uint8_t timer_bit;

	switch(frequency_id) {
		case 0: timer_bit = 9; break;
		case 1: timer_bit = 3; break;
		case 2: timer_bit = 5; break;
		case 3: timer_bit = 7; break;
	}

	for(uint8_t i = 0; i <= gb->cpu.ticks; i++) {

		const uint16_t timer = gb->cpu.timer + i;
		const uint8_t mu_output = GET_BIT(timer, timer_bit);
		const uint8_t result = mu_output & read_register(gb, TAC, TAC_STOP);

		// Emulate the falling edge detector
		if(old_result == 1 && result == 0) {
			uint8_t tima = read_byte(gb, TIMA, false);

			if(tima + 1 == 0x100) {
				tima = 0;
                gb->cpu.timer_overflow = true;
			} else {
				tima++;
			}

			write_byte(gb, TIMA, tima, false);	
		}

		old_result = result;
	}

    gb->cpu.timer += gb->cpu.ticks;
}