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

    // Enable interrupts
    cpu->registers->IME = false;
    cpu->is_halted = false;

    // Reset the timers
    cpu->div_clock = 0;
    cpu->cnt_clock = 0;
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
void execute_instr(gbc_system *gbc) {

    if(gbc->cpu->is_halted) {
        gbc->clocks += 4;
        return;
    }

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

    gbc->clocks += instruction.clocks;
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
void update_timer(gbc_system *gbc) {

    gbc->cpu->div_clock += gbc->clocks;

    // The divider register updates at one 256th of the clock speed (aka 256 clocks)
    // Reset the clock and update the register
    while(gbc->cpu->div_clock >= 256) {

        // Increment the register (don't care if it overflows)
        const uint8_t curr_div = read_byte(gbc, DIV, false);
        write_byte(gbc, DIV, curr_div + 1, false);

        gbc->cpu->div_clock -= 256;
    }

    // The timer counter updates at the rate given in the control register
    // Although unlike the divider, it must be enabled in the control register
    // (0 == Stopped) (1 == Running)
    if(read_register(gbc, TAC, TAC_STOP) == 1) {

        gbc->cpu->cnt_clock += gbc->clocks;

        // Get the speed to update the timer at
        const uint8_t speed_index = read_byte(gbc, TAC, false) & 0x3;

        // Get the array of possible tick to clock ratios
        static const uint16_t timer_thresholds[4] = {
            CLOCK_SPEED / 4096,
            CLOCK_SPEED / 262144,
            CLOCK_SPEED / 65536,
            CLOCK_SPEED / 16384
        };

        const uint16_t threshold = timer_thresholds[speed_index];

        // If the clock is at the current tick rate
        if(gbc->cpu->cnt_clock >= threshold) {

            const uint8_t curr_cnt = read_byte(gbc, TIMA, false);
            uint8_t new_cnt;

            // If the counter is about to overflow, reset it to the modulo
            if(curr_cnt + 1 == 256) {
                new_cnt = read_byte(gbc, TMA, false);
                write_register(gbc, IF, IEF_TIMER, 1);
            } else {
                new_cnt = curr_cnt + 1;
            }

            // Write the new value and reset the clock
            write_byte(gbc, TIMA, new_cnt, false);
            gbc->cpu->cnt_clock -= threshold;
        }
    }
}