#pragma once

uint8_t and(GameBoy *gb, const uint8_t a, const uint8_t b);
uint8_t or(GameBoy *gb, const uint8_t a, const uint8_t b);
uint8_t xor(GameBoy *gb, const uint8_t a, const uint8_t b);

uint8_t inc(GameBoy *gb, const uint8_t operand);
uint8_t dec(GameBoy *gb, const uint8_t operand);

uint8_t add_byte(GameBoy *gb, const uint8_t a, const uint8_t b);
uint8_t add_byte_carry(GameBoy *gb, const uint8_t a, const uint8_t b);
uint16_t add_short(GameBoy *gb, const uint16_t a, const uint16_t b);
uint16_t add_sp_signed_byte(GameBoy *gb, const uint16_t sp, const int8_t operand);

uint8_t sub_byte(GameBoy *gb, const uint8_t a, const uint8_t b);
uint8_t sub_byte_carry(GameBoy *gb, const uint8_t a, const uint8_t b);

uint8_t rotate_left(GameBoy *gb, const uint8_t operand, const bool affect_zero);
uint8_t rotate_right(GameBoy *gb, const uint8_t operand);
uint8_t rotate_left_carry(GameBoy *gb, const uint8_t operand, const bool affect_zero);
uint8_t rotate_right_carry(GameBoy *gb, const uint8_t operand, const bool affect_zero);
uint8_t shift_left_arith(GameBoy *gb, const uint8_t operand);
uint8_t shift_right_arith(GameBoy *gb, const uint8_t operand);
uint8_t shift_right_logic(GameBoy *gb, const uint8_t operand);

uint8_t swap(GameBoy *gb, const uint8_t operand);
void test_bit(GameBoy *gb, const uint8_t operand, const uint8_t bit);
uint8_t reset_bit(const uint8_t operand, const uint8_t bit);
uint8_t set_bit(const uint8_t operand, const uint8_t bit);

uint8_t daa(GameBoy *gb, const uint8_t operand); 