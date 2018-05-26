#include "lxgbc.h"
#include "cpu.h"
#include "alu.h"
#include "instr.h"

static bool did_byte_half_carry(unsigned char, unsigned char);
static bool did_byte_full_carry(unsigned char, unsigned char);
static bool did_byte_half_borrow(unsigned char, unsigned char);
static bool did_byte_full_borrow(unsigned char, unsigned char);
static bool did_short_half_carry(unsigned short, unsigned short);
static bool did_short_full_carry(unsigned short a, unsigned short b);

/*
*   Helper Functions
*/

// Checks if a byte operation resulted in a half-carry
static bool did_byte_half_carry(unsigned char a, unsigned char b) {
    return ((a & 0xF) + (b & 0xF)) > 0xF;
}

// Checks if a byte operation resulted in a half-borrow
static bool did_byte_half_borrow(unsigned char a, unsigned char b) {
    return ((a & 0xF) < (b & 0xF));
}

// Checks if a byte operation resulted in a full carry
static bool did_byte_full_carry(unsigned char a, unsigned char b) {
    return (a + b) > 0xFF;
}

// Checks if a byte operation resulted in a full borrow
static bool did_byte_full_borrow(unsigned char a, unsigned char b) {
    return (a < b);
}

// Checks if a short operation resulted in a half-carry
static bool did_short_half_carry(unsigned short a, unsigned short b) {
    return (a & 0xFFF) + (b & 0xFFF) > 0xFFF;
}

// Checks if a short operation resulted in a full carry
static bool did_short_full_carry(unsigned short a, unsigned short b) {
    return ((a + b) & 0xF0000) > 0;
}

/*
*   Instruction ALU
*/

// Bitwise AND of a and b
unsigned char and(const unsigned char a, const unsigned char b, unsigned char *flag) {
    
    const unsigned char result = a & b;

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

// Bitwise OR of a and b
unsigned char or(const unsigned char a, const unsigned char b, unsigned char *flag) {

    const unsigned char result = a | b;

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

// Bitwise XOR of a and b
unsigned char xor(const unsigned char a, const unsigned char b, unsigned char *flag) {
    
    const unsigned char result = a ^ b;

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

// Increment byte
unsigned char inc(const unsigned char operand, unsigned char *flag) {

    const unsigned char result = operand + 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);

    // Carry from bit 3 to 4
    if(did_byte_half_carry(operand, 1)) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    } 

    return result;
}

// Decrement byte
unsigned char dec(const unsigned char operand, unsigned char *flag) {

    const unsigned char result = operand - 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    // Borrow from bit 4 to 3
    if(did_byte_half_borrow(operand, 1)) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    }

    return result;
}

// Add two bytes and return a byte
unsigned char add_byte(const unsigned char a, const unsigned char b, unsigned char *flag) {
    
    const unsigned char result = a + b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);

    // Check for a carry from bit 3 to 4
    if(did_byte_half_carry(a, b)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    // Check for carry
    if(did_byte_full_carry(a, b)) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }
    
    return result;
}

// Add two bytes with the carry flag and return a byte
unsigned char add_byte_carry(const unsigned char a, const unsigned char b, unsigned char *flag) {

    const unsigned char carry = get_flag(FLAG_CARRY, *flag);
    const unsigned char result = a + b + carry;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);

    if(did_byte_half_carry(a, carry) || did_byte_half_carry(a + carry, b)) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    if(did_byte_full_carry(a, carry) || did_byte_full_carry(a + carry, b)) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result;
}

// Subtract two bytes and return a byte
unsigned char sub_byte(const unsigned char a, const unsigned char b, unsigned char *flag) {

    const unsigned char result = a - b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    // Borrow from bit 4 to 3
    if(did_byte_half_borrow(a, b)) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    }

    // If the result underflows a byte
    if(did_byte_full_borrow(a, b)) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result;
}

// Subtract two bytes, the carry flag and return a byte
unsigned char sub_byte_carry(const unsigned char a, const unsigned char b, unsigned char *flag) {

    const unsigned char carry = get_flag(FLAG_CARRY, *flag);
    const unsigned char result = a - b - carry;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    // Borrow from bit 4 to 3
    if(did_byte_half_borrow(a, carry) || did_byte_half_borrow(a - carry, b)) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    }

    // If the result underflows a byte
    if(did_byte_full_borrow(a, carry) || did_byte_full_borrow(a - carry, b)) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result;
}

// Add two 16 bit numbers
unsigned short add_short(const unsigned short a, const unsigned short b, unsigned char *flag) {
    
    const unsigned short result = a + b;

    set_flag(FLAG_SUBTRACT, 0, flag);

    // Carry from bit 11 to 12
    if(did_short_half_carry(a, b)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    // Result overflows a 16 bit integer
    if(did_short_full_carry(a, b)) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result;
}

// Designed to add a signed value to the SP register (only used in 2 instructions)
unsigned short add_sp_signed_byte(const unsigned short sp, const signed char operand, unsigned char *flag) {

    set_flag(FLAG_ZERO, 0, flag);
    set_flag(FLAG_SUBTRACT, 0, flag);

    if(did_byte_half_carry(sp & 0xFF, (unsigned char) operand)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    if(did_byte_full_carry(sp & 0xFF, (unsigned char) operand)) {
        set_flag(FLAG_CARRY, 1, flag); 
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return sp + operand;
}

// Shift the carry flag onto the bottom and pop the top into the carry flag
unsigned char rotate_left(const unsigned char operand, const bool affect_zero, unsigned char *flag) {
    
    // Shift the carry flag onto the result
    const unsigned char result = (operand << 1) | get_flag(FLAG_CARRY, *flag);

    if((result == 0) && affect_zero) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    
    // Set the carry flag to the old bit 7
    set_flag(FLAG_CARRY, (operand & 0x80) >> 7, flag);

    return result;
}

// Shift the carry flag onto the top and pop the bottom into the carry flag
unsigned char rotate_right(const unsigned char operand, unsigned char *flag) {

    // Shift the carry flag onto the result
    const unsigned char result = (operand >> 1) | (get_flag(FLAG_CARRY, *flag) << 7);

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    
    // Set the carry flag to the old bit 0
    set_flag(FLAG_CARRY, operand & 0x01, flag);

    return result;
}

// Pop the top into the carry flag and shift it onto the bottom
unsigned char rotate_left_carry(const unsigned char operand, const bool affect_zero, unsigned char *flag) {

    // Shift bit 0 onto the top 
    const unsigned char result = ((operand << 1) | (operand >> 7));

    if((result == 0) && affect_zero) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    
    // Set the carry flag to the old bit 7
    set_flag(FLAG_CARRY, (operand & 0x80) >> 7, flag);

    return result;
}

// Pop the bottom into the carry and shift it on the top
unsigned char rotate_right_carry(const unsigned char operand, const bool affect_zero, unsigned char *flag) {

    // Shift bit 0 onto the top 
    const unsigned char result = ((operand >> 1) | (operand << 7));

    if((result == 0) && affect_zero) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);
    
    // Set the carry flag to the old bit 0
    set_flag(FLAG_CARRY, operand & 0x01, flag);

    return result;
}

// Shift 0 on the bottom and pop the top into the carry flag
unsigned char shift_left_arith(const unsigned char operand, unsigned char *flag) {

    // Set the carry flag to the old bit 7
    set_flag(FLAG_CARRY, (operand & 0x80) >> 7, flag);

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);

    // Shift left (set bit 0 to 0)
    const unsigned char result = operand << 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    return result;
}

// Shift the top bit onto the top and pop the bottom into the carry flag
unsigned char shift_right_arith(const unsigned char operand, unsigned char *flag) {

    // Set the carry flag to the old bit 0
    set_flag(FLAG_CARRY, operand & 1, flag);

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);

    // Shift right
    const unsigned char result = (operand & 0x80) | (operand >> 1);

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    return result;
}

// Shift 0 onto the top and pop the bottom into the carry flag
unsigned char shift_right_logic(const unsigned char operand, unsigned char *flag) {
    
    // Set the carry flag to the old bit 0
    set_flag(FLAG_CARRY, operand & 0x01, flag);

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);

    // Shift right
    const unsigned char result = operand >> 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    return result;
}

// Swap the top and bottom nibbles of the operand
unsigned char swap(const unsigned char operand, unsigned char *flag) {

    // Swap the top and bottom halves
    const unsigned char result = ((operand & 0xF0) >> 4) | ((operand & 0x0F) << 4);
    
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

// Test if a bit is set (modifies zero flag)
void test_bit(const unsigned char regis, const unsigned char bit, unsigned char *flag) {

    // Read the nth bit
    const unsigned char bit_val = (regis >> bit) & 1;

    if(bit_val == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 1, flag);
}

// Sets a bit to 0
unsigned char reset_bit(const unsigned char regis, const unsigned char bit) {

    unsigned char result = regis;
    result &= ~(1 << bit);

    return result;
}

// Sets a bit to 1
unsigned char set_bit(const unsigned char regis, const unsigned char bit) {

    unsigned char result = regis;
    result |= (1 << bit);

    return result;
}

// Returns the BCD (Binary Coded Decimal) of the register value
unsigned char daa(const unsigned char regis, unsigned char *flag) {

    unsigned short result = regis;

    if(get_flag(FLAG_SUBTRACT, *flag) == 0)
    {
        if((get_flag(FLAG_HALFCARRY, *flag) == 1) || ((result & 0xF) > 0x09)) { 
            result += 0x06;
        }

        if((get_flag(FLAG_CARRY, *flag) == 1) || (result > 0x9F)) {
            result += 0x60;
        }
    }
    else
    {
        if(get_flag(FLAG_HALFCARRY, *flag) == 1)  {
            result = ((result - 0x06) & 0xFF);
        }

        if(get_flag(FLAG_CARRY, *flag) == 1) {
            result -= 0x60;
        }
    }

    if(result & 0x100) {
        set_flag(FLAG_CARRY, 1, flag);
    }

    result &= 0xFF;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag);
    }

    set_flag(FLAG_HALFCARRY, 0, flag);

    return (unsigned short) result;
}
