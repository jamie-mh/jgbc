#pragma once

#include "gameboy.h"

uint8_t and (GameBoy *, uint8_t, uint8_t);
uint8_t or (GameBoy *, uint8_t, uint8_t);
uint8_t xor (GameBoy *, uint8_t, uint8_t);

uint8_t inc(GameBoy *, uint8_t);
uint8_t dec(GameBoy *, uint8_t);

uint8_t add_byte(GameBoy *, uint8_t, uint8_t);
uint8_t add_byte_carry(GameBoy *, uint8_t, uint8_t);
uint16_t add_short(GameBoy *, uint16_t, uint16_t);
uint16_t add_sp_signed_byte(GameBoy *, uint16_t, int8_t);

uint8_t sub_byte(GameBoy *, uint8_t, uint8_t);
uint8_t sub_byte_carry(GameBoy *, uint8_t, uint8_t);

uint8_t rotate_left(GameBoy *, uint8_t, bool);
uint8_t rotate_right(GameBoy *, uint8_t);
uint8_t rotate_left_carry(GameBoy *, uint8_t, bool);
uint8_t rotate_right_carry(GameBoy *, uint8_t, bool);
uint8_t shift_left_arith(GameBoy *, uint8_t);
uint8_t shift_right_arith(GameBoy *, uint8_t);
uint8_t shift_right_logic(GameBoy *, uint8_t);

uint8_t swap(GameBoy *, uint8_t);
void test_bit(GameBoy *, uint8_t, uint8_t);
uint8_t reset_bit(uint8_t, uint8_t);
uint8_t set_bit(uint8_t, uint8_t);

uint8_t daa(GameBoy *, uint8_t);
