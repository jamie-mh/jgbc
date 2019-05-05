#include "jgbc.h"
#include "mmu.h"
#include "cpu.h"
#include "alu.h"
#include "instr.h"

// 0x00: NOP (- - - -)
void op_nop(GameBoy *gb) {
    // Do nothing
}

// 0x01: LD BC, d16 (- - - -)
void op_ld_bc_d16(GameBoy *gb, uint16_t operand) {
    REG(BC) = operand;
}

// 0x02: LD (BC), A (- - - -)
void op_ld_bcp_a(GameBoy *gb) {
    WRITE8(REG(BC), REG(A));
    TICK(1);
}

// 0x03: INC BC (- - - -)
void op_inc_bc(GameBoy *gb) {
    REG(BC)++;
    TICK(1);
}

// 0x04: INC B (Z 0 H -)
void op_inc_b(GameBoy *gb) {
    REG(B) = inc(gb, REG(B));
}

// 0x05: DEC B (Z 1 H -)
void op_dec_b(GameBoy *gb) {
    REG(B) = dec(gb, REG(B));
}

// 0x06: LD B, d8 (- - - -)
void op_ld_b_d8(GameBoy *gb, uint8_t operand) {
    REG(B) = operand;
}

// 0x07: RLCA (0 0 0 C)
void op_rlca(GameBoy *gb) {
    REG(A) = rotate_left_carry(gb, REG(A), false);
}

// 0x08: LD (a16), SP (- - - -)
void op_ld_a16p_sp(GameBoy *gb, uint16_t operand) {
    WRITE16(operand, REG(SP));
    TICK(2);
}

// 0x09: ADD HL, BC (- 0 H C)
void op_add_hl_bc(GameBoy *gb) {
    REG(HL) = add_short(gb, REG(HL), REG(BC));
    TICK(1);
}

// 0x0A: LD A, (BC) (- - - -)
void op_ld_a_bcp(GameBoy *gb) {
    REG(A) = READ8(REG(BC));
    TICK(1);
}

// 0x0B: DEC BC (- - - -)
void op_dec_bc(GameBoy *gb) {
    REG(BC)--;
    TICK(1);
}

// 0x0C: INC C (Z 0 H -)
void op_inc_c(GameBoy *gb) {
    REG(C) = inc(gb, REG(C));
}

// 0x0D: DEC C (Z 1 H -)
void op_dec_c(GameBoy *gb) {
    REG(C) = dec(gb, REG(C));
}

// 0x0E: LD C, d8 (- - - -)
void op_ld_c_d8(GameBoy *gb, uint8_t operand) {
    REG(C) = operand;
}

// 0x0F: RRCA (0 0 0 C)
void op_rrca(GameBoy *gb) {
    REG(A) = rotate_right_carry(gb, REG(A), false);
}

// 0x10: STOP (- - - -)
void op_stop(GameBoy *gb) {
    // Do nothing
}

// 0x11: LD DE, d16 (- - - -)
void op_ld_de_d16(GameBoy *gb, uint16_t operand) {
    REG(DE) = operand;
}

// 0x12: LD (DE), A (- - - -)
void op_ld_dep_a(GameBoy *gb) {
    WRITE8(REG(DE), REG(A));
    TICK(1);
}

// 0x13: INC DE (- - - -)
void op_inc_de(GameBoy *gb) {
    REG(DE)++;
    TICK(1);
}

// 0x14: INC D (Z 0 H -)
void op_inc_d(GameBoy *gb) {
    REG(D) = inc(gb, REG(D));
}

// 0x15: DEC D (Z 1 H -)
void op_dec_d(GameBoy *gb) {
    REG(D) = dec(gb, REG(D));
}

// 0x16: LD D, d8 (- - - -)
void op_ld_d_d8(GameBoy *gb, uint8_t operand) {
    REG(D) = operand;
}

// 0x17: RLA (0 0 0 C)
void op_rla(GameBoy *gb) {
    REG(A) = rotate_left(gb, REG(A), false);
}

// 0x18: JR r8 (- - - -)
void op_jr_r8(GameBoy *gb, int8_t operand) {
    REG(PC) += operand;
    TICK(1);
}

// 0x19: ADD HL, DE (- 0 H C)
void op_add_hl_de(GameBoy *gb) {
    REG(HL) = add_short(gb, REG(HL), REG(DE));
    TICK(1);
}

// 0x1A: LD A, (DE) (- - - -)
void op_ld_a_dep(GameBoy *gb) {
    REG(A) = READ8(REG(DE));
    TICK(1);
}

// 0x1B: DEC DE (- - - -)
void op_dec_de(GameBoy *gb) {
    REG(DE)--;
    TICK(1);
}

// 0x1C: INC E (Z 0 H -)
void op_inc_e(GameBoy *gb) {
    REG(E) = inc(gb, REG(E));
}

// 0x1D: DEC E (Z 1 H -)
void op_dec_e(GameBoy *gb) {
    REG(E) = dec(gb, REG(E));
}

// 0x1E: LD E, d8 (- - - -)
void op_ld_e_d8(GameBoy *gb, uint8_t operand) {
    REG(E) = operand;
}

// 0x1F: RRA (0 0 0 C)
void op_rra(GameBoy *gb) {
    REG(A) = rotate_right(gb, REG(A));
    FSET(FLAG_ZERO, 0);
}

// 0x20: JR NZ, r8 (- - - -)
void op_jr_nz_r8(GameBoy *gb, int8_t operand) {
    if(!FGET(FLAG_ZERO)) {
        op_jr_r8(gb, operand);
    }
}

// 0x21: LD HL, d16 (- - - -)
void op_ld_hl_d16(GameBoy *gb, uint16_t operand) {
    REG(HL) = operand;
}

// 0x22: LD (HL+), A (- - - -)
void op_ld_hlpp_a(GameBoy *gb) {
    WRITE8(REG(HL), REG(A));
    REG(HL)++;
    TICK(1);
}

// 0x23: INC HL (- - - -)
void op_inc_hl(GameBoy *gb) {
    REG(HL)++;
    TICK(1);
}

// 0x24: INC H (Z 0 H -)
void op_inc_h(GameBoy *gb) {
    REG(H) = inc(gb, REG(H));
}

// 0x25: DEC H (Z 1 H -)
void op_dec_h(GameBoy *gb) {
    REG(H) = dec(gb, REG(H));
}

// 0x26: LD H, d8 (- - - -)
void op_ld_h_d8(GameBoy *gb, uint8_t operand) {
    REG(H) = operand;
}

// 0x27: DAA (Z - 0 C)
void op_daa(GameBoy *gb) {
    REG(A) = daa(gb, REG(A));
}

// 0x28: JR Z, r8 (- - - -)
void op_jr_z_r8(GameBoy *gb, int8_t operand) {
    if(FGET(FLAG_ZERO)) {
        op_jr_r8(gb, operand);
    }
}

// 0x29: ADD HL, HL (- 0 H C)
void op_add_hl_hl(GameBoy *gb) {
    REG(HL) = add_short(gb, REG(HL), REG(HL));
    TICK(1);
}

// 0x2A: LD A, (HL+) (- - - -)
void op_ld_a_hlpp(GameBoy *gb) {
    REG(A) = READ8(REG(HL));
    REG(HL)++;
    TICK(1);
}

// 0x2B: DEC HL (- - - -)
void op_dec_hl(GameBoy *gb) {
    REG(HL)--;
    TICK(1);
}

// 0x2C: INC L (Z 0 H -)
void op_inc_l(GameBoy *gb) {
    REG(L) = inc(gb, REG(L));
}

// 0x2D: DEC L (Z 1 H -)
void op_dec_l(GameBoy *gb) {
    REG(L) = dec(gb, REG(L));
}

// 0x2E: LD L, d8 (- - - -)
void op_ld_l_d8(GameBoy *gb, uint8_t operand) {
    REG(L) = operand;
}

// 0x2F: CPL (- 1 1 -)
void op_cpl(GameBoy *gb) {
    REG(A) = ~REG(A);
    FSET(FLAG_SUBTRACT, 1);
    FSET(FLAG_HALFCARRY, 1);
}

// 0x30: JR NC, r8 (- - - -)
void op_jr_nc_r8(GameBoy *gb, int8_t operand) {
    if(!FGET(FLAG_CARRY)) {
        op_jr_r8(gb, operand);
    }
}

// 0x31: LD SP, d16 (- - - -)
void op_ld_sp_d16(GameBoy *gb, uint16_t operand) {
    REG(SP) = operand;
}

// 0x32: LD (HL-), A (- - - -)
void op_ld_hlmp_a(GameBoy *gb) {
    WRITE8(REG(HL), REG(A));
    REG(HL)--;
    TICK(1);
}

// 0x33: INC SP (- - - -)
void op_inc_sp(GameBoy *gb) {
    REG(SP)++;
    TICK(1);
}

// 0x34: INC (HL) (Z 0 H -)
void op_inc_hlp(GameBoy *gb) {
    WRITE8(REG(HL), inc(gb, READ8(REG(HL))));
    TICK(2);
}

// 0x35: DEC (HL) (Z 1 H -)
void op_dec_hlp(GameBoy *gb) {
    WRITE8(REG(HL), dec(gb, READ8(REG(HL))));
    TICK(2);
}

// 0x36: LD (HL), d8 (- - - -)
void op_ld_hlp_d8(GameBoy *gb, uint8_t operand) {
    WRITE8(REG(HL), operand);
    TICK(1);
}

// 0x37: SCF (- 0 0 1)
void op_scf(GameBoy *gb) {
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);
    FSET(FLAG_CARRY, 1);
}

// 0x38: JR C, r8 (- - - -)
void op_jr_c_r8(GameBoy *gb, int8_t operand) {
    if(FGET(FLAG_CARRY)) {
        op_jr_r8(gb, operand);
    }
}

// 0x39: ADD HL, SP (- 0 H C)
void op_add_hl_sp(GameBoy *gb) {
    REG(HL) = add_short(gb, REG(HL), REG(SP));
    TICK(1);
}

// 0x3A: LD A, (HL-) (- - - -)
void op_ld_a_hlmp(GameBoy *gb) {
    REG(A) = READ8(REG(HL));
    REG(HL)--;
    TICK(1);
}

// 0x3B: DEC SP (- - - -)
void op_dec_sp(GameBoy *gb) {
    REG(SP)--;
    TICK(1);
}

// 0x3C: INC A (Z 0 H -)
void op_inc_a(GameBoy *gb) {
    REG(A) = inc(gb, REG(A));
}

// 0x3D: DEC A (Z 1 H -)
void op_dec_a(GameBoy *gb) {
    REG(A) = dec(gb, REG(A));
}

// 0x3E: LD A, d8 (- - - -)
void op_ld_a_d8(GameBoy *gb, uint8_t operand) {
    REG(A) = operand;
}

// 0x3F: CCF (- 0 0 C)
void op_ccf(GameBoy *gb) {
    FSET(FLAG_SUBTRACT, 0);
    FSET(FLAG_HALFCARRY, 0);
    FSET(FLAG_CARRY, !FGET(FLAG_CARRY));
}

// 0x40: LD B, B (- - - -)
void op_ld_b_b(GameBoy *gb) {
    REG(B) = REG(B);
}

// 0x41: LD B, C (- - - -)
void op_ld_b_c(GameBoy *gb) {
    REG(B) = REG(C);
}

// 0x42: LD B, D (- - - -)
void op_ld_b_d(GameBoy *gb) {
    REG(B) = REG(D);
}

// 0x43: LD B, E (- - - -)
void op_ld_b_e(GameBoy *gb) {
    REG(B) = REG(E);
}

// 0x44: LD B, H (- - - -)
void op_ld_b_h(GameBoy *gb) {
    REG(B) = REG(H);
}

// 0x45: LD B, L (- - - -)
void op_ld_b_l(GameBoy *gb) {
    REG(B) = REG(L);
}

// 0x46: LD B, (HL) (- - - -)
void op_ld_b_hlp(GameBoy *gb) {
    REG(B) = READ8(REG(HL));
    TICK(1);
}

// 0x47: LD B, A (- - - -)
void op_ld_b_a(GameBoy *gb) {
    REG(B) = REG(A);
}

// 0x48: LD C, B (- - - -)
void op_ld_c_b(GameBoy *gb) {
    REG(C) = REG(B);
}

// 0x49: LD C, C (- - - -)
void op_ld_c_c(GameBoy *gb) {
    REG(C) = REG(C);
}

// 0x4A: LD C, D (- - - -)
void op_ld_c_d(GameBoy *gb) {
    REG(C) = REG(D);
}

// 0x4B: LD C, E (- - - -)
void op_ld_c_e(GameBoy *gb) {
    REG(C) = REG(E);
}

// 0x4C: LD C, H (- - - -)
void op_ld_c_h(GameBoy *gb) {
    REG(C) = REG(H);
}

// 0x4D: LD C, L (- - - -)
void op_ld_c_l(GameBoy *gb) {
    REG(C) = REG(L);
}

// 0x4E: LD C, (HL) (- - - -)
void op_ld_c_hlp(GameBoy *gb) {
    REG(C) = READ8(REG(HL));
    TICK(1);
}

// 0x4F: LD C, A (- - - -)
void op_ld_c_a(GameBoy *gb) {
    REG(C) = REG(A);
}

// 0x50: LD D, B (- - - -)
void op_ld_d_b(GameBoy *gb) {
    REG(D) = REG(B);
}

// 0x51: LD D, C (- - - -)
void op_ld_d_c(GameBoy *gb) {
    REG(D) = REG(C);
}

// 0x52: LD D, D (- - - -)
void op_ld_d_d(GameBoy *gb) {
    REG(D) = REG(D);
}

// 0x53: LD D, E (- - - -)
void op_ld_d_e(GameBoy *gb) {
    REG(D) = REG(E);
}

// 0x54: LD D, H (- - - -)
void op_ld_d_h(GameBoy *gb) {
    REG(D) = REG(H);
}

// 0x55: LD D, L (- - - -)
void op_ld_d_l(GameBoy *gb) {
    REG(D) = REG(L);
}

// 0x56: LD D, (HL) (- - - -)
void op_ld_d_hlp(GameBoy *gb) {
    REG(D) = READ8(REG(HL));
    TICK(1);
}

// 0x57: LD D, A (- - - -)
void op_ld_d_a(GameBoy *gb) {
    REG(D) = REG(A);
}

// 0x58: LD E, B (- - - -)
void op_ld_e_b(GameBoy *gb) {
    REG(E) = REG(B);
}

// 0x59: LD E, C (- - - -)
void op_ld_e_c(GameBoy *gb) {
    REG(E) = REG(C);
}

// 0x5A: LD E, D (- - - -)
void op_ld_e_d(GameBoy *gb) {
    REG(E) = REG(D);
}

// 0x5B: LD E, E (- - - -)
void op_ld_e_e(GameBoy *gb) {
    REG(E) = REG(E);
}

// 0x5C: LD E, H (- - - -)
void op_ld_e_h(GameBoy *gb) {
    REG(E) = REG(H);
}

// 0x5D: LD E, L (- - - -)
void op_ld_e_l(GameBoy *gb) {
    REG(E) = REG(L);
}

// 0x5E: LD E, (HL) (- - - -)
void op_ld_e_hlp(GameBoy *gb) {
    REG(E) = READ8(REG(HL));
    TICK(1);
}

// 0x5F: LD E, A (- - - -)
void op_ld_e_a(GameBoy *gb) {
    REG(E) = REG(A);
}

// 0x60: LD H, B (- - - -)
void op_ld_h_b(GameBoy *gb) {
    REG(H) = REG(B);
}

// 0x61: LD H, C (- - - -)
void op_ld_h_c(GameBoy *gb) {
    REG(H) = REG(C);
}

// 0x62: LD H, D (- - - -)
void op_ld_h_d(GameBoy *gb) {
    REG(H) = REG(D);
}

// 0x63: LD H, E (- - - -)
void op_ld_h_e(GameBoy *gb) {
    REG(H) = REG(E);
}

// 0x64: LD H, H (- - - -)
void op_ld_h_h(GameBoy *gb) {
    REG(H) = REG(H);
}

// 0x65: LD H, L (- - - -)
void op_ld_h_l(GameBoy *gb) {
    REG(H) = REG(L);
}

// 0x66: LD H, (HL) (- - - -)
void op_ld_h_hlp(GameBoy *gb) {
    REG(H) = READ8(REG(HL));
    TICK(1);
}

// 0x67: LD H, A (- - - -)
void op_ld_h_a(GameBoy *gb) {
    REG(H) = REG(A);
}

// 0x68: LD L, B (- - - -)
void op_ld_l_b(GameBoy *gb) {
    REG(L) = REG(B);
}

// 0x69: LD L, C (- - - -)
void op_ld_l_c(GameBoy *gb) {
    REG(L) = REG(C);
}

// 0x6A: LD L, D (- - - -)
void op_ld_l_d(GameBoy *gb) {
    REG(L) = REG(D);
}

// 0x6B: LD L, E (- - - -)
void op_ld_l_e(GameBoy *gb) {
    REG(L) = REG(E);
}

// 0x6C: LD L, H (- - - -)
void op_ld_l_h(GameBoy *gb) {
    REG(L) = REG(H);
}

// 0x6D: LD L, L (- - - -)
void op_ld_l_l(GameBoy *gb) {
    REG(L) = REG(L);
}

// 0x6E: LD L, (HL) (- - - -)
void op_ld_l_hlp(GameBoy *gb) {
    REG(L) = READ8(REG(HL));
    TICK(1);
}

// 0x6F: LD L, A (- - - -)
void op_ld_l_a(GameBoy *gb) {
    REG(L) = REG(A);
}

// 0x70: LD (HL), B (- - - -)
void op_ld_hlp_b(GameBoy *gb) {
    WRITE8(REG(HL), REG(B));
    TICK(1);
}

// 0x71: LD (HL), C (- - - -)
void op_ld_hlp_c(GameBoy *gb) {
    WRITE8(REG(HL), REG(C));
    TICK(1);
}

// 0x72: LD (HL), D (- - - -)
void op_ld_hlp_d(GameBoy *gb) {
    WRITE8(REG(HL), REG(D));
    TICK(1);
}

// 0x73: LD (HL), E (- - - -)
void op_ld_hlp_e(GameBoy *gb) {
    WRITE8(REG(HL), REG(E));
    TICK(1);
}

// 0x74: LD (HL), H (- - - -)
void op_ld_hlp_h(GameBoy *gb) {
    WRITE8(REG(HL), REG(H));
    TICK(1);
}

// 0x75: LD (HL), L (- - - -)
void op_ld_hlp_l(GameBoy *gb) {
    WRITE8(REG(HL), REG(L));
    TICK(1);
}

// 0x76: HALT (- - - -)
void op_halt(GameBoy *gb) {
    gb->cpu.is_halted = true;
}

// 0x77: LD (HL), A (- - - -)
void op_ld_hlp_a(GameBoy *gb) {
    WRITE8(REG(HL), REG(A));
    TICK(1);
}

// 0x78: LD A, B (- - - -)
void op_ld_a_b(GameBoy *gb) {
    REG(A) = REG(B);
}

// 0x79: LD A, C (- - - -)
void op_ld_a_c(GameBoy *gb) {
    REG(A) = REG(C);
}

// 0x7A: LD A, D (- - - -)
void op_ld_a_d(GameBoy *gb) {
    REG(A) = REG(D);
}

// 0x7B: LD A, E (- - - -)
void op_ld_a_e(GameBoy *gb) {
    REG(A) = REG(E);
}

// 0x7C: LD A, H (- - - -)
void op_ld_a_h(GameBoy *gb) {
    REG(A) = REG(H);
}

// 0x7D: LD A, L (- - - -)
void op_ld_a_l(GameBoy *gb) {
    REG(A) = REG(L);
}

// 0x7E: LD A, (HL) (- - - -)
void op_ld_a_hlp(GameBoy *gb) {
    REG(A) = READ8(REG(HL));
    TICK(1);
}

// 0x7F: LD A, A (- - - -)
void op_ld_a_a(GameBoy *gb) {
    REG(A) = REG(A);
}

// 0x80: ADD A, B (Z 0 H C)
void op_add_a_b(GameBoy *gb) {
    REG(A) = add_byte(gb, REG(A), REG(B));
}

// 0x81: ADD A, C (Z 0 H C)
void op_add_a_c(GameBoy *gb) {
    REG(A) = add_byte(gb, REG(A), REG(C));
}

// 0x82: ADD A, D (Z 0 H C)
void op_add_a_d(GameBoy *gb) {
    REG(A) = add_byte(gb, REG(A), REG(D));
}

// 0x83: ADD A, E (Z 0 H C)
void op_add_a_e(GameBoy *gb) {
    REG(A) = add_byte(gb, REG(A), REG(E));
}

// 0x84: ADD A, H (Z 0 H C)
void op_add_a_h(GameBoy *gb) {
    REG(A) = add_byte(gb, REG(A), REG(H));
}

// 0x85: ADD A, L (Z 0 H C)
void op_add_a_l(GameBoy *gb) {
    REG(A) = add_byte(gb, REG(A), REG(L));
}

// 0x86: ADD A, (HL) (Z 0 H C)
void op_add_a_hlp(GameBoy *gb) {
    REG(A) = add_byte(gb, REG(A), READ8(REG(HL)));
    TICK(1);
}

// 0x87: ADD A, A (Z 0 H C)
void op_add_a_a(GameBoy *gb) {
    REG(A) = add_byte(gb, REG(A), REG(A));
}

// 0x88: ADC A, B (Z 0 H C)
void op_adc_a_b(GameBoy *gb) {
    REG(A) = add_byte_carry(gb, REG(A), REG(B));
}

// 0x89: ADC A, C (Z 0 H C)
void op_adc_a_c(GameBoy *gb) {
    REG(A) = add_byte_carry(gb, REG(A), REG(C));
}

// 0x8A: ADC A, D (Z 0 H C)
void op_adc_a_d(GameBoy *gb) {
    REG(A) = add_byte_carry(gb, REG(A), REG(D));
}

// 0x8B: ADC A, E (Z 0 H C)
void op_adc_a_e(GameBoy *gb) {
    REG(A) = add_byte_carry(gb, REG(A), REG(E));
}

// 0x8C: ADC A, H (Z 0 H C)
void op_adc_a_h(GameBoy *gb) {
    REG(A) = add_byte_carry(gb, REG(A), REG(H));
}

// 0x8D: ADC A, L (Z 0 H C)
void op_adc_a_l(GameBoy *gb) {
    REG(A) = add_byte_carry(gb, REG(A), REG(L));
}

// 0x8E: ADC A, (HL) (Z 0 H C)
void op_adc_a_hlp(GameBoy *gb) {
    REG(A) = add_byte_carry(gb, REG(A), READ8(REG(HL)));
    TICK(1);
}

// 0x8F: ADC A, A (Z 0 H C)
void op_adc_a_a(GameBoy *gb) {
    REG(A) = add_byte_carry(gb, REG(A), REG(A));
}

// 0x90: SUB B (Z 1 H C)
void op_sub_b(GameBoy *gb) {
    REG(A) = sub_byte(gb, REG(A), REG(B));
}

// 0x91: SUB C (Z 1 H C)
void op_sub_c(GameBoy *gb) {
    REG(A) = sub_byte(gb, REG(A), REG(C));
}

// 0x92: SUB D (Z 1 H C)
void op_sub_d(GameBoy *gb) {
    REG(A) = sub_byte(gb, REG(A), REG(D));
}

// 0x93: SUB E (Z 1 H C)
void op_sub_e(GameBoy *gb) {
    REG(A) = sub_byte(gb, REG(A), REG(E));
}

// 0x94: SUB H (Z 1 H C)
void op_sub_h(GameBoy *gb) {
    REG(A) = sub_byte(gb, REG(A), REG(H));
}

// 0x95: SUB L (Z 1 H C)
void op_sub_l(GameBoy *gb) {
    REG(A) = sub_byte(gb, REG(A), REG(L));
}

// 0x96: SUB (HL) (Z 1 H C)
void op_sub_hlp(GameBoy *gb) {
    REG(A) = sub_byte(gb, REG(A), READ8(REG(HL)));
    TICK(1);
}

// 0x97: SUB A (Z 1 H C)
void op_sub_a(GameBoy *gb) {
    REG(A) = sub_byte(gb, REG(A), REG(A));
}

// 0x98: SBC A, B (Z 1 H C)
void op_sbc_a_b(GameBoy *gb) {
    REG(A) = sub_byte_carry(gb, REG(A), REG(B));
}

// 0x99: SBC A, C (Z 1 H C)
void op_sbc_a_c(GameBoy *gb) {
    REG(A) = sub_byte_carry(gb, REG(A), REG(C));
}

// 0x9A: SBC A, D (Z 1 H C)
void op_sbc_a_d(GameBoy *gb) {
    REG(A) = sub_byte_carry(gb, REG(A), REG(D));
}

// 0x9B: SBC A, E (Z 1 H C)
void op_sbc_a_e(GameBoy *gb) {
    REG(A) = sub_byte_carry(gb, REG(A), REG(E));
}

// 0x9C: SBC A, H (Z 1 H C)
void op_sbc_a_h(GameBoy *gb) {
    REG(A) = sub_byte_carry(gb, REG(A), REG(H));
}

// 0x9D: SBC A, L (Z 1 H C)
void op_sbc_a_l(GameBoy *gb) {
    REG(A) = sub_byte_carry(gb, REG(A), REG(L));
}

// 0x9E: SBC A, (HL) (Z 1 H C)
void op_sbc_a_hlp(GameBoy *gb) {
    REG(A) = sub_byte_carry(gb, REG(A), READ8(REG(HL)));
    TICK(1);
}

// 0x9F: SBC A, A (Z 1 H C)
void op_sbc_a_a(GameBoy *gb) {
    REG(A) = sub_byte_carry(gb, REG(A), REG(A));
}

// 0xA0: AND B (Z 0 1 0)
void op_and_b(GameBoy *gb) {
    REG(A) = and(gb, REG(A), REG(B));
}

// 0xA1: AND C (Z 0 1 0)
void op_and_c(GameBoy *gb) {
    REG(A) = and(gb, REG(A), REG(C));
}

// 0xA2: AND D (Z 0 1 0)
void op_and_d(GameBoy *gb) {
    REG(A) = and(gb, REG(A), REG(D));
}

// 0xA3: AND E (Z 0 1 0)
void op_and_e(GameBoy *gb) {
    REG(A) = and(gb, REG(A), REG(E));
}

// 0xA4: AND H (Z 0 1 0)
void op_and_h(GameBoy *gb) {
    REG(A) = and(gb, REG(A), REG(H));
}

//and(gb,  0xA5: AND L (Z 0 1 0)
void op_and_l(GameBoy *gb) {
    REG(A) = and(gb, REG(A), REG(L));
}

// 0xA6: AND (HL) (Z 0 1 0)
void op_and_hlp(GameBoy *gb) {
    REG(A) = and(gb, REG(A), READ8(REG(HL)));
    TICK(1);
}

// 0xA7: AND A (Z 0 1 0)
void op_and_a(GameBoy *gb) {
    REG(A) = and(gb, REG(A), REG(A));
}

// 0xA8: XOR B (Z 0 0 0)
void op_xor_b(GameBoy *gb) {
    REG(A) = xor(gb, REG(B), REG(A));
}

// 0xA9: XOR C (Z 0 0 0)
void op_xor_c(GameBoy *gb) {
    REG(A) = xor(gb, REG(C), REG(A));
}

// 0xAA: XOR D (Z 0 0 0)
void op_xor_d(GameBoy *gb) {
    REG(A) = xor(gb, REG(D), REG(A));
}

// 0xAB: XOR E (Z 0 0 0)
void op_xor_e(GameBoy *gb) {
    REG(A) = xor(gb, REG(E), REG(A));
}

// 0xAC: XOR H (Z 0 0 0)
void op_xor_h(GameBoy *gb) {
    REG(A) = xor(gb, REG(H), REG(A));
}

// 0xAD: XOR L (Z 0 0 0)
void op_xor_l(GameBoy *gb) {
    REG(A) = xor(gb, REG(L), REG(A));
}

// 0xAE: XOR (HL) (Z 0 0 0)
void op_xor_hlp(GameBoy *gb) {
    REG(A) = xor(gb, READ8(REG(HL)), REG(A));
    TICK(1);
}

// 0xAF: XOR A (Z 0 0 0)
void op_xor_a(GameBoy *gb) { 
    REG(A) = xor(gb, REG(A), REG(A));
}

// 0xB0: OR B (Z 0 0 0)
void op_or_b(GameBoy *gb) {
    REG(A) = or(gb, REG(A), REG(B));
}

// 0xB1: OR C (Z 0 0 0)
void op_or_c(GameBoy *gb) {
    REG(A) = or(gb, REG(A), REG(C));
}

// 0xB2: OR D (Z 0 0 0)
void op_or_d(GameBoy *gb) {
    REG(A) = or(gb, REG(A), REG(D));
}

// 0xB3: OR E (Z 0 0 0)
void op_or_e(GameBoy *gb) {
    REG(A) = or(gb, REG(A), REG(E));
}

// 0xB4: OR H (Z 0 0 0)
void op_or_h(GameBoy *gb) {
    REG(A) = or(gb, REG(A), REG(H));
}

// 0xB5: OR L (Z 0 0 0)
void op_or_l(GameBoy *gb) {
    REG(A) = or(gb, REG(A), REG(L));
}

// 0xB6: OR (HL) (Z 0 0 0)
void op_or_hlp(GameBoy *gb) {
    REG(A) = or(gb, REG(A), READ8(REG(HL)));
    TICK(1);
}

// 0xB7: OR A (Z 0 0 0)
void op_or_a(GameBoy *gb) {
    REG(A) = or(gb, REG(A), REG(A));
}

// 0xB8: CP B (Z 1 H C)
void op_cp_b(GameBoy *gb) {
    sub_byte(gb, REG(A), REG(B));
}

// 0xB9: CP C (Z 1 H C)
void op_cp_c(GameBoy *gb) {
    sub_byte(gb, REG(A), REG(C));
}

// 0xBA: CP D (Z 1 H C)
void op_cp_d(GameBoy *gb) {
    sub_byte(gb, REG(A), REG(D));
}

// 0xBB: CP E (Z 1 H C)
void op_cp_e(GameBoy *gb) {
    sub_byte(gb, REG(A), REG(E));
}

// 0xBC: CP H (Z 1 H C)
void op_cp_h(GameBoy *gb) {
    sub_byte(gb, REG(A), REG(H));
}

// 0xBD: CP L (Z 1 H C)
void op_cp_l(GameBoy *gb) {
    sub_byte(gb, REG(A), REG(L));
}

// 0xBE: CP (HL) (Z 1 H C)
void op_cp_hlp(GameBoy *gb) {
    sub_byte(gb, REG(A), READ8(REG(HL)));
    TICK(1);
}

// 0xBF: CP A (Z 1 H C)
void op_cp_a(GameBoy *gb) {
    sub_byte(gb, REG(A), REG(A));
}

// 0xC0: RET NZ (- - - -)
void op_ret_nz(GameBoy *gb) {
    if(!FGET(FLAG_ZERO)) {
        op_ret(gb);
    } 
    TICK(1);
}

// 0xC1: POP BC (- - - -)
void op_pop_bc(GameBoy *gb) {
    REG(BC) = POP16();
    TICK(2);
}

// 0xC2: JP NZ, a16 (- - - -)
void op_jp_nz_a16(GameBoy *gb, uint16_t operand) {
    if(!FGET(FLAG_ZERO)) {
        op_jp_a16(gb, operand);
    } 
}

// 0xC3: JP a16 (- - - -)
void op_jp_a16(GameBoy *gb, uint16_t operand) {
    REG(PC) = operand;
    TICK(1);
}

// 0xC4: CALL NZ, a16 (- - - -)
void op_call_nz_a16(GameBoy *gb, uint16_t operand) {
    if(!FGET(FLAG_ZERO)) {
        op_call_a16(gb, operand); 
    } 
}

// 0xC5: PUSH BC (- - - -)
void op_push_bc(GameBoy *gb) {
    PUSH16(REG(BC));
    TICK(3);
}

// 0xC6: ADD A, d8 (Z 0 H C)
void op_add_a_d8(GameBoy *gb, uint8_t operand) {
    REG(A) = add_byte(gb, REG(A), operand);
}

// 0xC7: RST 00H (- - - -)
void op_rst_00h(GameBoy *gb) {
    PUSH16(REG(PC));
    REG(PC) = 0x0;
    TICK(3);
}

// 0xC8: RET Z (- - - -)
void op_ret_z(GameBoy *gb) {
    if(FGET(FLAG_ZERO)) {
        op_ret(gb); 
    }
    TICK(1);
}

// 0xC9: RET (- - - -)
void op_ret(GameBoy *gb) {
    REG(PC) = POP16();
    TICK(3);
}

// 0xCA: JP Z, a16 (- - - -)
void op_jp_z_a16(GameBoy *gb, uint16_t operand) {
    if(FGET(FLAG_ZERO)) {
        op_jp_a16(gb, operand);
    } 
}

// 0xCB: PREFIX CB (- - - -)
void op_prefix_cb(GameBoy *gb) {
    // Do nothing, placeholder function
}

// 0xCC: CALL Z, a16 (- - - -)
void op_call_z_a16(GameBoy *gb, uint16_t operand) {
    if(FGET(FLAG_ZERO)) {
        op_call_a16(gb, operand); 
    } 
}

// 0xCD: CALL a16 (- - - -)
void op_call_a16(GameBoy *gb, uint16_t operand) {
    PUSH16(REG(PC));
    REG(PC) = operand;
    TICK(3);
}

// 0xCE: ADC A, d8 (Z 0 H C)
void op_adc_a_d8(GameBoy *gb, uint8_t operand) {
    REG(A) = add_byte_carry(gb, REG(A), operand);
}

// 0xCF: RST 08H (- - - -)
void op_rst_08h(GameBoy *gb) {
    PUSH16(REG(PC));
    REG(PC) = 0x8;
    TICK(3);
}

// 0xD0: RET NC (- - - -)
void op_ret_nc(GameBoy *gb) {
    if(!FGET(FLAG_CARRY)) {
        op_ret(gb); 
    }
    TICK(1);
}

// 0xD1: POP DE (- - - -)
void op_pop_de(GameBoy *gb) {
    REG(DE) = POP16();
    TICK(2);
}

// 0xD2: JP NC, a16 (- - - -)
void op_jp_nc_a16(GameBoy *gb, uint16_t operand) {
    if(!FGET(FLAG_CARRY)) {
        op_jp_a16(gb, operand);
    }
}

// 0xD4: CALL NC, a16 (- - - -)
void op_call_nc_a16(GameBoy *gb, uint16_t operand) {
    if(!FGET(FLAG_CARRY)) {
        op_call_a16(gb, operand); 
    }
}

// 0xD5: PUSH DE (- - - -)
void op_push_de(GameBoy *gb) {
    PUSH16(REG(DE));
    TICK(3);
}

// 0xD6: SUB d8 (Z 1 H C)
void op_sub_d8(GameBoy *gb, uint8_t operand) {
    REG(A) = sub_byte(gb, REG(A), operand);
}

// 0xD7: RST 10H (- - - -)
void op_rst_10h(GameBoy *gb) {
    PUSH16(REG(PC));
    REG(PC) = 0x10;
    TICK(3);
}

// 0xD8: RET C (- - - -)
void op_ret_c(GameBoy *gb) {
    if(FGET(FLAG_CARRY)) {
        op_ret(gb);
    }
    TICK(1);
}

// 0xD9: RETI (- - - -)
void op_reti(GameBoy *gb) {
    op_ret(gb); 
    op_ei(gb);
}

// 0xDA: JP C, a16 (- - - -)
void op_jp_c_a16(GameBoy *gb, uint16_t operand) {
    if(FGET(FLAG_CARRY)) {
        op_jp_a16(gb, operand);
    }
}

// 0xDC: CALL C, a16 (- - - -)
void op_call_c_a16(GameBoy *gb, uint16_t operand) {
    if(FGET(FLAG_CARRY)) {
        op_call_a16(gb, operand); 
    }
}

// 0xDE: SBC A, d8 (Z 1 H C)
void op_sbc_a_d8(GameBoy *gb, uint8_t operand) {
    REG(A) = sub_byte_carry(gb, REG(A), operand);
}

// 0xDF: RST 18H (- - - -)
void op_rst_18h(GameBoy *gb) {
    PUSH16(REG(PC));
    REG(PC) = 0x18;
    TICK(3);
}

// 0xE0: LDH (a8), A (- - - -)
void op_ldh_a8p_a(GameBoy *gb, uint8_t operand) {
    WRITE8(0xFF00 + operand, REG(A));
    TICK(1);
}

// 0xE1: POP HL (- - - -)
void op_pop_hl(GameBoy *gb) {
    REG(HL) = POP16();
    TICK(2);
}

// 0xE2: LD (C), A (- - - -)
void op_ld_cp_a(GameBoy *gb) {
    WRITE8(0xFF00 + REG(C), REG(A));
    TICK(1);
}

// 0xE5: PUSH HL (- - - -)
void op_push_hl(GameBoy *gb) {
    PUSH16(REG(HL));
    TICK(3);
}

// 0xE6: AND d8 (Z 0 1 0)
void op_and_d8(GameBoy *gb, uint8_t operand) {
    REG(A) = and(gb, REG(A), operand);
}

// 0xE7: RST 20H (- - - -)
void op_rst_20h(GameBoy *gb) {
    PUSH16(REG(PC));
    REG(PC) = 0x20;
    TICK(3);
}

// 0xE8: ADD SP, r8 (0 0 H C)
void op_add_sp_r8(GameBoy *gb, int8_t operand) {
    REG(SP) = add_sp_signed_byte(gb, REG(SP), operand); 
    TICK(2);
}

// 0xE9: JP (HL) (- - - -)
void op_jp_hlp(GameBoy *gb) {
    REG(PC) = REG(HL);
}

// 0xEA: LD (a16), A (- - - -)
void op_ld_a16p_a(GameBoy *gb, uint16_t operand) {
    WRITE8(operand, REG(A));
    TICK(1);
}

// 0xEE: XOR d8 (Z 0 0 0)
void op_xor_d8(GameBoy *gb, uint8_t operand) {
    REG(A) = xor(gb, operand, REG(A));
}

// 0xEF: RST 28H (- - - -)
void op_rst_28h(GameBoy *gb) {
    PUSH16(REG(PC));
    REG(PC) = 0x28;
    TICK(3);
}

// 0xF0: LDH A, (a8) (- - - -)
void op_ldh_a_a8p(GameBoy *gb, uint8_t operand) {
    REG(A) = READ8(0xFF00 + operand);
    TICK(1);
}

// 0xF1: POP AF (Z N H C)
void op_pop_af(GameBoy *gb) {
    REG(AF) = POP16();
    TICK(2);
    // The unused bits are cleared
    REG(F) &= 0xF0;
}

// 0xF2: LD A, (C) (- - - -)
void op_ld_a_cp(GameBoy *gb) {
    REG(A) = READ8(0xFF00 + REG(C));
    TICK(1);
}

// 0xF3: DI (- - - -)
void op_di(GameBoy *gb) {
   REG(IME) = false; 
}

// 0xF5: PUSH AF (- - - -)
void op_push_af(GameBoy *gb) {
    PUSH16(REG(AF));
    TICK(3);
}

// 0xF6: OR d8 (Z 0 0 0)
void op_or_d8(GameBoy *gb, uint8_t operand) {
    REG(A) = or(gb, REG(A), operand);
}

// 0xF7: RST 30H (- - - -)
void op_rst_30h(GameBoy *gb) {
    PUSH16(REG(PC));
    REG(PC) = 0x30;
    TICK(3);
}

// 0xF8: LD HL, SP+r8 (0 0 H C)
void op_ld_hl_sppr8(GameBoy *gb, int8_t operand) {
    REG(HL) = add_sp_signed_byte(gb, REG(SP), operand); 
    TICK(1);
}

// 0xF9: LD SP, HL (- - - -)
void op_ld_sp_hl(GameBoy *gb) {
    REG(SP) = REG(HL);
}

// 0xFA: LD A, (a16) (- - - -)
void op_ld_a_a16p(GameBoy *gb, uint16_t operand) {
    REG(A) = READ8(operand);
    TICK(2);
}

// 0xFB: EI (- - - -)
void op_ei(GameBoy *gb) {
    REG(IME) = true;
}

// 0xFE: CP d8 (Z 1 H C)
void op_cp_d8(GameBoy *gb, uint8_t operand) {
    sub_byte(gb, REG(A), operand);
}

// 0xFF: RST 38H (- - - -)
void op_rst_38h(GameBoy *gb) {
    PUSH16(REG(PC));
    REG(PC) = 0x38;
    TICK(3);
}

// 0xCB00: RLC B (Z 0 0 C)
void op_rlc_b(GameBoy *gb) {
    REG(B) = rotate_left_carry(gb, REG(B), true);
}

// 0xCB01: RLC C (Z 0 0 C)
void op_rlc_c(GameBoy *gb) {
    REG(C) = rotate_left_carry(gb, REG(C), true);
}

// 0xCB02: RLC D (Z 0 0 C)
void op_rlc_d(GameBoy *gb) {
    REG(D) = rotate_left_carry(gb, REG(D), true);
}

// 0xCB03: RLC E (Z 0 0 C)
void op_rlc_e(GameBoy *gb) {
    REG(E) = rotate_left_carry(gb, REG(E), true);
}

// 0xCB04: RLC H (Z 0 0 C)
void op_rlc_h(GameBoy *gb) {
    REG(H) = rotate_left_carry(gb, REG(H), true);
}

// 0xCB05: RLC L (Z 0 0 C)
void op_rlc_l(GameBoy *gb) {
    REG(L) = rotate_left_carry(gb, REG(L), true);
}

// 0xCB06: RLC (HL) (Z 0 0 C)
void op_rlc_hlp(GameBoy *gb) {
    WRITE8(REG(HL), rotate_left_carry(gb, READ8(REG(HL)), true));
    TICK(2);
}

// 0xCB07: RLC A (Z 0 0 C)
void op_rlc_a(GameBoy *gb) {
    REG(A) = rotate_left_carry(gb, REG(A), true);
}

// 0xCB08: RRC B (Z 0 0 C)
void op_rrc_b(GameBoy *gb) {
    REG(B) = rotate_right_carry(gb, REG(B), true);
}

// 0xCB09: RRC C (Z 0 0 C)
void op_rrc_c(GameBoy *gb) {
    REG(C) = rotate_right_carry(gb, REG(C), true);
}

// 0xCB0A: RRC D (Z 0 0 C)
void op_rrc_d(GameBoy *gb) {
    REG(D) = rotate_right_carry(gb, REG(D), true);
}

// 0xCB0B: RRC E (Z 0 0 C)
void op_rrc_e(GameBoy *gb) {
    REG(E) = rotate_right_carry(gb, REG(E), true);
}

// 0xCB0C: RRC H (Z 0 0 C)
void op_rrc_h(GameBoy *gb) {
    REG(H) = rotate_right_carry(gb, REG(H), true);
}

// 0xCB0D: RRC L (Z 0 0 C)
void op_rrc_l(GameBoy *gb) {
    REG(L) = rotate_right_carry(gb, REG(L), true);
}

// 0xCB0E: RRC (HL) (Z 0 0 C)
void op_rrc_hlp(GameBoy *gb) {
    WRITE8(REG(HL), rotate_right_carry(gb, READ8(REG(HL)), true));
    TICK(2);
}

// 0xCB0F: RRC A (Z 0 0 C)
void op_rrc_a(GameBoy *gb) {
    REG(A) = rotate_right_carry(gb, REG(A), true);
}

// 0xCB10: RL B (Z 0 0 C)
void op_rl_b(GameBoy *gb) {
    REG(B) = rotate_left(gb, REG(B), true);
}

// 0xCB11: RL C (Z 0 0 C)
void op_rl_c(GameBoy *gb) {
    REG(C) = rotate_left(gb, REG(C), true);
}

// 0xCB12: RL D (Z 0 0 C)
void op_rl_d(GameBoy *gb) {
    REG(D) = rotate_left(gb, REG(D), true);
}

// 0xCB13: RL E (Z 0 0 C)
void op_rl_e(GameBoy *gb) {
    REG(E) = rotate_left(gb, REG(E), true);
}

// 0xCB14: RL H (Z 0 0 C)
void op_rl_h(GameBoy *gb) {
    REG(H) = rotate_left(gb, REG(H), true);
}

// 0xCB15: RL L (Z 0 0 C)
void op_rl_l(GameBoy *gb) {
    REG(L) = rotate_left(gb, REG(L), true);
}

// 0xCB16: RL (HL) (Z 0 0 C)
void op_rl_hlp(GameBoy *gb) {
    WRITE8(REG(HL), rotate_left(gb, READ8(REG(HL)), true));
    TICK(2);
}

// 0xCB17: RL A (Z 0 0 C)
void op_rl_a(GameBoy *gb) {
    REG(A) = rotate_left(gb, REG(A), true);
}

// 0xCB18: RR B (Z 0 0 C)
void op_rr_b(GameBoy *gb) {
    REG(B) = rotate_right(gb, REG(B));
}

// 0xCB19: RR C (Z 0 0 C)
void op_rr_c(GameBoy *gb) {
    REG(C) = rotate_right(gb, REG(C));
}

// 0xCB1A: RR D (Z 0 0 C)
void op_rr_d(GameBoy *gb) {
    REG(D) = rotate_right(gb, REG(D));
}

// 0xCB1B: RR E (Z 0 0 C)
void op_rr_e(GameBoy *gb) {
    REG(E) = rotate_right(gb, REG(E));
}

// 0xCB1C: RR H (Z 0 0 C)
void op_rr_h(GameBoy *gb) {
    REG(H) = rotate_right(gb, REG(H));
}

// 0xCB1D: RR L (Z 0 0 C)
void op_rr_l(GameBoy *gb) {
    REG(L) = rotate_right(gb, REG(L));
}

// 0xCB1E: RR (HL) (Z 0 0 C)
void op_rr_hlp(GameBoy *gb) {
    WRITE8(REG(HL), rotate_right(gb, READ8(REG(HL))));
    TICK(2);
}

// 0xCB1F: RR A (Z 0 0 C)
void op_rr_a(GameBoy *gb) {
    REG(A) = rotate_right(gb, REG(A));
}

// 0xCB20: SLA B (Z 0 0 C)
void op_sla_b(GameBoy *gb) {
    REG(B) = shift_left_arith(gb, REG(B));
}

// 0xCB21: SLA C (Z 0 0 C)
void op_sla_c(GameBoy *gb) {
    REG(C) = shift_left_arith(gb, REG(C));
}

// 0xCB22: SLA D (Z 0 0 C)
void op_sla_d(GameBoy *gb) {
    REG(D) = shift_left_arith(gb, REG(D));
}

// 0xCB23: SLA E (Z 0 0 C)
void op_sla_e(GameBoy *gb) {
    REG(E) = shift_left_arith(gb, REG(E));
}

// 0xCB24: SLA H (Z 0 0 C)
void op_sla_h(GameBoy *gb) {
    REG(H) = shift_left_arith(gb, REG(H));
}

// 0xCB25: SLA L (Z 0 0 C)
void op_sla_l(GameBoy *gb) {
    REG(L) = shift_left_arith(gb, REG(L));
}

// 0xCB26: SLA (HL) (Z 0 0 C)
void op_sla_hlp(GameBoy *gb) {
    WRITE8(REG(HL), shift_left_arith(gb, READ8(REG(HL))));
    TICK(2);
}

// 0xCB27: SLA A (Z 0 0 C)
void op_sla_a(GameBoy *gb) {
    REG(A) = shift_left_arith(gb, REG(A));
}

// 0xCB28: SRA B (Z 0 0 0)
void op_sra_b(GameBoy *gb) {
    REG(B) = shift_right_arith(gb, REG(B));
}

// 0xCB29: SRA C (Z 0 0 0)
void op_sra_c(GameBoy *gb) {
    REG(C) = shift_right_arith(gb, REG(C));
}

// 0xCB2A: SRA D (Z 0 0 0)
void op_sra_d(GameBoy *gb) {
    REG(D) = shift_right_arith(gb, REG(D));
}

// 0xCB2B: SRA E (Z 0 0 0)
void op_sra_e(GameBoy *gb) {
    REG(E) = shift_right_arith(gb, REG(E));
}

// 0xCB2C: SRA H (Z 0 0 0)
void op_sra_h(GameBoy *gb) {
    REG(H) = shift_right_arith(gb, REG(H));
}

// 0xCB2D: SRA L (Z 0 0 0)
void op_sra_l(GameBoy *gb) {
    REG(L) = shift_right_arith(gb, REG(L));
}

// 0xCB2E: SRA (HL) (Z 0 0 0)
void op_sra_hlp(GameBoy *gb) {
    WRITE8(REG(HL), shift_right_arith(gb, READ8(REG(HL))));
    TICK(2);
}

// 0xCB2F: SRA A (Z 0 0 0)
void op_sra_a(GameBoy *gb) {
    REG(A) = shift_right_arith(gb, REG(A));
}

// 0xCB30: SWAP B (Z 0 0 0)
void op_swap_b(GameBoy *gb) {
    REG(B) = swap(gb, REG(B));
}

// 0xCB31: SWAP C (Z 0 0 0)
void op_swap_c(GameBoy *gb) {
    REG(C) = swap(gb, REG(C));
}

// 0xCB32: SWAP D (Z 0 0 0)
void op_swap_d(GameBoy *gb) {
    REG(D) = swap(gb, REG(D));
}

// 0xCB33: SWAP E (Z 0 0 0)
void op_swap_e(GameBoy *gb) {
    REG(E) = swap(gb, REG(E));
}

// 0xCB34: SWAP H (Z 0 0 0)
void op_swap_h(GameBoy *gb) {
    REG(H) = swap(gb, REG(H));
}

// 0xCB35: SWAP L (Z 0 0 0)
void op_swap_l(GameBoy *gb) {
    REG(L) = swap(gb, REG(L));
}

// 0xCB36: SWAP (HL) (Z 0 0 0)
void op_swap_hlp(GameBoy *gb) {
    WRITE8(REG(HL), swap(gb, READ8(REG(HL))));
    TICK(2);
}

// 0xCB37: SWAP A (Z 0 0 0)
void op_swap_a(GameBoy *gb) {
    REG(A) = swap(gb, REG(A));
}

// 0xCB38: SRL B (Z 0 0 C)
void op_srl_b(GameBoy *gb) {
    REG(B) = shift_right_logic(gb, REG(B)); 
}

// 0xCB39: SRL C (Z 0 0 C)
void op_srl_c(GameBoy *gb) {
    REG(C) = shift_right_logic(gb, REG(C)); 
}

// 0xCB3A: SRL D (Z 0 0 C)
void op_srl_d(GameBoy *gb) {
    REG(D) = shift_right_logic(gb, REG(D)); 
}

// 0xCB3B: SRL E (Z 0 0 C)
void op_srl_e(GameBoy *gb) {
    REG(E) = shift_right_logic(gb, REG(E)); 
}

// 0xCB3C: SRL H (Z 0 0 C)
void op_srl_h(GameBoy *gb) {
    REG(H) = shift_right_logic(gb, REG(H)); 
}

// 0xCB3D: SRL L (Z 0 0 C)
void op_srl_l(GameBoy *gb) {
    REG(L) = shift_right_logic(gb, REG(L)); 
}

// 0xCB3E: SRL (HL) (Z 0 0 C)
void op_srl_hlp(GameBoy *gb) {
    WRITE8(REG(HL), shift_right_logic(gb, READ8(REG(HL)))); 
    TICK(2);
}

// 0xCB3F: SRL A (Z 0 0 C)
void op_srl_a(GameBoy *gb) {
    REG(A) = shift_right_logic(gb, REG(A)); 
}

// 0xCB40: BIT 0, B (Z 0 1 -)
void op_bit_0_b(GameBoy *gb) {
    test_bit(gb, REG(B), 0); 
}

// 0xCB41: BIT 0, C (Z 0 1 -)
void op_bit_0_c(GameBoy *gb) {
    test_bit(gb, REG(C), 0); 
}

// 0xCB42: BIT 0, D (Z 0 1 -)
void op_bit_0_d(GameBoy *gb) {
    test_bit(gb, REG(D), 0); 
}

// 0xCB43: BIT 0, E (Z 0 1 -)
void op_bit_0_e(GameBoy *gb) {
    test_bit(gb, REG(E), 0); 
}

// 0xCB44: BIT 0, H (Z 0 1 -)
void op_bit_0_h(GameBoy *gb) {
    test_bit(gb, REG(H), 0); 
}

// 0xCB45: BIT 0, L (Z 0 1 -)
void op_bit_0_l(GameBoy *gb) {
    test_bit(gb, REG(L), 0); 
}

// 0xCB46: BIT 0, (HL) (Z 0 1 -)
void op_bit_0_hlp(GameBoy *gb) {
    test_bit(gb, READ8(REG(HL)), 0); 
    TICK(2);
}

// 0xCB47: BIT 0, A (Z 0 1 -)
void op_bit_0_a(GameBoy *gb) {
    test_bit(gb, REG(A), 0); 
}

// 0xCB48: BIT 1, B (Z 0 1 -)
void op_bit_1_b(GameBoy *gb) {
    test_bit(gb, REG(B), 1); 
}

// 0xCB49: BIT 1, C (Z 0 1 -)
void op_bit_1_c(GameBoy *gb) {
    test_bit(gb, REG(C), 1); 
}

// 0xCB4A: BIT 1, D (Z 0 1 -)
void op_bit_1_d(GameBoy *gb) {
    test_bit(gb, REG(D), 1); 
}

// 0xCB4B: BIT 1, E (Z 0 1 -)
void op_bit_1_e(GameBoy *gb) {
    test_bit(gb, REG(E), 1); 
}

// 0xCB4C: BIT 1, H (Z 0 1 -)
void op_bit_1_h(GameBoy *gb) {
    test_bit(gb, REG(H), 1); 
}

// 0xCB4D: BIT 1, L (Z 0 1 -)
void op_bit_1_l(GameBoy *gb) {
    test_bit(gb, REG(L), 1); 
}

// 0xCB4E: BIT 1, (HL) (Z 0 1 -)
void op_bit_1_hlp(GameBoy *gb) {
    test_bit(gb, READ8(REG(HL)), 1);
    TICK(2);
}

// 0xCB4F: BIT 1, A (Z 0 1 -)
void op_bit_1_a(GameBoy *gb) {
    test_bit(gb, REG(A), 1); 
}

// 0xCB50: BIT 2, B (Z 0 1 -)
void op_bit_2_b(GameBoy *gb) {
    test_bit(gb, REG(B), 2); 
}

// 0xCB51: BIT 2, C (Z 0 1 -)
void op_bit_2_c(GameBoy *gb) {
    test_bit(gb, REG(C), 2); 
}

// 0xCB52: BIT 2, D (Z 0 1 -)
void op_bit_2_d(GameBoy *gb) {
    test_bit(gb, REG(D), 2); 
}

// 0xCB53: BIT 2, E (Z 0 1 -)
void op_bit_2_e(GameBoy *gb) {
    test_bit(gb, REG(E), 2); 
}

// 0xCB54: BIT 2, H (Z 0 1 -)
void op_bit_2_h(GameBoy *gb) {
    test_bit(gb, REG(H), 2); 
}

// 0xCB55: BIT 2, L (Z 0 1 -)
void op_bit_2_l(GameBoy *gb) {
    test_bit(gb, REG(L), 2); 
}

// 0xCB56: BIT 2, (HL) (Z 0 1 -)
void op_bit_2_hlp(GameBoy *gb) {
    test_bit(gb, READ8(REG(HL)), 2); 
    TICK(2);
}

// 0xCB57: BIT 2, A (Z 0 1 -)
void op_bit_2_a(GameBoy *gb) {
    test_bit(gb, REG(A), 2); 
}

// 0xCB58: BIT 3, B (Z 0 1 -)
void op_bit_3_b(GameBoy *gb) {
    test_bit(gb, REG(B), 3); 
}

// 0xCB59: BIT 3, C (Z 0 1 -)
void op_bit_3_c(GameBoy *gb) {
    test_bit(gb, REG(C), 3); 
}

// 0xCB5A: BIT 3, D (Z 0 1 -)
void op_bit_3_d(GameBoy *gb) {
    test_bit(gb, REG(D), 3); 
}

// 0xCB5B: BIT 3, E (Z 0 1 -)
void op_bit_3_e(GameBoy *gb) {
    test_bit(gb, REG(E), 3); 
}

// 0xCB5C: BIT 3, H (Z 0 1 -)
void op_bit_3_h(GameBoy *gb) {
    test_bit(gb, REG(H), 3); 
}

// 0xCB5D: BIT 3, L (Z 0 1 -)
void op_bit_3_l(GameBoy *gb) {
    test_bit(gb, REG(L), 3); 
}

// 0xCB5E: BIT 3, (HL) (Z 0 1 -)
void op_bit_3_hlp(GameBoy *gb) {
    test_bit(gb, READ8(REG(HL)), 3); 
    TICK(2);
}

// 0xCB5F: BIT 3, A (Z 0 1 -)
void op_bit_3_a(GameBoy *gb) {
    test_bit(gb, REG(A), 3); 
}

// 0xCB60: BIT 4, B (Z 0 1 -)
void op_bit_4_b(GameBoy *gb) {
    test_bit(gb, REG(B), 4); 
}

// 0xCB61: BIT 4, C (Z 0 1 -)
void op_bit_4_c(GameBoy *gb) {
    test_bit(gb, REG(C), 4); 
}

// 0xCB62: BIT 4, D (Z 0 1 -)
void op_bit_4_d(GameBoy *gb) {
    test_bit(gb, REG(D), 4); 
}

// 0xCB63: BIT 4, E (Z 0 1 -)
void op_bit_4_e(GameBoy *gb) {
    test_bit(gb, REG(E), 4); 
}

// 0xCB64: BIT 4, H (Z 0 1 -)
void op_bit_4_h(GameBoy *gb) {
    test_bit(gb, REG(H), 4); 
}

// 0xCB65: BIT 4, L (Z 0 1 -)
void op_bit_4_l(GameBoy *gb) {
    test_bit(gb, REG(L), 4); 
}

// 0xCB66: BIT 4, (HL) (Z 0 1 -)
void op_bit_4_hlp(GameBoy *gb) {
    test_bit(gb, READ8(REG(HL)), 4); 
    TICK(2);
}

// 0xCB67: BIT 4, A (Z 0 1 -)
void op_bit_4_a(GameBoy *gb) {
    test_bit(gb, REG(A), 4); 
}

// 0xCB68: BIT 5, B (Z 0 1 -)
void op_bit_5_b(GameBoy *gb) {
    test_bit(gb, REG(B), 5); 
}

// 0xCB69: BIT 5, C (Z 0 1 -)
void op_bit_5_c(GameBoy *gb) {
    test_bit(gb, REG(C), 5); 
}

// 0xCB6A: BIT 5, D (Z 0 1 -)
void op_bit_5_d(GameBoy *gb) {
    test_bit(gb, REG(D), 5); 
}

// 0xCB6B: BIT 5, E (Z 0 1 -)
void op_bit_5_e(GameBoy *gb) {
    test_bit(gb, REG(E), 5); 
}

// 0xCB6C: BIT 5, H (Z 0 1 -)
void op_bit_5_h(GameBoy *gb) {
    test_bit(gb, REG(H), 5); 
}

// 0xCB6D: BIT 5, L (Z 0 1 -)
void op_bit_5_l(GameBoy *gb) {
    test_bit(gb, REG(L), 5); 
}

// 0xCB6E: BIT 5, (HL) (Z 0 1 -)
void op_bit_5_hlp(GameBoy *gb) {
    test_bit(gb, READ8(REG(HL)), 5); 
    TICK(2);
}

// 0xCB6F: BIT 5, A (Z 0 1 -)
void op_bit_5_a(GameBoy *gb) {
    test_bit(gb, REG(A), 5); 
}

// 0xCB70: BIT 6, B (Z 0 1 -)
void op_bit_6_b(GameBoy *gb) {
    test_bit(gb, REG(B), 6); 
}

// 0xCB71: BIT 6, C (Z 0 1 -)
void op_bit_6_c(GameBoy *gb) {
    test_bit(gb, REG(C), 6); 
}

// 0xCB72: BIT 6, D (Z 0 1 -)
void op_bit_6_d(GameBoy *gb) {
    test_bit(gb, REG(D), 6); 
}

// 0xCB73: BIT 6, E (Z 0 1 -)
void op_bit_6_e(GameBoy *gb) {
    test_bit(gb, REG(E), 6); 
}

// 0xCB74: BIT 6, H (Z 0 1 -)
void op_bit_6_h(GameBoy *gb) {
    test_bit(gb, REG(H), 6); 
}

// 0xCB75: BIT 6, L (Z 0 1 -)
void op_bit_6_l(GameBoy *gb) {
    test_bit(gb, REG(L), 6); 
}

// 0xCB76: BIT 6, (HL) (Z 0 1 -)
void op_bit_6_hlp(GameBoy *gb) {
    test_bit(gb, READ8(REG(HL)), 6); 
    TICK(2);
}

// 0xCB77: BIT 6, A (Z 0 1 -)
void op_bit_6_a(GameBoy *gb) {
    test_bit(gb, REG(A), 6); 
}

// 0xCB78: BIT 7, B (Z 0 1 -)
void op_bit_7_b(GameBoy *gb) {
    test_bit(gb, REG(B), 7); 
}

// 0xCB79: BIT 7, C (Z 0 1 -)
void op_bit_7_c(GameBoy *gb) {
    test_bit(gb, REG(C), 7); 
}

// 0xCB7A: BIT 7, D (Z 0 1 -)
void op_bit_7_d(GameBoy *gb) {
    test_bit(gb, REG(D), 7); 
}

// 0xCB7B: BIT 7, E (Z 0 1 -)
void op_bit_7_e(GameBoy *gb) {
    test_bit(gb, REG(E), 7); 
}

// 0xCB7C: BIT 7, H (Z 0 1 -)
void op_bit_7_h(GameBoy *gb) {
    test_bit(gb, REG(H), 7); 
}

// 0xCB7D: BIT 7, L (Z 0 1 -)
void op_bit_7_l(GameBoy *gb) {
    test_bit(gb, REG(L), 7); 
}

// 0xCB7E: BIT 7, (HL) (Z 0 1 -)
void op_bit_7_hlp(GameBoy *gb) {
    test_bit(gb, READ8(REG(HL)), 7); 
}

// 0xCB7F: BIT 7, A (Z 0 1 -)
void op_bit_7_a(GameBoy *gb) {
    test_bit(gb, REG(A), 7); 
}

// 0xCB80: RES 0, B (- - - -)
void op_res_0_b(GameBoy *gb) {
    REG(B) = reset_bit(REG(B), 0);
}

// 0xCB81: RES 0, C (- - - -)
void op_res_0_c(GameBoy *gb) {
    REG(C) = reset_bit(REG(C), 0);
}

// 0xCB82: RES 0, D (- - - -)
void op_res_0_d(GameBoy *gb) {
    REG(D) = reset_bit(REG(D), 0);
}

// 0xCB83: RES 0, E (- - - -)
void op_res_0_e(GameBoy *gb) {
    REG(E) = reset_bit(REG(E), 0);
}

// 0xCB84: RES 0, H (- - - -)
void op_res_0_h(GameBoy *gb) {
    REG(H) = reset_bit(REG(H), 0);
}

// 0xCB85: RES 0, L (- - - -)
void op_res_0_l(GameBoy *gb) {
    REG(L) = reset_bit(REG(L), 0);
}

// 0xCB86: RES 0, (HL) (- - - -)
void op_res_0_hlp(GameBoy *gb) {
    WRITE8(REG(HL), reset_bit(READ8(REG(HL)), 0));
    TICK(2);
}

// 0xCB87: RES 0, A (- - - -)
void op_res_0_a(GameBoy *gb) {
    REG(A) = reset_bit(REG(A), 0);
}

// 0xCB88: RES 1, B (- - - -)
void op_res_1_b(GameBoy *gb) {
    REG(B) = reset_bit(REG(B), 1);
}

// 0xCB89: RES 1, C (- - - -)
void op_res_1_c(GameBoy *gb) {
    REG(C) = reset_bit(REG(C), 1);
}

// 0xCB8A: RES 1, D (- - - -)
void op_res_1_d(GameBoy *gb) {
    REG(D) = reset_bit(REG(D), 1);
}

// 0xCB8B: RES 1, E (- - - -)
void op_res_1_e(GameBoy *gb) {
    REG(E) = reset_bit(REG(E), 1);
}

// 0xCB8C: RES 1, H (- - - -)
void op_res_1_h(GameBoy *gb) {
    REG(H) = reset_bit(REG(H), 1);
}

// 0xCB8D: RES 1, L (- - - -)
void op_res_1_l(GameBoy *gb) {
    REG(L) = reset_bit(REG(L), 1);
}

// 0xCB8E: RES 1, (HL) (- - - -)
void op_res_1_hlp(GameBoy *gb) {
    WRITE8(REG(HL), reset_bit(READ8(REG(HL)), 1));
    TICK(2);
}

// 0xCB8F: RES 1, A (- - - -)
void op_res_1_a(GameBoy *gb) {
    REG(A) = reset_bit(REG(A), 1);
}

// 0xCB90: RES 2, B (- - - -)
void op_res_2_b(GameBoy *gb) {
    REG(B) = reset_bit(REG(B), 2);
}

// 0xCB91: RES 2, C (- - - -)
void op_res_2_c(GameBoy *gb) {
    REG(C) = reset_bit(REG(C), 2);
}

// 0xCB92: RES 2, D (- - - -)
void op_res_2_d(GameBoy *gb) {
    REG(D) = reset_bit(REG(D), 2);
}

// 0xCB93: RES 2, E (- - - -)
void op_res_2_e(GameBoy *gb) {
    REG(E) = reset_bit(REG(E), 2);
}

// 0xCB94: RES 2, H (- - - -)
void op_res_2_h(GameBoy *gb) {
    REG(H) = reset_bit(REG(H), 2);
}

// 0xCB95: RES 2, L (- - - -)
void op_res_2_l(GameBoy *gb) {
    REG(L) = reset_bit(REG(L), 2);
}

// 0xCB96: RES 2, (HL) (- - - -)
void op_res_2_hlp(GameBoy *gb) {
    WRITE8(REG(HL), reset_bit(READ8(REG(HL)), 2));
    TICK(2);
}

// 0xCB97: RES 2, A (- - - -)
void op_res_2_a(GameBoy *gb) {
    REG(A) = reset_bit(REG(A), 2);
}

// 0xCB98: RES 3, B (- - - -)
void op_res_3_b(GameBoy *gb) {
    REG(B) = reset_bit(REG(B), 3);
}

// 0xCB99: RES 3, C (- - - -)
void op_res_3_c(GameBoy *gb) {
    REG(C) = reset_bit(REG(C), 3);
}

// 0xCB9A: RES 3, D (- - - -)
void op_res_3_d(GameBoy *gb) {
    REG(D) = reset_bit(REG(D), 3);
}

// 0xCB9B: RES 3, E (- - - -)
void op_res_3_e(GameBoy *gb) {
    REG(E) = reset_bit(REG(E), 3);
}

// 0xCB9C: RES 3, H (- - - -)
void op_res_3_h(GameBoy *gb) {
    REG(H) = reset_bit(REG(H), 3);
}

// 0xCB9D: RES 3, L (- - - -)
void op_res_3_l(GameBoy *gb) {
    REG(L) = reset_bit(REG(L), 3);
}

// 0xCB9E: RES 3, (HL) (- - - -)
void op_res_3_hlp(GameBoy *gb) {
    WRITE8(REG(HL), reset_bit(READ8(REG(HL)), 3));
    TICK(2);
}

// 0xCB9F: RES 3, A (- - - -)
void op_res_3_a(GameBoy *gb) {
    REG(A) = reset_bit(REG(A), 3);
}

// 0xCBA0: RES 4, B (- - - -)
void op_res_4_b(GameBoy *gb) {
    REG(B) = reset_bit(REG(B), 4);
}

// 0xCBA1: RES 4, C (- - - -)
void op_res_4_c(GameBoy *gb) {
    REG(C) = reset_bit(REG(C), 4);
}

// 0xCBA2: RES 4, D (- - - -)
void op_res_4_d(GameBoy *gb) {
    REG(D) = reset_bit(REG(D), 4);
}

// 0xCBA3: RES 4, E (- - - -)
void op_res_4_e(GameBoy *gb) {
    REG(E) = reset_bit(REG(E), 4);
}

// 0xCBA4: RES 4, H (- - - -)
void op_res_4_h(GameBoy *gb) {
    REG(H) = reset_bit(REG(H), 4);
}

// 0xCBA5: RES 4, L (- - - -)
void op_res_4_l(GameBoy *gb) {
    REG(L) = reset_bit(REG(L), 4);
}

// 0xCBA6: RES 4, (HL) (- - - -)
void op_res_4_hlp(GameBoy *gb) {
    WRITE8(REG(HL), reset_bit(READ8(REG(HL)), 4));
    TICK(2);
}

// 0xCBA7: RES 4, A (- - - -)
void op_res_4_a(GameBoy *gb) {
    REG(A) = reset_bit(REG(A), 4);
}

// 0xCBA8: RES 5, B (- - - -)
void op_res_5_b(GameBoy *gb) {
    REG(B) = reset_bit(REG(B), 5);
}

// 0xCBA9: RES 5, C (- - - -)
void op_res_5_c(GameBoy *gb) {
    REG(C) = reset_bit(REG(C), 5);
}

// 0xCBAA: RES 5, D (- - - -)
void op_res_5_d(GameBoy *gb) {
    REG(D) = reset_bit(REG(D), 5);
}

// 0xCBAB: RES 5, E (- - - -)
void op_res_5_e(GameBoy *gb) {
    REG(E) = reset_bit(REG(E), 5);
}

// 0xCBAC: RES 5, H (- - - -)
void op_res_5_h(GameBoy *gb) {
    REG(H) = reset_bit(REG(H), 5);
}

// 0xCBAD: RES 5, L (- - - -)
void op_res_5_l(GameBoy *gb) {
    REG(L) = reset_bit(REG(L), 5);
}

// 0xCBAE: RES 5, (HL) (- - - -)
void op_res_5_hlp(GameBoy *gb) {
    WRITE8(REG(HL), reset_bit(READ8(REG(HL)), 5));
    TICK(2);
}

// 0xCBAF: RES 5, A (- - - -)
void op_res_5_a(GameBoy *gb) {
    REG(A) = reset_bit(REG(A), 5);
}

// 0xCBB0: RES 6, B (- - - -)
void op_res_6_b(GameBoy *gb) {
    REG(B) = reset_bit(REG(B), 6);
}

// 0xCBB1: RES 6, C (- - - -)
void op_res_6_c(GameBoy *gb) {
    REG(C) = reset_bit(REG(C), 6);
}

// 0xCBB2: RES 6, D (- - - -)
void op_res_6_d(GameBoy *gb) {
    REG(D) = reset_bit(REG(D), 6);
}

// 0xCBB3: RES 6, E (- - - -)
void op_res_6_e(GameBoy *gb) {
    REG(E) = reset_bit(REG(E), 6);
}

// 0xCBB4: RES 6, H (- - - -)
void op_res_6_h(GameBoy *gb) {
    REG(H) = reset_bit(REG(H), 6);
}

// 0xCBB5: RES 6, L (- - - -)
void op_res_6_l(GameBoy *gb) {
    REG(L) = reset_bit(REG(L), 6);
}

// 0xCBB6: RES 6, (HL) (- - - -)
void op_res_6_hlp(GameBoy *gb) {
    WRITE8(REG(HL), reset_bit(READ8(REG(HL)), 6));
    TICK(2);
}

// 0xCBB7: RES 6, A (- - - -)
void op_res_6_a(GameBoy *gb) {
    REG(A) = reset_bit(REG(A), 6);
}

// 0xCBB8: RES 7, B (- - - -)
void op_res_7_b(GameBoy *gb) {
    REG(B) = reset_bit(REG(B), 7);
}

// 0xCBB9: RES 7, C (- - - -)
void op_res_7_c(GameBoy *gb) {
    REG(C) = reset_bit(REG(C), 7);
}

// 0xCBBA: RES 7, D (- - - -)
void op_res_7_d(GameBoy *gb) {
    REG(D) = reset_bit(REG(D), 7);
}

// 0xCBBB: RES 7, E (- - - -)
void op_res_7_e(GameBoy *gb) {
    REG(E) = reset_bit(REG(E), 7);
}

// 0xCBBC: RES 7, H (- - - -)
void op_res_7_h(GameBoy *gb) {
    REG(H) = reset_bit(REG(H), 7);
}

// 0xCBBD: RES 7, L (- - - -)
void op_res_7_l(GameBoy *gb) {
    REG(L) = reset_bit(REG(L), 7);
}

// 0xCBBE: RES 7, (HL) (- - - -)
void op_res_7_hlp(GameBoy *gb) {
    WRITE8(REG(HL), reset_bit(READ8(REG(HL)), 7));
    TICK(2);
}

// 0xCBBF: RES 7, A (- - - -)
void op_res_7_a(GameBoy *gb) {
    REG(A) = reset_bit(REG(A), 7);
}

// 0xCBC0: SET 0, B (- - - -)
void op_set_0_b(GameBoy *gb) {
   REG(B) = set_bit(REG(B), 0);
}

// 0xCBC1: SET 0, C (- - - -)
void op_set_0_c(GameBoy *gb) {
   REG(C) = set_bit(REG(C), 0);
}

// 0xCBC2: SET 0, D (- - - -)
void op_set_0_d(GameBoy *gb) {
   REG(D) = set_bit(REG(D), 0);
}

// 0xCBC3: SET 0, E (- - - -)
void op_set_0_e(GameBoy *gb) {
   REG(E) = set_bit(REG(E), 0);
}

// 0xCBC4: SET 0, H (- - - -)
void op_set_0_h(GameBoy *gb) {
   REG(H) = set_bit(REG(H), 0);
}

// 0xCBC5: SET 0, L (- - - -)
void op_set_0_l(GameBoy *gb) {
   REG(L) = set_bit(REG(L), 0);
}

// 0xCBC6: SET 0, (HL) (- - - -)
void op_set_0_hlp(GameBoy *gb) {
    WRITE8(REG(HL), set_bit(READ8(REG(HL)), 0));
    TICK(2);
}

// 0xCBC7: SET 0, A (- - - -)
void op_set_0_a(GameBoy *gb) {
    REG(A) = set_bit(REG(A), 0);
}

// 0xCBC8: SET 1, B (- - - -)
void op_set_1_b(GameBoy *gb) {
    REG(B) = set_bit(REG(B), 1);
}

// 0xCBC9: SET 1, C (- - - -)
void op_set_1_c(GameBoy *gb) {
    REG(C) = set_bit(REG(C), 1);
}

// 0xCBCA: SET 1, D (- - - -)
void op_set_1_d(GameBoy *gb) {
    REG(D) = set_bit(REG(D), 1);
}

// 0xCBCB: SET 1, E (- - - -)
void op_set_1_e(GameBoy *gb) {
    REG(E) = set_bit(REG(E), 1);
}

// 0xCBCC: SET 1, H (- - - -)
void op_set_1_h(GameBoy *gb) {
    REG(H) = set_bit(REG(H), 1);
}

// 0xCBCD: SET 1, L (- - - -)
void op_set_1_l(GameBoy *gb) {
    REG(L) = set_bit(REG(L), 1);
}

// 0xCBCE: SET 1, (HL) (- - - -)
void op_set_1_hlp(GameBoy *gb) {
    WRITE8(REG(HL), set_bit(READ8(REG(HL)), 1));
    TICK(2);
}

// 0xCBCF: SET 1, A (- - - -)
void op_set_1_a(GameBoy *gb) {
    REG(A) = set_bit(REG(A), 1);
}

// 0xCBD0: SET 2, B (- - - -)
void op_set_2_b(GameBoy *gb) {
    REG(B) = set_bit(REG(B), 2);
}

// 0xCBD1: SET 2, C (- - - -)
void op_set_2_c(GameBoy *gb) {
    REG(C) = set_bit(REG(C), 2);
}

// 0xCBD2: SET 2, D (- - - -)
void op_set_2_d(GameBoy *gb) {
    REG(D) = set_bit(REG(D), 2);
}

// 0xCBD3: SET 2, E (- - - -)
void op_set_2_e(GameBoy *gb) {
    REG(E) = set_bit(REG(E), 2);
}

// 0xCBD4: SET 2, H (- - - -)
void op_set_2_h(GameBoy *gb) {
    REG(H) = set_bit(REG(H), 2);
}

// 0xCBD5: SET 2, L (- - - -)
void op_set_2_l(GameBoy *gb) {
    REG(L) = set_bit(REG(L), 2);
}

// 0xCBD6: SET 2, (HL) (- - - -)
void op_set_2_hlp(GameBoy *gb) {
    WRITE8(REG(HL), set_bit(READ8(REG(HL)), 2));
    TICK(2);
}

// 0xCBD7: SET 2, A (- - - -)
void op_set_2_a(GameBoy *gb) {
    REG(A) = set_bit(REG(A), 2);
}

// 0xCBD8: SET 3, B (- - - -)
void op_set_3_b(GameBoy *gb) {
    REG(B) = set_bit(REG(B), 3);
}

// 0xCBD9: SET 3, C (- - - -)
void op_set_3_c(GameBoy *gb) {
    REG(C) = set_bit(REG(C), 3);
}

// 0xCBDA: SET 3, D (- - - -)
void op_set_3_d(GameBoy *gb) {
    REG(D) = set_bit(REG(D), 3);
}

// 0xCBDB: SET 3, E (- - - -)
void op_set_3_e(GameBoy *gb) {
    REG(E) = set_bit(REG(E), 3);
}

// 0xCBDC: SET 3, H (- - - -)
void op_set_3_h(GameBoy *gb) {
    REG(H) = set_bit(REG(H), 3);
}

// 0xCBDD: SET 3, L (- - - -)
void op_set_3_l(GameBoy *gb) {
    REG(L) = set_bit(REG(L), 3);
}

// 0xCBDE: SET 3, (HL) (- - - -)
void op_set_3_hlp(GameBoy *gb) {
    WRITE8(REG(HL), set_bit(READ8(REG(HL)), 3));
    TICK(2);
}

// 0xCBDF: SET 3, A (- - - -)
void op_set_3_a(GameBoy *gb) {
    REG(A) = set_bit(REG(A), 3);
}

// 0xCBE0: SET 4, B (- - - -)
void op_set_4_b(GameBoy *gb) {
    REG(B) = set_bit(REG(B), 4);
}

// 0xCBE1: SET 4, C (- - - -)
void op_set_4_c(GameBoy *gb) {
    REG(C) = set_bit(REG(C), 4);
}

// 0xCBE2: SET 4, D (- - - -)
void op_set_4_d(GameBoy *gb) {
    REG(D) = set_bit(REG(D), 4);
}

// 0xCBE3: SET 4, E (- - - -)
void op_set_4_e(GameBoy *gb) {
    REG(E) = set_bit(REG(E), 4);
}

// 0xCBE4: SET 4, H (- - - -)
void op_set_4_h(GameBoy *gb) {
    REG(H) = set_bit(REG(H), 4);
}

// 0xCBE5: SET 4, L (- - - -)
void op_set_4_l(GameBoy *gb) {
    REG(L) = set_bit(REG(L), 4);
}

// 0xCBE6: SET 4, (HL) (- - - -)
void op_set_4_hlp(GameBoy *gb) {
    WRITE8(REG(HL), set_bit(READ8(REG(HL)), 4));
    TICK(2);
}

// 0xCBE7: SET 4, A (- - - -)
void op_set_4_a(GameBoy *gb) {
    REG(A) = set_bit(REG(A), 4);
}

// 0xCBE8: SET 5, B (- - - -)
void op_set_5_b(GameBoy *gb) {
    REG(B) = set_bit(REG(B), 5);
}

// 0xCBE9: SET 5, C (- - - -)
void op_set_5_c(GameBoy *gb) {
    REG(C) = set_bit(REG(C), 5);
}

// 0xCBEA: SET 5, D (- - - -)
void op_set_5_d(GameBoy *gb) {
    REG(D) = set_bit(REG(D), 5);
}

// 0xCBEB: SET 5, E (- - - -)
void op_set_5_e(GameBoy *gb) {
    REG(E) = set_bit(REG(E), 5);
}

// 0xCBEC: SET 5, H (- - - -)
void op_set_5_h(GameBoy *gb) {
    REG(H) = set_bit(REG(H), 5);
}

// 0xCBED: SET 5, L (- - - -)
void op_set_5_l(GameBoy *gb) {
    REG(L) = set_bit(REG(L), 5);
}

// 0xCBEE: SET 5, (HL) (- - - -)
void op_set_5_hlp(GameBoy *gb) {
    WRITE8(REG(HL), set_bit(READ8(REG(HL)), 5));
    TICK(2);
}

// 0xCBEF: SET 5, A (- - - -)
void op_set_5_a(GameBoy *gb) {
    REG(A) = set_bit(REG(A), 5);
}

// 0xCBF0: SET 6, B (- - - -)
void op_set_6_b(GameBoy *gb) {
    REG(B) = set_bit(REG(B), 6);
}

// 0xCBF1: SET 6, C (- - - -)
void op_set_6_c(GameBoy *gb) {
    REG(C) = set_bit(REG(C), 6);
}

// 0xCBF2: SET 6, D (- - - -)
void op_set_6_d(GameBoy *gb) {
    REG(D) = set_bit(REG(D), 6);
}

// 0xCBF3: SET 6, E (- - - -)
void op_set_6_e(GameBoy *gb) {
    REG(E) = set_bit(REG(E), 6);
}

// 0xCBF4: SET 6, H (- - - -)
void op_set_6_h(GameBoy *gb) {
    REG(H) = set_bit(REG(H), 6);
}

// 0xCBF5: SET 6, L (- - - -)
void op_set_6_l(GameBoy *gb) {
    REG(L) = set_bit(REG(L), 6);
}

// 0xCBF6: SET 6, (HL) (- - - -)
void op_set_6_hlp(GameBoy *gb) {
    WRITE8(REG(HL), set_bit(READ8(REG(HL)), 6));
    TICK(2);
}

// 0xCBF7: SET 6, A (- - - -)
void op_set_6_a(GameBoy *gb) {
    REG(A) = set_bit(REG(A), 6);
}

// 0xCBF8: SET 7, B (- - - -)
void op_set_7_b(GameBoy *gb) {
    REG(B) = set_bit(REG(B), 7);
}

// 0xCBF9: SET 7, C (- - - -)
void op_set_7_c(GameBoy *gb) {
    REG(C) = set_bit(REG(C), 7);
}

// 0xCBFA: SET 7, D (- - - -)
void op_set_7_d(GameBoy *gb) {
    REG(D) = set_bit(REG(D), 7);
}

// 0xCBFB: SET 7, E (- - - -)
void op_set_7_e(GameBoy *gb) {
    REG(E) = set_bit(REG(E), 7);
}

// 0xCBFC: SET 7, H (- - - -)
void op_set_7_h(GameBoy *gb) {
    REG(H) = set_bit(REG(H), 7);
}

// 0xCBFD: SET 7, L (- - - -)
void op_set_7_l(GameBoy *gb) {
    REG(L) = set_bit(REG(L), 7);
}

// 0xCBFE: SET 7, (HL) (- - - -)
void op_set_7_hlp(GameBoy *gb) {
    WRITE8(REG(HL), set_bit(READ8(REG(HL)), 7));
    TICK(2);
}

// 0xCBFF: SET 7, A (- - - -)
void op_set_7_a(GameBoy *gb) {
    REG(A) = set_bit(REG(A), 7);
}