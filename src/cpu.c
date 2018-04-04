#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "instr.h"

static void service_interrupt(gbc_system *, const unsigned char);
static void execute_instr(gbc_instruction, gbc_system *);
static gbc_instruction get_curr_instr(gbc_system *);


// Initialises the CPU registers
void init_cpu(gbc_cpu *cpu) {

    // Initialize the registers
    cpu->registers = malloc(sizeof(gbc_registers));

    // Default register values
    cpu->registers->AF = DEFAULT_AF;
    cpu->registers->BC = DEFAULT_BC;
    cpu->registers->DE = DEFAULT_DE;
    cpu->registers->HL = DEFAULT_HL;

    // Set the pointers to their default value
    cpu->registers->PC = DEFAULT_PC;
    cpu->registers->SP = DEFAULT_SP;

    // Enable interrupts
    cpu->registers->IME = 1;
    cpu->is_halted = 0;

    // Reset the clock
    cpu->clock = 0;
    cpu->run_for = 0;
}

// Returns an instruction struct of the provided opcode
gbc_instruction find_instr(const unsigned char opcode, gbc_system *gbc) {

    // If the opcode has the CB prefix
    if(opcode == 0xCB) {
        unsigned char next_opcode = read_byte(gbc->ram, gbc->cpu->registers->PC + 1);
        return cb_instructions[next_opcode];
    } 
    // Standard instruction
    else {
        return instructions[opcode];
    }
}

// Executes an instruction by calling the function associated with it 
static void execute_instr(gbc_instruction instruction, gbc_system *gbc) {

    // Create a pointer to the function to execute
    void (*opcode_function)();
    opcode_function = instruction.execute;

    // Execute the instruction based on the length of the operand
    char operand_len = instruction.length - 1;
    unsigned short operand;

    // If it is a CB instruction, it's shorter
    if(instruction.execute == &op_prefix_cb) {
        operand_len--; 
    }

    // Increment the program counter
    unsigned short instr_start = gbc->cpu->registers->PC;
    gbc->cpu->registers->PC += instruction.length;

    switch(operand_len) {

        case 0:
            opcode_function(gbc);
            break;

        case 1:
            operand = read_byte(gbc->ram, instr_start + 1);
            opcode_function(gbc, (unsigned char) operand);
            break;

        case 2:
            operand = read_short(gbc->ram, instr_start + 1);
            opcode_function(gbc, operand);
            break;
    }
}

// Execute a cpu instruction and respect its duration in clock cycles
void cpu_do_clock(gbc_system *gbc) {

    // Wait until the current instruction is finished
    if(gbc->cpu->clock < gbc->cpu->run_for) {
        gbc->cpu->clock++;
    }
    // We've moved to the next instruction
    else {

        // Execute the instruction
        gbc_instruction instruction = get_curr_instr(gbc);
        execute_instr(instruction, gbc);

        // Set the clock accordingly
        gbc->cpu->clock = 0;
        gbc->cpu->run_for = instruction.clocks;
    }
}

// Gets the current instruction at the program counter
static gbc_instruction get_curr_instr(gbc_system *gbc) {
    
    // Get the opcode at the program counter
    unsigned char opcode = read_byte(gbc->ram, gbc->cpu->registers->PC);

    // Get the instruction structure
    return find_instr(opcode, gbc);
}

// Returns the bit no of the specified flag in the F register
char get_flag_offset(const char flag) {
    
    // Find the offset of the bit
    switch(flag) {
        case FLAG_ZERO: return 7;
        case FLAG_SUBTRACT: return 6;
        case FLAG_HALFCARRY: return 5;
        case FLAG_CARRY: return 4;
        default: return -1;
    }
}

// Sets a flag in the F register
void set_flag(const char flag, const unsigned char value, unsigned char *regis) {

    char offset = get_flag_offset(flag);

    // Set or clear the bit
    if(value == 0) {
        *regis &= ~(1 << offset);
    } else if(value == 1) {
        *regis |= 1 << offset;          
    }
}

// Returns the value of a flag in the F register
char get_flag(const char flag, const unsigned char regis) {

    char offset = get_flag_offset(flag);

    // Read the nth bit of the register
    return (regis >> offset) & 1;
}

// Pushes a byte to the stack after decrementing the stack pointer
void stack_push_byte(gbc_ram *ram, unsigned short *sp, const unsigned char value) {
   
    // Decrement the stack pointer
    *sp -= sizeof(char);

    // Write the value
    write_byte(ram, *sp, value);
}

// Pushes a short to the stack after decrementing the stack pointer
void stack_push_short(gbc_ram *ram, unsigned short *sp, const unsigned short value) {
    
    // Push the short to the stack as two bytes
    stack_push_byte(ram, sp, (value & 0xFF00) >> 8);
    stack_push_byte(ram, sp, value & 0x00FF);
}

// Pops a byte from the stack and increments the stack pointer
unsigned char stack_pop_byte(gbc_ram *ram, unsigned short *sp) {
    
    // Read the value
    unsigned char value = read_byte(ram, *sp);
    
    // Increment the stack pointer
    *sp += sizeof(char);

    return value;
}

// Pops a short from the stack and increments the stack pointer
unsigned short stack_pop_short(gbc_ram *ram, unsigned short *sp) {
    
    // Get two bytes from the stack 
    unsigned char byte_a = stack_pop_byte(ram, sp); 
    unsigned char byte_b = stack_pop_byte(ram, sp); 

    // Combine them
    return byte_a | (byte_b << 8);
}

// Checks if the cpu has pending interrupts and services them 
void check_interrupt(gbc_system *gbc) {
    
    // Read the interrupt enable and request registers
    unsigned char enabled = read_byte(gbc->ram, IE);
    unsigned char flag = read_byte(gbc->ram, IF);

    // Find the enabled and requested interrupts
    for(int i = 0; i < 5; i++) {
        
        // If this interrupt is enabled and requested
        // This just checks that the i'th bit is set in both bytes
        if(((enabled >> i) & 1) && ((flag >> i) & 1)) {

            // If interrupts are globally enabled
            if(gbc->cpu->registers->IME) {
                service_interrupt(gbc, i);
            }

            gbc->cpu->is_halted = 0;
        }
    }
}

// Calls the interrupt subroutine after storing the program counter on the stack
static void service_interrupt(gbc_system *gbc, const unsigned char number) {
    
    static const unsigned short interrupt[5] = {
        INT_VBLANK,
        INT_LCD_STAT,
        INT_TIMER,
        INT_SERIAL,
        INT_JOYPAD
    };

    if(number <= 5) {

        // Push the current program counter to the stack
        stack_push_short(gbc->ram, &gbc->cpu->registers->SP, gbc->cpu->registers->PC);
        
        // Disable the interrupt request
        write_register(gbc->ram, IE, number, 0);
        write_register(gbc->ram, IF, number, 0);
        gbc->cpu->registers->IME = 0;

        // Jump to the interrupt subroutine
        gbc->cpu->registers->PC = interrupt[number];
    }  
}

/*
*   Common CPU operations
*/

unsigned char and(const unsigned char a, const unsigned char b, unsigned char *flag) {
    
    unsigned char result = a & b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
   
    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 1, flag);
    set_flag(FLAG_CARRY, 0, flag);

    return result;
}

unsigned char or(const unsigned char a, const unsigned char b, unsigned char *flag) {

    unsigned char result = a | b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    set_flag(FLAG_CARRY, 0, flag);

    return result;
}

unsigned char xor(const unsigned char a, const unsigned char b, unsigned char *flag) {
    
    unsigned char result = a ^ b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    set_flag(FLAG_CARRY, 0, flag);

    return result;
}

unsigned char inc(const unsigned char operand, unsigned char *flag) {

    unsigned char result = operand + 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);

    if((operand & 0xF) == 0xF) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    } 

    return result;
}

unsigned char dec(const unsigned char operand, unsigned char *flag) {

    unsigned char result = operand - 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    if(operand & 0x0f) {
        set_flag(FLAG_HALFCARRY, 0, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 1, flag);
    }

    return result;
}

unsigned char add_byte(const unsigned char a, const unsigned char b, unsigned char *flag) {
    
    unsigned short result = a + b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);

    // Check for a carry from bit 3
    if(((a & 0xF) + (b & 0xF)) > 0xF) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    // Check for carry
    if((result & 0xFF00) > 0) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }
    
    return result & 0xFF;
}

unsigned char sub_byte(const unsigned char a, const unsigned char b, unsigned char *flag) {

    unsigned short result = a - b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    if((a & 0xF) > (b & 0xF)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    if((result & 0xFF00) > 0) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result & 0xFF;
}

unsigned short add_short(const unsigned short a, const unsigned short b, unsigned char *flag) {
    
    unsigned int result = a + b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);

    if((a & 0xF) > (b & 0xF)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    if((result & 0xFFFF0000) > 0) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result & 0xFFFF;
}

unsigned char rotate_left(const unsigned char operand, unsigned char *flag) {
    
    unsigned char old = operand;
    unsigned char result = operand;
    
    // Shift the carry flag onto the result
    result = (result << 1) | get_flag(FLAG_CARRY, *flag);

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    
    // Set the carry flag to the old bit 7
    set_flag(FLAG_CARRY, (old & 0x80) >> 7, flag);

    return result;
}

unsigned char rotate_right(const unsigned char operand, unsigned char *flag) {

    unsigned char old = operand;
    unsigned char result = operand;
    
    // Shift the carry flag onto the result
    result = (result >> 1) | (get_flag(FLAG_CARRY, *flag) << 7);

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    
    // Set the carry flag to the old bit 0
    set_flag(FLAG_CARRY, old & 0x01, flag);

    return result;
}

unsigned char rotate_left_carry(const unsigned char operand, unsigned char *flag) {

    unsigned char old = operand;
    unsigned char result = operand;
    
    // Shift bit 0 onto the top 
    result = (result << 1) | (result & 0x01);

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    
    // Set the carry flag to the old bit 7
    set_flag(FLAG_CARRY, (old & 0x80) >> 7, flag);

    return result;
}

unsigned char rotate_right_carry(const unsigned char operand, unsigned char *flag) {

    unsigned char old = operand;
    unsigned char result = operand;
    
    // Shift bit 0 onto the top 
    result = (result >> 1) | ((result & 0x01) << 7);

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    
    // Set the carry flag to the old bit 0
    set_flag(FLAG_CARRY, old & 0x01, flag);

    return result;
}

unsigned char shift_left_arith(const unsigned char operand, unsigned char *flag) {

}

unsigned char shift_right_arith(const unsigned char operand, unsigned char *flag) {

}

unsigned char shift_right_logic(const unsigned char operand, unsigned char *flag) {

}

unsigned char swap(const unsigned char operand, unsigned char *flag) {

}

void test_bit(const unsigned char regis, const unsigned char bit, unsigned char *flag) {

}

unsigned char reset_bit(const unsigned char regis, const unsigned char bit, unsigned char *flag) {

}

unsigned char set_bit(const unsigned char regis, const unsigned char bit, unsigned char *flag) {

}