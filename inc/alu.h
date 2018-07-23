#pragma once

uint8_t and(const uint8_t a, const uint8_t b, uint8_t *flag);
uint8_t or(const uint8_t a, const uint8_t b, uint8_t *flag);
uint8_t xor(const uint8_t a, const uint8_t b, uint8_t *flag);

uint8_t inc(const uint8_t operand, uint8_t *flag);
uint8_t dec(const uint8_t operand, uint8_t *flag);

uint8_t add_byte(const uint8_t a, const uint8_t b, uint8_t *flag);
uint8_t add_byte_carry(const uint8_t a, const uint8_t b, uint8_t *flag);
uint16_t add_short(const uint16_t a, const uint16_t b, uint8_t *flag);
uint16_t add_sp_signed_byte(const uint16_t sp, const int8_t operand, uint8_t *flag);

uint8_t sub_byte(const uint8_t a, const uint8_t b, uint8_t *flag);
uint8_t sub_byte_carry(const uint8_t a, const uint8_t b, uint8_t *flag);

uint8_t rotate_left(const uint8_t operand, const bool affect_zero, uint8_t *flag);
uint8_t rotate_right(const uint8_t operand, uint8_t *flag);
uint8_t rotate_left_carry(const uint8_t operand, const bool affect_zero, uint8_t *flag);
uint8_t rotate_right_carry(const uint8_t operand, const bool affect_zero, uint8_t *flag);
uint8_t shift_left_arith(const uint8_t operand, uint8_t *flag);
uint8_t shift_right_arith(const uint8_t operand, uint8_t *flag);
uint8_t shift_right_logic(const uint8_t operand, uint8_t *flag);

uint8_t swap(const uint8_t operand, uint8_t *flag);
void test_bit(const uint8_t operand, const uint8_t bit, uint8_t *flag);
uint8_t reset_bit(const uint8_t operand, const uint8_t bit);
uint8_t set_bit(const uint8_t operand, const uint8_t bit);

uint8_t daa(const uint8_t operand, uint8_t *flag); 