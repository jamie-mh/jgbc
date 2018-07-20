#include "jgbc.h"
#include "ram.h"
#include "cpu.h"
#include "alu.h"
#include "instr.h"

static void service_interrupt(gbc_system *, const uint8_t);
static gbc_instruction get_curr_instr(gbc_system *);


// Initialises the CPU registers
void init_cpu(gbc_cpu *cpu) {

    cpu->registers = malloc(sizeof(gbc_registers));

    cpu->registers->AF = DEFAULT_AF;
    cpu->registers->BC = DEFAULT_BC;
    cpu->registers->DE = DEFAULT_DE;
    cpu->registers->HL = DEFAULT_HL;

    cpu->registers->PC = DEFAULT_PC;
    cpu->registers->SP = DEFAULT_SP;

    cpu->registers->IME = false;
    cpu->is_halted = false;
	cpu->timer = 0;
	cpu->timer_overflow_time = 0;
}

// Returns an instruction struct of the provided opcode
gbc_instruction find_instr(const uint8_t opcode, const uint16_t address, gbc_system *gbc) {

    // If the opcode has the CB prefix
    if(opcode == 0xCB) {
        uint8_t next_opcode = read_byte(gbc, address + 1, false);
        return cb_instructions[next_opcode];
    } 
    // Standard instruction
    else {
        return instructions[opcode];
    }
}

// Executes an instruction by calling the function associated with it and returns the cycles it took
uint8_t execute_instr(gbc_system *gbc) {

    gbc_instruction instruction = get_curr_instr(gbc);

    void (*opcode_function)();
    opcode_function = instruction.execute;

    char operand_len = instruction.length - 1;
    uint16_t operand;

    // If it is a CB instruction, it's shorter because we don't count the prefix
    if(instruction.execute == &op_prefix_cb) {
        operand_len--; 
    }

    uint16_t instr_start = gbc->cpu->registers->PC;
    gbc->cpu->registers->PC += instruction.length;

    switch(operand_len) {

        case 0:
            opcode_function(gbc);
            break;

        case 1:
            operand = read_byte(gbc, instr_start + 1, false);
            opcode_function(gbc, (uint8_t) operand);
            break;

        case 2:
            operand = read_short(gbc, instr_start + 1, false);
            opcode_function(gbc, operand);
            break;
    }

    return instruction.clocks;
}

// Gets the current instruction at the program counter
static gbc_instruction get_curr_instr(gbc_system *gbc) {
    
    uint8_t opcode = read_byte(gbc, gbc->cpu->registers->PC, false);
    return find_instr(opcode, gbc->cpu->registers->PC, gbc);
}

// Returns the bit no of the specified flag in the F register
char get_flag_offset(const char flag) {

    switch(flag) {
        case FLAG_ZERO: return 7;
        case FLAG_SUBTRACT: return 6;
        case FLAG_HALFCARRY: return 5;
        case FLAG_CARRY: return 4;
        default: return -1;
    }
}

// Sets a flag in the F register
void set_flag(const char flag, const uint8_t value, uint8_t *regis) {

    char offset = get_flag_offset(flag);

    // Set or clear the bit
    if(value == 0) {
        *regis &= ~(1 << offset);
    } else if(value == 1) {
        *regis |= 1 << offset;          
    }
}

// Returns the value of a flag in the F register
char get_flag(const char flag, const uint8_t regis) {

    char offset = get_flag_offset(flag);

    // Read the nth bit of the register
    return (regis >> offset) & 1;
}

// Pushes a byte to the stack after decrementing the stack pointer
void stack_push_byte(gbc_system *gbc, uint16_t *sp, const uint8_t value) {

    *sp -= sizeof(uint8_t);
    write_byte(gbc, *sp, value, 1);
}

// Pushes a short to the stack after decrementing the stack pointer
void stack_push_short(gbc_system *gbc, uint16_t *sp, const uint16_t value) {
    
    // Push the short to the stack as two bytes
    stack_push_byte(gbc, sp, (value & 0xFF00) >> 8);
    stack_push_byte(gbc, sp, value & 0x00FF);
}

// Pops a byte from the stack and increments the stack pointer
uint8_t stack_pop_byte(gbc_system *gbc, uint16_t *sp) {
    
    uint8_t value = read_byte(gbc, *sp, false);
    *sp += sizeof(uint8_t);

    return value;
}

// Pops a short from the stack and increments the stack pointer
uint16_t stack_pop_short(gbc_system *gbc, uint16_t *sp) {
    
    const uint8_t byte_a = stack_pop_byte(gbc, sp); 
    const uint8_t byte_b = stack_pop_byte(gbc, sp); 

    return byte_a | (byte_b << 8);
}

// Checks if the cpu has pending interrupts and services them 
void check_interrupts(gbc_system *gbc) {

    const uint8_t enabled = read_byte(gbc, IE, false);
    const uint8_t flag = read_byte(gbc, IF, false);

    for(int i = 0; i < 5; i++) {
        
        // If this interrupt is enabled and requested
        // This just checks that the i'th bit is set in both bytes
        if(GET_BIT(enabled, i) && GET_BIT(flag, i)) {

            if(gbc->cpu->registers->IME) {
                service_interrupt(gbc, i);
            }
    
            gbc->cpu->is_halted = false;
        }
    }
}

// Calls the interrupt subroutine after storing the program counter on the stack
static void service_interrupt(gbc_system *gbc, const uint8_t number) {
    
    static const uint16_t interrupt[5] = {
        INT_VBLANK,
        INT_LCD_STAT,
        INT_TIMER,
        INT_SERIAL,
        INT_JOYPAD
    };

    assert(number < 5);

    // Push the current program counter to the stack
    stack_push_short(gbc, &gbc->cpu->registers->SP, gbc->cpu->registers->PC);
    
    // Disable the interrupt request
    write_register(gbc, IF, number, 0);
    gbc->cpu->registers->IME = false;
    gbc->cpu->registers->PC = interrupt[number];
}

// Updates the timer if they are enabled and requests interrupts
void update_timer(gbc_system *gbc, uint8_t clocks) {

	static uint8_t old_result = 0;

	// See: http://gbdev.gg8.se/wiki/articles/Timer_Obscure_Behaviour

	// When the timer overflows, TIMA is set to 0 for 4 clocks.
	// Then an interrupt is called
	if(gbc->cpu->timer_overflow_time > 0) {
		gbc->cpu->timer_overflow_time = 0;

		if(read_byte(gbc, TIMA, false) == 0x00) {
			write_byte(gbc, TIMA, read_byte(gbc, TMA, false), false);
			write_register(gbc, IF, IEF_TIMER, 1);
		}
	}

	// Emulate the timer multiplexer
	const uint8_t frequency_id = read_byte(gbc, TAC, false) & 0x3;
	uint8_t timer_bit;

	switch(frequency_id) {
		case 0: timer_bit = 9; break;
		case 1: timer_bit = 3; break;
		case 2: timer_bit = 5; break;
		case 3: timer_bit = 7; break;
	}

	for(uint8_t i = 0; i <= clocks; i++) {

		const uint16_t timer = gbc->cpu->timer + i;
		const uint8_t mu_output = GET_BIT(timer, timer_bit);
		const uint8_t result = mu_output & read_register(gbc, TAC, TAC_STOP);

		// Emulate the falling edge detector
		if(old_result == 1 && result == 0) {
			uint8_t tima = read_byte(gbc, TIMA, false);

			// If the counter is about to overflow, reset it to the modulo
			if(tima + 1 == 0x100) {
				tima = 0;
				gbc->cpu->timer_overflow_time = gbc->clocks;
			} else {
				tima++;
			}

			write_byte(gbc, TIMA, tima, false);	
		}

		old_result = result;
	}

	gbc->cpu->timer += clocks;
}