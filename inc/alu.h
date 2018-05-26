#pragma once

unsigned char and(const unsigned char, const unsigned char, unsigned char *);
unsigned char or(const unsigned char, const unsigned char, unsigned char *);
unsigned char xor(const unsigned char, const unsigned char, unsigned char *);

unsigned char inc(const unsigned char, unsigned char *);
unsigned char dec(const unsigned char, unsigned char *);

unsigned char add_byte(const unsigned char, const unsigned char, unsigned char *);
unsigned char add_byte_carry(const unsigned char, const unsigned char, unsigned char *);
unsigned short add_short(const unsigned short, const unsigned short, unsigned char *);
unsigned short add_sp_signed_byte(const unsigned short, const signed char, unsigned char *);

unsigned char sub_byte(const unsigned char, const unsigned char, unsigned char *);
unsigned char sub_byte_carry(const unsigned char, const unsigned char, unsigned char *);

unsigned char rotate_left(const unsigned char, const bool, unsigned char *);
unsigned char rotate_right(const unsigned char, unsigned char *);
unsigned char rotate_left_carry(const unsigned char, const bool, unsigned char *);
unsigned char rotate_right_carry(const unsigned char, const bool, unsigned char *);
unsigned char shift_left_arith(const unsigned char, unsigned char *);
unsigned char shift_right_arith(const unsigned char, unsigned char *);
unsigned char shift_right_logic(const unsigned char, unsigned char *);

unsigned char swap(const unsigned char, unsigned char *);
void test_bit(const unsigned char, const unsigned char, unsigned char *);
unsigned char reset_bit(const unsigned char, const unsigned char);
unsigned char set_bit(const unsigned char, const unsigned char);

unsigned char daa(const unsigned char, unsigned char *); 
