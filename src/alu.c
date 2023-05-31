#include "alu.h"
#include "cpu.h"
#include "macro.h"

static bool did_byte_half_carry(uint8_t, uint8_t);
static bool did_byte_full_carry(uint8_t, uint8_t);
static bool did_byte_half_borrow(uint8_t, uint8_t);
static bool did_byte_full_borrow(uint8_t, uint8_t);
static bool did_short_half_carry(uint16_t, uint16_t);
static bool did_short_full_carry(uint16_t, uint16_t);

/*
 *   Helper Functions
 */

static bool did_byte_half_carry(const uint8_t a, const uint8_t b) { return ((a & 0xF) + (b & 0xF)) > 0xF; }

static bool did_byte_half_borrow(const uint8_t a, const uint8_t b) { return ((a & 0xF) < (b & 0xF)); }

static bool did_byte_full_carry(const uint8_t a, const uint8_t b) { return (a + b) > 0xFF; }

static bool did_byte_full_borrow(const uint8_t a, const uint8_t b) { return (a < b); }

static bool did_short_half_carry(const uint16_t a, const uint16_t b) { return (a & 0xFFF) + (b & 0xFFF) > 0xFFF; }

static bool did_short_full_carry(const uint16_t a, const uint16_t b) { return ((a + b) & 0xF0000) > 0; }

/*
 *   Instruction ALU
 */

uint8_t and(GameBoy * gb, const uint8_t a, const uint8_t b) {
    const uint8_t result = a & b;

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 1);
    FSET(FLAG_CARRY, 0);

    return result;
}

uint8_t or(GameBoy * gb, const uint8_t a, const uint8_t b) {
    const uint8_t result = a | b;

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);
    FSET(FLAG_CARRY, 0);

    return result;
}

uint8_t xor(GameBoy * gb, const uint8_t a, const uint8_t b) {
    const uint8_t result = a ^ b;

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);
    FSET(FLAG_CARRY, 0);

    return result;
}

uint8_t inc(GameBoy *gb, const uint8_t operand) {
    const uint8_t result = operand + 1;

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, did_byte_half_carry(operand, 1));

    return result;
}

uint8_t dec(GameBoy *gb, const uint8_t operand) {
    const uint8_t result = operand - 1;

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 1);
    FSET(FLAG_HALFCARRY, did_byte_half_borrow(operand, 1));

    return result;
}

uint8_t add_byte(GameBoy *gb, const uint8_t a, const uint8_t b) {
    const uint8_t result = a + b;

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, did_byte_half_carry(a, b));
    FSET(FLAG_CARRY, did_byte_full_carry(a, b));

    return result;
}

uint8_t add_byte_carry(GameBoy *gb, const uint8_t a, const uint8_t b) {
    const uint8_t carry = FGET(FLAG_CARRY);
    const uint8_t result = a + b + carry;

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, did_byte_half_carry(a, carry) || did_byte_half_carry(a + carry, b));
    FSET(FLAG_CARRY, did_byte_full_carry(a, carry) || did_byte_full_carry(a + carry, b));

    return result;
}

uint8_t sub_byte(GameBoy *gb, const uint8_t a, const uint8_t b) {
    const uint8_t result = a - b;

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 1);
    FSET(FLAG_HALFCARRY, did_byte_half_borrow(a, b));
    FSET(FLAG_CARRY, did_byte_full_borrow(a, b));

    return result;
}

uint8_t sub_byte_carry(GameBoy *gb, const uint8_t a, const uint8_t b) {
    const uint8_t carry = FGET(FLAG_CARRY);
    const uint8_t result = a - b - carry;

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 1);
    FSET(FLAG_HALFCARRY, did_byte_half_borrow(a, carry) || did_byte_half_borrow(a - carry, b));
    FSET(FLAG_CARRY, did_byte_full_borrow(a, carry) || did_byte_full_borrow(a - carry, b));

    return result;
}

uint16_t add_short(GameBoy *gb, const uint16_t a, const uint16_t b) {
    const uint16_t result = a + b;

    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, did_short_half_carry(a, b));
    FSET(FLAG_CARRY, did_short_full_carry(a, b));

    return result;
}

// Designed to add a signed value to the SP register (only used in 2 instructions)
uint16_t add_sp_signed_byte(GameBoy *gb, const uint16_t sp, const int8_t operand) {
    FSET(FLAG_ZERO, 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, did_byte_half_carry(sp & 0xFF, (uint8_t) operand));
    FSET(FLAG_CARRY, did_byte_full_carry(sp & 0xFF, (uint8_t) operand));

    return sp + operand;
}

// Shift the carry flag onto the bottom and pop the top into the carry flag
uint8_t rotate_left(GameBoy *gb, const uint8_t operand, const bool affect_zero) {

    // Shift the carry flag onto the result
    const uint8_t result = (operand << 1) | FGET(FLAG_CARRY);
    FSET(FLAG_ZERO, (result == 0) && affect_zero);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);

    // Set the carry flag to the old bit 7
    FSET(FLAG_CARRY, (operand & 0x80) >> 7);

    return result;
}

// Shift the carry flag onto the top and pop the bottom into the carry flag
uint8_t rotate_right(GameBoy *gb, const uint8_t operand) {

    // Shift the carry flag onto the result
    const uint8_t result = (operand >> 1) | (FGET(FLAG_CARRY) << 7);

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);

    // Set the carry flag to the old bit 0
    FSET(FLAG_CARRY, operand & 0x01);

    return result;
}

// Pop the top into the carry flag and shift it onto the bottom
uint8_t rotate_left_carry(GameBoy *gb, const uint8_t operand, const bool affect_zero) {

    // Shift bit 0 onto the top
    const uint8_t result = ((operand << 1) | (operand >> 7));
    FSET(FLAG_ZERO, (result == 0) && affect_zero);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);

    // Set the carry flag to the old bit 7
    FSET(FLAG_CARRY, (operand & 0x80) >> 7);

    return result;
}

// Pop the bottom into the carry and shift it on the top
uint8_t rotate_right_carry(GameBoy *gb, const uint8_t operand, const bool affect_zero) {

    // Shift bit 0 onto the top
    const uint8_t result = ((operand >> 1) | (operand << 7));
    FSET(FLAG_ZERO, (result == 0) && affect_zero);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);

    // Set the carry flag to the old bit 0
    FSET(FLAG_CARRY, operand & 0x01);

    return result;
}

// Shift 0 on the bottom and pop the top into the carry flag
uint8_t shift_left_arith(GameBoy *gb, const uint8_t operand) {

    // Set the carry flag to the old bit 7
    FSET(FLAG_CARRY, (operand & 0x80) >> 7);

    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);

    // Shift left (set bit 0 to 0)
    const uint8_t result = operand << 1;
    FSET(FLAG_ZERO, result == 0);

    return result;
}

// Shift the top bit onto the top and pop the bottom into the carry flag
uint8_t shift_right_arith(GameBoy *gb, const uint8_t operand) {

    // Set the carry flag to the old bit 0
    FSET(FLAG_CARRY, operand & 1);

    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);

    // Shift right
    const uint8_t result = (operand & 0x80) | (operand >> 1);
    FSET(FLAG_ZERO, result == 0);

    return result;
}

// Shift 0 onto the top and pop the bottom into the carry flag
uint8_t shift_right_logic(GameBoy *gb, const uint8_t operand) {

    // Set the carry flag to the old bit 0
    FSET(FLAG_CARRY, operand & 0x01);

    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);

    // Shift right
    const uint8_t result = operand >> 1;
    FSET(FLAG_ZERO, result == 0);

    return result;
}

// Swap the top and bottom nibbles of the operand
uint8_t swap(GameBoy *gb, const uint8_t operand) {
    const uint8_t result = ((operand & 0xF0) >> 4) | ((operand & 0x0F) << 4);

    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);
    FSET(FLAG_CARRY, 0);

    return result;
}

void test_bit(GameBoy *gb, const uint8_t regis, const uint8_t bit) {
    const uint8_t bit_val = GET_BIT(regis, bit);

    FSET(FLAG_ZERO, bit_val == 0);
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 1);
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
uint8_t daa(GameBoy *gb, const uint8_t regis) {
    uint16_t result = regis;

    if (FGET(FLAG_SUBTRACT) == 0) {
        if ((FGET(FLAG_HALFCARRY) == 1) || ((result & 0xF) > 0x09)) {
            result += 0x06;
        }

        if ((FGET(FLAG_CARRY) == 1) || (result > 0x9F)) {
            result += 0x60;
        }
    } else {
        if (FGET(FLAG_HALFCARRY) == 1) {
            result = ((result - 0x06) & 0xFF);
        }

        if (FGET(FLAG_CARRY) == 1) {
            result -= 0x60;
        }
    }

    if (result & 0x100) {
        FSET(FLAG_CARRY, 1);
    }

    result &= 0xFF;
    FSET(FLAG_ZERO, result == 0);
    FSET(FLAG_HALFCARRY, 0);

    return (uint8_t) result;
}
