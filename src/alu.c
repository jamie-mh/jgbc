#include "jgbc.h"
#include "cpu.h"
#include "alu.h"
#include "instr.h"

static bool did_byte_half_carry(uint8_t a, uint8_t b);
static bool did_byte_full_carry(uint8_t a, uint8_t b);
static bool did_byte_half_borrow(uint8_t a, uint8_t b);
static bool did_byte_full_borrow(uint8_t a, uint8_t b);
static bool did_short_half_carry(uint16_t a, uint16_t b);
static bool did_short_full_carry(uint16_t a, uint16_t b);

/*
*   Helper Functions
*/

static bool did_byte_half_carry(uint8_t a, uint8_t b) {
    return ((a & 0xF) + (b & 0xF)) > 0xF;
}

static bool did_byte_half_borrow(uint8_t a, uint8_t b) {
    return ((a & 0xF) < (b & 0xF));
}

static bool did_byte_full_carry(uint8_t a, uint8_t b) {
    return (a + b) > 0xFF;
}

static bool did_byte_full_borrow(uint8_t a, uint8_t b) {
    return (a < b);
}

static bool did_short_half_carry(uint16_t a, uint16_t b) {
    return (a & 0xFFF) + (b & 0xFFF) > 0xFFF;
}

static bool did_short_full_carry(uint16_t a, uint16_t b) {
    return ((a + b) & 0xF0000) > 0;
}

/*
*   Instruction ALU
*/

uint8_t and(const uint8_t a, const uint8_t b, uint8_t *flag) {
    
    const uint8_t result = a & b;

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

uint8_t or(const uint8_t a, const uint8_t b, uint8_t *flag) {

    const uint8_t result = a | b;

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

uint8_t xor(const uint8_t a, const uint8_t b, uint8_t *flag) {
    
    const uint8_t result = a ^ b;

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

uint8_t inc(const uint8_t operand, uint8_t *flag) {

    const uint8_t result = operand + 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);

    if(did_byte_half_carry(operand, 1)) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    } 

    return result;
}

uint8_t dec(const uint8_t operand, uint8_t *flag) {

    const uint8_t result = operand - 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    if(did_byte_half_borrow(operand, 1)) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    }

    return result;
}

uint8_t add_byte(const uint8_t a, const uint8_t b, uint8_t *flag) {
    
    const uint8_t result = a + b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);

    if(did_byte_half_carry(a, b)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    if(did_byte_full_carry(a, b)) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }
    
    return result;
}

uint8_t add_byte_carry(const uint8_t a, const uint8_t b, uint8_t *flag) {

    const uint8_t carry = get_flag(FLAG_CARRY, *flag);
    const uint8_t result = a + b + carry;

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

uint8_t sub_byte(const uint8_t a, const uint8_t b, uint8_t *flag) {

    const uint8_t result = a - b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    if(did_byte_half_borrow(a, b)) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    }

    if(did_byte_full_borrow(a, b)) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result;
}

uint8_t sub_byte_carry(const uint8_t a, const uint8_t b, uint8_t *flag) {

    const uint8_t carry = get_flag(FLAG_CARRY, *flag);
    const uint8_t result = a - b - carry;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    if(did_byte_half_borrow(a, carry) || did_byte_half_borrow(a - carry, b)) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    }

    if(did_byte_full_borrow(a, carry) || did_byte_full_borrow(a - carry, b)) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result;
}

uint16_t add_short(const uint16_t a, const uint16_t b, uint8_t *flag) {
    
    const uint16_t result = a + b;

    set_flag(FLAG_SUBTRACT, 0, flag);

    if(did_short_half_carry(a, b)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    if(did_short_full_carry(a, b)) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result;
}

// Designed to add a signed value to the SP register (only used in 2 instructions)
uint16_t add_sp_signed_byte(const uint16_t sp, const int8_t operand, uint8_t *flag) {

    set_flag(FLAG_ZERO, 0, flag);
    set_flag(FLAG_SUBTRACT, 0, flag);

    if(did_byte_half_carry(sp & 0xFF, (uint8_t) operand)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    if(did_byte_full_carry(sp & 0xFF, (uint8_t) operand)) {
        set_flag(FLAG_CARRY, 1, flag); 
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return sp + operand;
}

// Shift the carry flag onto the bottom and pop the top into the carry flag
uint8_t rotate_left(const uint8_t operand, const bool affect_zero, uint8_t *flag) {
    
    // Shift the carry flag onto the result
    const uint8_t result = (operand << 1) | get_flag(FLAG_CARRY, *flag);

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
uint8_t rotate_right(const uint8_t operand, uint8_t *flag) {

    // Shift the carry flag onto the result
    const uint8_t result = (operand >> 1) | (get_flag(FLAG_CARRY, *flag) << 7);

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
uint8_t rotate_left_carry(const uint8_t operand, const bool affect_zero, uint8_t *flag) {

    // Shift bit 0 onto the top 
    const uint8_t result = ((operand << 1) | (operand >> 7));

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
uint8_t rotate_right_carry(const uint8_t operand, const bool affect_zero, uint8_t *flag) {

    // Shift bit 0 onto the top 
    const uint8_t result = ((operand >> 1) | (operand << 7));

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
uint8_t shift_left_arith(const uint8_t operand, uint8_t *flag) {

    // Set the carry flag to the old bit 7
    set_flag(FLAG_CARRY, (operand & 0x80) >> 7, flag);

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);

    // Shift left (set bit 0 to 0)
    const uint8_t result = operand << 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    return result;
}

// Shift the top bit onto the top and pop the bottom into the carry flag
uint8_t shift_right_arith(const uint8_t operand, uint8_t *flag) {

    // Set the carry flag to the old bit 0
    set_flag(FLAG_CARRY, operand & 1, flag);

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);

    // Shift right
    const uint8_t result = (operand & 0x80) | (operand >> 1);

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    return result;
}

// Shift 0 onto the top and pop the bottom into the carry flag
uint8_t shift_right_logic(const uint8_t operand, uint8_t *flag) {
    
    // Set the carry flag to the old bit 0
    set_flag(FLAG_CARRY, operand & 0x01, flag);

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 0, flag);

    // Shift right
    const uint8_t result = operand >> 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    return result;
}

// Swap the top and bottom nibbles of the operand
uint8_t swap(const uint8_t operand, uint8_t *flag) {

    const uint8_t result = ((operand & 0xF0) >> 4) | ((operand & 0x0F) << 4);
    
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

void test_bit(const uint8_t regis, const uint8_t bit, uint8_t *flag) {

    const uint8_t bit_val = GET_BIT(regis, bit);

    if(bit_val == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);
    set_flag(FLAG_HALFCARRY, 1, flag);
}

uint8_t reset_bit(const uint8_t regis, const uint8_t bit) {
    uint8_t result = regis;
    result &= ~(1 << bit);

    return result;
}

uint8_t set_bit(const uint8_t regis, const uint8_t bit) {
    uint8_t result = regis;
    result |= (1 << bit);

    return result;
}

// Returns the BCD (Binary Coded Decimal) of the register value
uint8_t daa(const uint8_t regis, uint8_t *flag) {

    uint16_t result = regis;

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

    return (uint8_t) result;
}