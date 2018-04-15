#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "alu.h"
#include "instr.h"

// 0x00: NOP (- - - -)
void op_nop(gbc_system *gbc) {
    // Do nothing
}

// 0x01: LD BC, d16 (- - - -)
void op_ld_bc_d16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->BC = operand;
}

// 0x02: LD (BC), A (- - - -)
void op_ld_bcp_a(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->BC, 
        gbc->cpu->registers->A,
        1
    );
}

// 0x03: INC BC (- - - -)
void op_inc_bc(gbc_system *gbc) {
    gbc->cpu->registers->BC++;
}

// 0x04: INC B (Z 0 H -)
void op_inc_b(gbc_system *gbc) {
    gbc->cpu->registers->B = inc(
        gbc->cpu->registers->B,
        &gbc->cpu->registers->F
    );
}

// 0x05: DEC B (Z 1 H -)
void op_dec_b(gbc_system *gbc) {
    gbc->cpu->registers->B = dec(
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0x06: LD B, d8 (- - - -)
void op_ld_b_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->B = operand;
}

// 0x07: RLCA (0 0 0 C)
void op_rlca(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_left_carry(
        gbc->cpu->registers->A,
        0,
        &gbc->cpu->registers->F
    );
}

// 0x08: LD (a16), SP (- - - -)
void op_ld_a16p_sp(gbc_system *gbc, unsigned short operand) {
    write_byte(
        gbc->ram, operand, 
        gbc->cpu->registers->SP,
        1
    );
}

// 0x09: ADD HL, BC (- 0 H C)
void op_add_hl_bc(gbc_system *gbc) {
    gbc->cpu->registers->HL = add_short(
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->BC, 
        &gbc->cpu->registers->F
    );
}

// 0x0A: LD A, (BC) (- - - -)
void op_ld_a_bcp(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(
        gbc->ram, 
        gbc->cpu->registers->BC
    );
}

// 0x0B: DEC BC (- - - -)
void op_dec_bc(gbc_system *gbc) {
    gbc->cpu->registers->BC--;
}

// 0x0C: INC C (Z 0 H -)
void op_inc_c(gbc_system *gbc) {
    gbc->cpu->registers->C = inc(
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0x0D: DEC C (Z 1 H -)
void op_dec_c(gbc_system *gbc) {
    gbc->cpu->registers->C = dec(
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0x0E: LD C, d8 (- - - -)
void op_ld_c_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->C = operand;
}

// 0x0F: RRCA (0 0 0 C)
void op_rrca(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_right_carry(
        gbc->cpu->registers->A,
        0,
        &gbc->cpu->registers->F
    );
}

// 0x10: STOP (- - - -)
void op_stop(gbc_system *gbc) {
    printf("Unimplemented Instruction: STOP 0\n");
}

// 0x11: LD DE, d16 (- - - -)
void op_ld_de_d16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->DE = operand;
}

// 0x12: LD (DE), A (- - - -)
void op_ld_dep_a(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->DE, 
        gbc->cpu->registers->A,
        1
    );
}

// 0x13: INC DE (- - - -)
void op_inc_de(gbc_system *gbc) {
    gbc->cpu->registers->DE++;
}

// 0x14: INC D (Z 0 H -)
void op_inc_d(gbc_system *gbc) {
    gbc->cpu->registers->D = inc(
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0x15: DEC D (Z 1 H -)
void op_dec_d(gbc_system *gbc) {
    gbc->cpu->registers->D = dec(
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0x16: LD D, d8 (- - - -)
void op_ld_d_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->D = operand;
}

// 0x17: RLA (0 0 0 C)
void op_rla(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_left(
        gbc->cpu->registers->A, 
        0, 
        &gbc->cpu->registers->F
    );
}

// 0x18: JR r8 (- - - -)
void op_jr_r8(gbc_system *gbc, char operand) {
    gbc->cpu->registers->PC += operand;
}

// 0x19: ADD HL, DE (- 0 H C)
void op_add_hl_de(gbc_system *gbc) {
    gbc->cpu->registers->HL = add_short(
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->DE, 
        &gbc->cpu->registers->F
    );
}

// 0x1A: LD A, (DE) (- - - -)
void op_ld_a_dep(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(
        gbc->ram, 
        gbc->cpu->registers->DE
    );
}

// 0x1B: DEC DE (- - - -)
void op_dec_de(gbc_system *gbc) {
    gbc->cpu->registers->DE--;
}

// 0x1C: INC E (Z 0 H -)
void op_inc_e(gbc_system *gbc) {
    gbc->cpu->registers->E = inc(
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0x1D: DEC E (Z 1 H -)
void op_dec_e(gbc_system *gbc) {
    gbc->cpu->registers->E = dec(
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0x1E: LD E, d8 (- - - -)
void op_ld_e_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->E = operand;
}

// 0x1F: RRA (0 0 0 C)
void op_rra(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_right(
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
    set_flag(FLAG_ZERO, 0, &gbc->cpu->registers->F);
}

// 0x20: JR NZ, r8 (- - - -)
void op_jr_nz_r8(gbc_system *gbc, char operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 0) {
        gbc->cpu->registers->PC += operand;
    }
}

// 0x21: LD HL, d16 (- - - -)
void op_ld_hl_d16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->HL = operand;
}

// 0x22: LD (HL+), A (- - - -)
void op_ld_hlpp_a(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->A,
        1
    );
    gbc->cpu->registers->HL++;
}

// 0x23: INC HL (- - - -)
void op_inc_hl(gbc_system *gbc) {
    gbc->cpu->registers->HL++;
}

// 0x24: INC H (Z 0 H -)
void op_inc_h(gbc_system *gbc) {
    gbc->cpu->registers->H = inc(
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0x25: DEC H (Z 1 H -)
void op_dec_h(gbc_system *gbc) {
    gbc->cpu->registers->H = dec(
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0x26: LD H, d8 (- - - -)
void op_ld_h_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->H = operand;
}

// 0x27: DAA (Z - 0 C)
void op_daa(gbc_system *gbc) {
    gbc->cpu->registers->A = daa(
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0x28: JR Z, r8 (- - - -)
void op_jr_z_r8(gbc_system *gbc, char operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 1) {
        gbc->cpu->registers->PC += operand;
    }
}

// 0x29: ADD HL, HL (- 0 H C)
void op_add_hl_hl(gbc_system *gbc) {
    gbc->cpu->registers->HL = add_short(
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->HL, 
        &gbc->cpu->registers->F
    );
}

// 0x2A: LD A, (HL+) (- - - -)
void op_ld_a_hlpp(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(
        gbc->ram, 
        gbc->cpu->registers->HL
    );
    gbc->cpu->registers->HL++;
}

// 0x2B: DEC HL (- - - -)
void op_dec_hl(gbc_system *gbc) {
    gbc->cpu->registers->HL--;
}

// 0x2C: INC L (Z 0 H -)
void op_inc_l(gbc_system *gbc) {
    gbc->cpu->registers->L = inc(
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0x2D: DEC L (Z 1 H -)
void op_dec_l(gbc_system *gbc) {
    gbc->cpu->registers->L = dec(
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0x2E: LD L, d8 (- - - -)
void op_ld_l_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->L = operand;
}

// 0x2F: CPL (- 1 1 -)
void op_cpl(gbc_system *gbc) {
    gbc->cpu->registers->A = ~(gbc->cpu->registers->A);
    set_flag(FLAG_SUBTRACT, 1, &gbc->cpu->registers->F);
    set_flag(FLAG_HALFCARRY, 1, &gbc->cpu->registers->F);
}

// 0x30: JR NC, r8 (- - - -)
void op_jr_nc_r8(gbc_system *gbc, char operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 0) {
        gbc->cpu->registers->PC += operand;
    }
}

// 0x31: LD SP, d16 (- - - -)
void op_ld_sp_d16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->SP = operand;
}

// 0x32: LD (HL-), A (- - - -)
void op_ld_hlmp_a(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->A,
        1
    );
    gbc->cpu->registers->HL--;
}

// 0x33: INC SP (- - - -)
void op_inc_sp(gbc_system *gbc) {
    gbc->cpu->registers->SP++;
}

// 0x34: INC (HL) (Z 0 H -)
void op_inc_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL, 
        inc(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            &gbc->cpu->registers->F
        ),
        1
    );
}

// 0x35: DEC (HL) (Z 1 H -)
void op_dec_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        dec(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            &gbc->cpu->registers->F
        ),
        1
    );
}

// 0x36: LD (HL), d8 (- - - -)
void op_ld_hlp_d8(gbc_system *gbc, unsigned char operand) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        operand,
        1
    );
}

// 0x37: SCF (- 0 0 1)
void op_scf(gbc_system *gbc) {
    set_flag(FLAG_SUBTRACT, 0, &gbc->cpu->registers->F);
    set_flag(FLAG_HALFCARRY, 0, &gbc->cpu->registers->F);
    set_flag(FLAG_CARRY, 1, &gbc->cpu->registers->F);
}

// 0x38: JR C, r8 (- - - -)
void op_jr_c_r8(gbc_system *gbc, char operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 1) {
        gbc->cpu->registers->PC += operand;
    }
}

// 0x39: ADD HL, SP (- 0 H C)
void op_add_hl_sp(gbc_system *gbc) {
    gbc->cpu->registers->HL = add_short(
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->SP, 
        &gbc->cpu->registers->F
    );
}

// 0x3A: LD A, (HL-) (- - - -)
void op_ld_a_hlmp(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(gbc->ram, gbc->cpu->registers->HL);
    gbc->cpu->registers->HL--;
}

// 0x3B: DEC SP (- - - -)
void op_dec_sp(gbc_system *gbc) {
    gbc->cpu->registers->SP--;
}

// 0x3C: INC A (Z 0 H -)
void op_inc_a(gbc_system *gbc) {
    gbc->cpu->registers->A = inc(
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0x3D: DEC A (Z 1 H -)
void op_dec_a(gbc_system *gbc) {
    gbc->cpu->registers->A = dec(
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0x3E: LD A, d8 (- - - -)
void op_ld_a_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = operand;
}

// 0x3F: CCF (- 0 0 C)
void op_ccf(gbc_system *gbc) {
    set_flag(FLAG_SUBTRACT, 0, &gbc->cpu->registers->F);
    set_flag(FLAG_HALFCARRY, 0, &gbc->cpu->registers->F);
    set_flag(FLAG_CARRY, !get_flag(FLAG_CARRY, gbc->cpu->registers->F), &gbc->cpu->registers->F);
}

// 0x40: LD B, B (- - - -)
void op_ld_b_b(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->B;
}

// 0x41: LD B, C (- - - -)
void op_ld_b_c(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->C;
}

// 0x42: LD B, D (- - - -)
void op_ld_b_d(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->D;
}

// 0x43: LD B, E (- - - -)
void op_ld_b_e(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->E;
}

// 0x44: LD B, H (- - - -)
void op_ld_b_h(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->H;
}

// 0x45: LD B, L (- - - -)
void op_ld_b_l(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->L;
}

// 0x46: LD B, (HL) (- - - -)
void op_ld_b_hlp(gbc_system *gbc) {
    gbc->cpu->registers->B = read_byte(
        gbc->ram, 
        gbc->cpu->registers->HL
    );
}

// 0x47: LD B, A (- - - -)
void op_ld_b_a(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->A;
}

// 0x48: LD C, B (- - - -)
void op_ld_c_b(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->B;
}

// 0x49: LD C, C (- - - -)
void op_ld_c_c(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->C;
}

// 0x4A: LD C, D (- - - -)
void op_ld_c_d(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->D;
}

// 0x4B: LD C, E (- - - -)
void op_ld_c_e(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->E;
}

// 0x4C: LD C, H (- - - -)
void op_ld_c_h(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->H;
}

// 0x4D: LD C, L (- - - -)
void op_ld_c_l(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->L;
}

// 0x4E: LD C, (HL) (- - - -)
void op_ld_c_hlp(gbc_system *gbc) {
    gbc->cpu->registers->C = read_byte(
        gbc->ram, 
        gbc->cpu->registers->HL
    );
}

// 0x4F: LD C, A (- - - -)
void op_ld_c_a(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->A;
}

// 0x50: LD D, B (- - - -)
void op_ld_d_b(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->B;
}

// 0x51: LD D, C (- - - -)
void op_ld_d_c(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->C;
}

// 0x52: LD D, D (- - - -)
void op_ld_d_d(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->D;
}

// 0x53: LD D, E (- - - -)
void op_ld_d_e(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->E;
}

// 0x54: LD D, H (- - - -)
void op_ld_d_h(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->H;
}

// 0x55: LD D, L (- - - -)
void op_ld_d_l(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->L;
}

// 0x56: LD D, (HL) (- - - -)
void op_ld_d_hlp(gbc_system *gbc) {
    gbc->cpu->registers->D = read_byte(
        gbc->ram, 
        gbc->cpu->registers->HL
    );
}

// 0x57: LD D, A (- - - -)
void op_ld_d_a(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->A;
}

// 0x58: LD E, B (- - - -)
void op_ld_e_b(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->B;
}

// 0x59: LD E, C (- - - -)
void op_ld_e_c(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->C;
}

// 0x5A: LD E, D (- - - -)
void op_ld_e_d(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->D;
}

// 0x5B: LD E, E (- - - -)
void op_ld_e_e(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->E;
}

// 0x5C: LD E, H (- - - -)
void op_ld_e_h(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->H;
}

// 0x5D: LD E, L (- - - -)
void op_ld_e_l(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->L;
}

// 0x5E: LD E, (HL) (- - - -)
void op_ld_e_hlp(gbc_system *gbc) {
    gbc->cpu->registers->E = read_byte(
        gbc->ram, 
        gbc->cpu->registers->HL
    );
}

// 0x5F: LD E, A (- - - -)
void op_ld_e_a(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->A;
}

// 0x60: LD H, B (- - - -)
void op_ld_h_b(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->B;
}

// 0x61: LD H, C (- - - -)
void op_ld_h_c(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->C;
}

// 0x62: LD H, D (- - - -)
void op_ld_h_d(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->D;
}

// 0x63: LD H, E (- - - -)
void op_ld_h_e(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->E;
}

// 0x64: LD H, H (- - - -)
void op_ld_h_h(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->H;
}

// 0x65: LD H, L (- - - -)
void op_ld_h_l(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->L;
}

// 0x66: LD H, (HL) (- - - -)
void op_ld_h_hlp(gbc_system *gbc) {
    gbc->cpu->registers->H = read_byte(
        gbc->ram, 
        gbc->cpu->registers->HL
    );
}

// 0x67: LD H, A (- - - -)
void op_ld_h_a(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->A;
}

// 0x68: LD L, B (- - - -)
void op_ld_l_b(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->B;
}

// 0x69: LD L, C (- - - -)
void op_ld_l_c(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->C;
}

// 0x6A: LD L, D (- - - -)
void op_ld_l_d(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->D;
}

// 0x6B: LD L, E (- - - -)
void op_ld_l_e(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->E;
}

// 0x6C: LD L, H (- - - -)
void op_ld_l_h(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->H;
}

// 0x6D: LD L, L (- - - -)
void op_ld_l_l(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->L;
}

// 0x6E: LD L, (HL) (- - - -)
void op_ld_l_hlp(gbc_system *gbc) {
    gbc->cpu->registers->L = read_byte(
        gbc->ram, 
        gbc->cpu->registers->HL
    );
}

// 0x6F: LD L, A (- - - -)
void op_ld_l_a(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->A;
}

// 0x70: LD (HL), B (- - - -)
void op_ld_hlp_b(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->B,
        1
    );
}

// 0x71: LD (HL), C (- - - -)
void op_ld_hlp_c(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->C,
        1
    );
}

// 0x72: LD (HL), D (- - - -)
void op_ld_hlp_d(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->D,
        1
    );
}

// 0x73: LD (HL), E (- - - -)
void op_ld_hlp_e(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->E,
        1
    );
}

// 0x74: LD (HL), H (- - - -)
void op_ld_hlp_h(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->H,
        1
    );
}

// 0x75: LD (HL), L (- - - -)
void op_ld_hlp_l(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->L,
        1
    );
}

// 0x76: HALT (- - - -)
void op_halt(gbc_system *gbc) {
    gbc->cpu->is_halted = 1;
}

// 0x77: LD (HL), A (- - - -)
void op_ld_hlp_a(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        gbc->cpu->registers->HL, 
        gbc->cpu->registers->A,
        1
    );
}

// 0x78: LD A, B (- - - -)
void op_ld_a_b(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->B;
}

// 0x79: LD A, C (- - - -)
void op_ld_a_c(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->C;
}

// 0x7A: LD A, D (- - - -)
void op_ld_a_d(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->D;
}

// 0x7B: LD A, E (- - - -)
void op_ld_a_e(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->E;
}

// 0x7C: LD A, H (- - - -)
void op_ld_a_h(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->H;
}

// 0x7D: LD A, L (- - - -)
void op_ld_a_l(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->L;
}

// 0x7E: LD A, (HL) (- - - -)
void op_ld_a_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(
        gbc->ram, 
        gbc->cpu->registers->HL
    );
}

// 0x7F: LD A, A (- - - -)
void op_ld_a_a(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->A;
}

// 0x80: ADD A, B (Z 0 H C)
void op_add_a_b(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0x81: ADD A, C (Z 0 H C)
void op_add_a_c(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0x82: ADD A, D (Z 0 H C)
void op_add_a_d(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0x83: ADD A, E (Z 0 H C)
void op_add_a_e(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0x84: ADD A, H (Z 0 H C)
void op_add_a_h(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0x85: ADD A, L (Z 0 H C)
void op_add_a_l(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0x86: ADD A, (HL) (Z 0 H C)
void op_add_a_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(
        gbc->cpu->registers->A, 
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        &gbc->cpu->registers->F
    );
}

// 0x87: ADD A, A (Z 0 H C)
void op_add_a_a(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0x88: ADC A, B (Z 0 H C)
void op_adc_a_b(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte_carry(
        gbc->cpu->registers->A,
        gbc->cpu->registers->B,
        &gbc->cpu->registers->F
    );
}

// 0x89: ADC A, C (Z 0 H C)
void op_adc_a_c(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->C,
        &gbc->cpu->registers->F
    );
}

// 0x8A: ADC A, D (Z 0 H C)
void op_adc_a_d(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->D,
        &gbc->cpu->registers->F
    );
}

// 0x8B: ADC A, E (Z 0 H C)
void op_adc_a_e(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->E,
        &gbc->cpu->registers->F
    );
}

// 0x8C: ADC A, H (Z 0 H C)
void op_adc_a_h(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->H,
        &gbc->cpu->registers->F
    );
}

// 0x8D: ADC A, L (Z 0 H C)
void op_adc_a_l(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->L,
        &gbc->cpu->registers->F
    );
}

// 0x8E: ADC A, (HL) (Z 0 H C)
void op_adc_a_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte_carry(
        gbc->cpu->registers->A, 
        read_byte(gbc->ram, gbc->cpu->registers->HL),
        &gbc->cpu->registers->F
    );
}

// 0x8F: ADC A, A (Z 0 H C)
void op_adc_a_a(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->A,
        &gbc->cpu->registers->F
    );
}

// 0x90: SUB B (Z 1 H C)
void op_sub_b(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0x91: SUB C (Z 1 H C)
void op_sub_c(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0x92: SUB D (Z 1 H C)
void op_sub_d(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0x93: SUB E (Z 1 H C)
void op_sub_e(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0x94: SUB H (Z 1 H C)
void op_sub_h(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0x95: SUB L (Z 1 H C)
void op_sub_l(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0x96: SUB (HL) (Z 1 H C)
void op_sub_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(
        gbc->cpu->registers->A, 
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        &gbc->cpu->registers->F
    );
}

// 0x97: SUB A (Z 1 H C)
void op_sub_a(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0x98: SBC A, B (Z 1 H C)
void op_sbc_a_b(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->B,
        &gbc->cpu->registers->F
    );
}

// 0x99: SBC A, C (Z 1 H C)
void op_sbc_a_c(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->C,
        &gbc->cpu->registers->F
    );
}

// 0x9A: SBC A, D (Z 1 H C)
void op_sbc_a_d(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->D,
        &gbc->cpu->registers->F
    );
}

// 0x9B: SBC A, E (Z 1 H C)
void op_sbc_a_e(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->E,
        &gbc->cpu->registers->F
    );
}

// 0x9C: SBC A, H (Z 1 H C)
void op_sbc_a_h(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->H,
        &gbc->cpu->registers->F
    );
}

// 0x9D: SBC A, L (Z 1 H C)
void op_sbc_a_l(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->L,
        &gbc->cpu->registers->F
    );
}

// 0x9E: SBC A, (HL) (Z 1 H C)
void op_sbc_a_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte_carry(
        gbc->cpu->registers->A, 
        read_byte(gbc->ram, gbc->cpu->registers->HL),
        &gbc->cpu->registers->F
    );
}

// 0x9F: SBC A, A (Z 1 H C)
void op_sbc_a_a(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte_carry(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->A,
        &gbc->cpu->registers->F
    );
}

// 0xA0: AND B (Z 0 1 0)
void op_and_b(gbc_system *gbc) {
    gbc->cpu->registers->A = and(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0xA1: AND C (Z 0 1 0)
void op_and_c(gbc_system *gbc) {
    gbc->cpu->registers->A = and(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0xA2: AND D (Z 0 1 0)
void op_and_d(gbc_system *gbc) {
    gbc->cpu->registers->A = and(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0xA3: AND E (Z 0 1 0)
void op_and_e(gbc_system *gbc) {
    gbc->cpu->registers->A = and(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0xA4: AND H (Z 0 1 0)
void op_and_h(gbc_system *gbc) {
    gbc->cpu->registers->A = and(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0xA5: AND L (Z 0 1 0)
void op_and_l(gbc_system *gbc) {
    gbc->cpu->registers->A = and(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0xA6: AND (HL) (Z 0 1 0)
void op_and_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = and(
        gbc->cpu->registers->A, 
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        &gbc->cpu->registers->F
    );
}

// 0xA7: AND A (Z 0 1 0)
void op_and_a(gbc_system *gbc) {
    gbc->cpu->registers->A = and(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->A,
        &gbc->cpu->registers->F
    );
}

// 0xA8: XOR B (Z 0 0 0)
void op_xor_b(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(
        gbc->cpu->registers->B, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xA9: XOR C (Z 0 0 0)
void op_xor_c(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(
        gbc->cpu->registers->C, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xAA: XOR D (Z 0 0 0)
void op_xor_d(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(
        gbc->cpu->registers->D, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xAB: XOR E (Z 0 0 0)
void op_xor_e(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(
        gbc->cpu->registers->E, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xAC: XOR H (Z 0 0 0)
void op_xor_h(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(
        gbc->cpu->registers->H, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xAD: XOR L (Z 0 0 0)
void op_xor_l(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(
        gbc->cpu->registers->L, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xAE: XOR (HL) (Z 0 0 0)
void op_xor_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(
        read_byte(gbc->ram, gbc->cpu->registers->HL),
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xAF: XOR A (Z 0 0 0)
void op_xor_a(gbc_system *gbc) { 
    gbc->cpu->registers->A = xor(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xB0: OR B (Z 0 0 0)
void op_or_b(gbc_system *gbc) {
    gbc->cpu->registers->A = or(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0xB1: OR C (Z 0 0 0)
void op_or_c(gbc_system *gbc) {
    gbc->cpu->registers->A = or(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0xB2: OR D (Z 0 0 0)
void op_or_d(gbc_system *gbc) {
    gbc->cpu->registers->A = or(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0xB3: OR E (Z 0 0 0)
void op_or_e(gbc_system *gbc) {
    gbc->cpu->registers->A = or(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0xB4: OR H (Z 0 0 0)
void op_or_h(gbc_system *gbc) {
    gbc->cpu->registers->A = or(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0xB5: OR L (Z 0 0 0)
void op_or_l(gbc_system *gbc) {
    gbc->cpu->registers->A = or(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0xB6: OR (HL) (Z 0 0 0)
void op_or_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = or(
        gbc->cpu->registers->A, 
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        &gbc->cpu->registers->F
    );
}

// 0xB7: OR A (Z 0 0 0)
void op_or_a(gbc_system *gbc) {
    gbc->cpu->registers->A = or(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xB8: CP B (Z 1 H C)
void op_cp_b(gbc_system *gbc) {
    sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0xB9: CP C (Z 1 H C)
void op_cp_c(gbc_system *gbc) {
    sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0xBA: CP D (Z 1 H C)
void op_cp_d(gbc_system *gbc) {
    sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0xBB: CP E (Z 1 H C)
void op_cp_e(gbc_system *gbc) {
    sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0xBC: CP H (Z 1 H C)
void op_cp_h(gbc_system *gbc) {
    sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0xBD: CP L (Z 1 H C)
void op_cp_l(gbc_system *gbc) {
    sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0xBE: CP (HL) (Z 1 H C)
void op_cp_hlp(gbc_system *gbc) {
    sub_byte(
        gbc->cpu->registers->A,
        read_byte(gbc->ram, gbc->cpu->registers->HL),
        &gbc->cpu->registers->F
    );
}

// 0xBF: CP A (Z 1 H C)
void op_cp_a(gbc_system *gbc) {
    sub_byte(
        gbc->cpu->registers->A, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xC0: RET NZ (- - - -)
void op_ret_nz(gbc_system *gbc) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 0) {
        op_ret(gbc); 
    } 
}

// 0xC1: POP BC (- - - -)
void op_pop_bc(gbc_system *gbc) {
    gbc->cpu->registers->BC = stack_pop_short(
        gbc->ram, 
        &gbc->cpu->registers->SP
    );
}

// 0xC2: JP NZ, a16 (- - - -)
void op_jp_nz_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 0) {
        gbc->cpu->registers->PC = operand; 
    } 
}

// 0xC3: JP a16 (- - - -)
void op_jp_a16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->PC = operand;
}

// 0xC4: CALL NZ, a16 (- - - -)
void op_call_nz_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 0) {
        op_call_a16(gbc, operand); 
    } 
}

// 0xC5: PUSH BC (- - - -)
void op_push_bc(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->BC
    );
}

// 0xC6: ADD A, d8 (Z 0 H C)
void op_add_a_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = add_byte(
        gbc->cpu->registers->A, 
        operand, 
        &gbc->cpu->registers->F
    );
}

// 0xC7: RST 00H (- - - -)
void op_rst_00h(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->PC
    );
    gbc->cpu->registers->PC = 0x0;
}

// 0xC8: RET Z (- - - -)
void op_ret_z(gbc_system *gbc) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 1) {
        op_ret(gbc); 
    } 
}

// 0xC9: RET (- - - -)
void op_ret(gbc_system *gbc) {
    gbc->cpu->registers->PC = stack_pop_short(
        gbc->ram, 
        &gbc->cpu->registers->SP
    );
    gbc->cpu->is_interrupted = 0;
}

// 0xCA: JP Z, a16 (- - - -)
void op_jp_z_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 1) {
        gbc->cpu->registers->PC = operand; 
    } 
}

// 0xCB: PREFIX CB (- - - -)
void op_prefix_cb(gbc_system *gbc) {
    // Do nothing, placeholder function
}

// 0xCC: CALL Z, a16 (- - - -)
void op_call_z_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 1) {
        op_call_a16(gbc, operand); 
    } 
}

// 0xCD: CALL a16 (- - - -)
void op_call_a16(gbc_system *gbc, unsigned short operand) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP,
        gbc->cpu->registers->PC
    );
    gbc->cpu->registers->PC = operand;
}

// 0xCE: ADC A, d8 (Z 0 H C)
void op_adc_a_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = add_byte_carry(
        gbc->cpu->registers->A,
        operand,
        &gbc->cpu->registers->F
    );
}

// 0xCF: RST 08H (- - - -)
void op_rst_08h(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->PC
    );
    gbc->cpu->registers->PC = 0x8;
}

// 0xD0: RET NC (- - - -)
void op_ret_nc(gbc_system *gbc) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 0) {
        op_ret(gbc); 
    }
}

// 0xD1: POP DE (- - - -)
void op_pop_de(gbc_system *gbc) {
    gbc->cpu->registers->DE = stack_pop_short(
        gbc->ram, 
        &gbc->cpu->registers->SP
    );
}

// 0xD2: JP NC, a16 (- - - -)
void op_jp_nc_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 0) {
        gbc->cpu->registers->PC = operand; 
    }
}

// 0xD4: CALL NC, a16 (- - - -)
void op_call_nc_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 0) {
        op_call_a16(gbc, operand); 
    }
}

// 0xD5: PUSH DE (- - - -)
void op_push_de(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->DE
    );
}

// 0xD6: SUB d8 (Z 1 H C)
void op_sub_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = sub_byte(
        gbc->cpu->registers->A, 
        operand, 
        &gbc->cpu->registers->F
    );
}

// 0xD7: RST 10H (- - - -)
void op_rst_10h(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->PC
    );
    gbc->cpu->registers->PC = 0x10;
}

// 0xD8: RET C (- - - -)
void op_ret_c(gbc_system *gbc) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 1) {
        op_ret(gbc); 
    }
}

// 0xD9: RETI (- - - -)
void op_reti(gbc_system *gbc) {
    op_ret(gbc); 
    op_ei(gbc);
}

// 0xDA: JP C, a16 (- - - -)
void op_jp_c_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 1) {
        gbc->cpu->registers->PC = operand; 
    }
}

// 0xDC: CALL C, a16 (- - - -)
void op_call_c_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 1) {
        op_call_a16(gbc, operand); 
    }
}

// 0xDE: SBC A, d8 (Z 1 H C)
void op_sbc_a_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = sub_byte_carry(
        gbc->cpu->registers->A,
        operand,
        &gbc->cpu->registers->F
    );
}

// 0xDF: RST 18H (- - - -)
void op_rst_18h(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->PC
    );
    gbc->cpu->registers->PC = 0x18;
}

// 0xE0: LDH (a8), A (- - - -)
void op_ldh_a8p_a(gbc_system *gbc, unsigned char operand) {
    write_byte(
        gbc->ram, 
        0xFF00 + operand, 
        gbc->cpu->registers->A,
        1
    );
}

// 0xE1: POP HL (- - - -)
void op_pop_hl(gbc_system *gbc) {
    gbc->cpu->registers->HL = stack_pop_short(
        gbc->ram, 
        &gbc->cpu->registers->SP
    );
}

// 0xE2: LD (C), A (- - - -)
void op_ld_cp_a(gbc_system *gbc) {
    write_byte(
        gbc->ram, 
        0xFF00 + gbc->cpu->registers->C, 
        gbc->cpu->registers->A,
        1
    );
}

// 0xE5: PUSH HL (- - - -)
void op_push_hl(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->HL
    );
}

// 0xE6: AND d8 (Z 0 1 0)
void op_and_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = and(
        gbc->cpu->registers->A, 
        operand, 
        &gbc->cpu->registers->F
    );
}

// 0xE7: RST 20H (- - - -)
void op_rst_20h(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->PC
    );
    gbc->cpu->registers->PC = 0x20;
}

// 0xE8: ADD SP, r8 (0 0 H C)
void op_add_sp_r8(gbc_system *gbc, char operand) {

    set_flag(FLAG_ZERO, 0, &gbc->cpu->registers->F);
    set_flag(FLAG_SUBTRACT, 0, &gbc->cpu->registers->F);

    int result = gbc->cpu->registers->SP + operand;

    // If a half carry occurred
    if((((gbc->cpu->registers->SP & 0xF) + (operand & 0xF)) & 0x10) == 0x10) {
        set_flag(FLAG_HALFCARRY, 1, &gbc->cpu->registers->F); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, &gbc->cpu->registers->F); 
    }

    // A carry occurred
    if((result & 0xF0000) > 0) {
        set_flag(FLAG_CARRY, 1, &gbc->cpu->registers->F);
    } else {
        set_flag(FLAG_CARRY, 0, &gbc->cpu->registers->F);
    }

    gbc->cpu->registers->SP = (unsigned short) result;
}

// 0xE9: JP (HL) (- - - -)
void op_jp_hlp(gbc_system *gbc) {
    gbc->cpu->registers->PC = gbc->cpu->registers->HL;
}

// 0xEA: LD (a16), A (- - - -)
void op_ld_a16p_a(gbc_system *gbc, unsigned short operand) {
    write_byte(
        gbc->ram, 
        operand, 
        gbc->cpu->registers->A,
        1
    );
}

// 0xEE: XOR d8 (Z 0 0 0)
void op_xor_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = xor(
        operand, 
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xEF: RST 28H (- - - -)
void op_rst_28h(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->PC
    );
    gbc->cpu->registers->PC = 0x28;
}

// 0xF0: LDH A, (a8) (- - - -)
void op_ldh_a_a8p(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = read_byte(
        gbc->ram, 
        0xFF00 + operand
    );
}

// 0xF1: POP AF (Z N H C)
void op_pop_af(gbc_system *gbc) {
    gbc->cpu->registers->AF = stack_pop_short(
        gbc->ram, 
        &gbc->cpu->registers->SP
    );

    // The unused bits are cleared
    gbc->cpu->registers->F = gbc->cpu->registers->F & 0xF0;
}

// 0xF2: LD A, (C) (- - - -)
void op_ld_a_cp(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(
        gbc->ram, 
        0xFF00 + gbc->cpu->registers->C
    );
}

// 0xF3: DI (- - - -)
void op_di(gbc_system *gbc) {
   gbc->cpu->registers->IME = 0; 
}

// 0xF5: PUSH AF (- - - -)
void op_push_af(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->AF
    );
}

// 0xF6: OR d8 (Z 0 0 0)
void op_or_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = or(
        gbc->cpu->registers->A, 
        operand, 
        &gbc->cpu->registers->F
    );
}

// 0xF7: RST 30H (- - - -)
void op_rst_30h(gbc_system *gbc) {
    stack_push_short(gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->PC
    );
    gbc->cpu->registers->PC = 0x30;
}

// 0xF8: LD HL, SP+r8 (0 0 H C)
void op_ld_hl_sppr8(gbc_system *gbc, char operand) {
    gbc->cpu->registers->HL = add_short(
        gbc->cpu->registers->SP, 
        operand, 
        &gbc->cpu->registers->F
    );
    set_flag(FLAG_ZERO, 0, &gbc->cpu->registers->F);
}

// 0xF9: LD SP, HL (- - - -)
void op_ld_sp_hl(gbc_system *gbc) {
    gbc->cpu->registers->SP = gbc->cpu->registers->HL;
}

// 0xFA: LD A, (a16) (- - - -)
void op_ld_a_a16p(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->A = read_byte(
        gbc->ram, 
        operand
    );
}

// 0xFB: EI (- - - -)
void op_ei(gbc_system *gbc) {
    gbc->cpu->registers->IME = 1;
}

// 0xFE: CP d8 (Z 1 H C)
void op_cp_d8(gbc_system *gbc, unsigned char operand) {
    sub_byte(
        gbc->cpu->registers->A, 
        operand, 
        &gbc->cpu->registers->F
    );
}

// 0xFF: RST 38H (- - - -)
void op_rst_38h(gbc_system *gbc) {
    stack_push_short(
        gbc->ram, 
        &gbc->cpu->registers->SP, 
        gbc->cpu->registers->PC
    );
    gbc->cpu->registers->PC = 0x38;
}

// 0xCB00: RLC B (Z 0 0 C)
void op_rlc_b(gbc_system *gbc) {
    gbc->cpu->registers->B = rotate_left_carry(
        gbc->cpu->registers->B,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB01: RLC C (Z 0 0 C)
void op_rlc_c(gbc_system *gbc) {
    gbc->cpu->registers->C = rotate_left_carry(
        gbc->cpu->registers->C,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB02: RLC D (Z 0 0 C)
void op_rlc_d(gbc_system *gbc) {
    gbc->cpu->registers->D = rotate_left_carry(
        gbc->cpu->registers->D, 
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB03: RLC E (Z 0 0 C)
void op_rlc_e(gbc_system *gbc) {
    gbc->cpu->registers->E = rotate_left_carry(
        gbc->cpu->registers->E,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB04: RLC H (Z 0 0 C)
void op_rlc_h(gbc_system *gbc) {
    gbc->cpu->registers->H = rotate_left_carry(
        gbc->cpu->registers->H,
        1, 
        &gbc->cpu->registers->F
    );
}

// 0xCB05: RLC L (Z 0 0 C)
void op_rlc_l(gbc_system *gbc) {
    gbc->cpu->registers->L = rotate_left_carry(
        gbc->cpu->registers->L,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB06: RLC (HL) (Z 0 0 C)
void op_rlc_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        rotate_left_carry(
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            1,
            &gbc->cpu->registers->F
        ),
        1
    );
}

// 0xCB07: RLC A (Z 0 0 C)
void op_rlc_a(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_left_carry(
        gbc->cpu->registers->A,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB08: RRC B (Z 0 0 C)
void op_rrc_b(gbc_system *gbc) {
    gbc->cpu->registers->B = rotate_right_carry(
        gbc->cpu->registers->B,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB09: RRC C (Z 0 0 C)
void op_rrc_c(gbc_system *gbc) {
    gbc->cpu->registers->C = rotate_right_carry(
        gbc->cpu->registers->C, 
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB0A: RRC D (Z 0 0 C)
void op_rrc_d(gbc_system *gbc) {
    gbc->cpu->registers->D = rotate_right_carry(
        gbc->cpu->registers->D, 
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB0B: RRC E (Z 0 0 C)
void op_rrc_e(gbc_system *gbc) {
    gbc->cpu->registers->E = rotate_right_carry(
        gbc->cpu->registers->E,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB0C: RRC H (Z 0 0 C)
void op_rrc_h(gbc_system *gbc) {
    gbc->cpu->registers->H = rotate_right_carry(
        gbc->cpu->registers->H,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB0D: RRC L (Z 0 0 C)
void op_rrc_l(gbc_system *gbc) {
    gbc->cpu->registers->L = rotate_right_carry(
        gbc->cpu->registers->L,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB0E: RRC (HL) (Z 0 0 C)
void op_rrc_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        rotate_right_carry(
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            1,
            &gbc->cpu->registers->F
        ),
        1
    );
}

// 0xCB0F: RRC A (Z 0 0 C)
void op_rrc_a(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_right_carry(
        gbc->cpu->registers->A,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB10: RL B (Z 0 0 C)
void op_rl_b(gbc_system *gbc) {
    gbc->cpu->registers->B = rotate_left(
        gbc->cpu->registers->B,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB11: RL C (Z 0 0 C)
void op_rl_c(gbc_system *gbc) {
    gbc->cpu->registers->C = rotate_left(
        gbc->cpu->registers->C,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB12: RL D (Z 0 0 C)
void op_rl_d(gbc_system *gbc) {
    gbc->cpu->registers->D = rotate_left(
        gbc->cpu->registers->D, 
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB13: RL E (Z 0 0 C)
void op_rl_e(gbc_system *gbc) {
    gbc->cpu->registers->E = rotate_left(
        gbc->cpu->registers->E, 
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB14: RL H (Z 0 0 C)
void op_rl_h(gbc_system *gbc) {
    gbc->cpu->registers->H = rotate_left(
        gbc->cpu->registers->H,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB15: RL L (Z 0 0 C)
void op_rl_l(gbc_system *gbc) {
    gbc->cpu->registers->L = rotate_left(
        gbc->cpu->registers->L, 
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB16: RL (HL) (Z 0 0 C)
void op_rl_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        rotate_left(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            1,
            &gbc->cpu->registers->F
        ),
        1
    );
}

// 0xCB17: RL A (Z 0 0 C)
void op_rl_a(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_left(
        gbc->cpu->registers->A,
        1,
        &gbc->cpu->registers->F
    );
}

// 0xCB18: RR B (Z 0 0 C)
void op_rr_b(gbc_system *gbc) {
    gbc->cpu->registers->B = rotate_right(
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0xCB19: RR C (Z 0 0 C)
void op_rr_c(gbc_system *gbc) {
    gbc->cpu->registers->C = rotate_right(
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0xCB1A: RR D (Z 0 0 C)
void op_rr_d(gbc_system *gbc) {
    gbc->cpu->registers->D = rotate_right(
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0xCB1B: RR E (Z 0 0 C)
void op_rr_e(gbc_system *gbc) {
    gbc->cpu->registers->E = rotate_right(
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0xCB1C: RR H (Z 0 0 C)
void op_rr_h(gbc_system *gbc) {
    gbc->cpu->registers->H = rotate_right(
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0xCB1D: RR L (Z 0 0 C)
void op_rr_l(gbc_system *gbc) {
    gbc->cpu->registers->L = rotate_right(
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0xCB1E: RR (HL) (Z 0 0 C)
void op_rr_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        rotate_right(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            &gbc->cpu->registers->F
        ),
        1
    );
}

// 0xCB1F: RR A (Z 0 0 C)
void op_rr_a(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_right(
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xCB20: SLA B (Z 0 0 C)
void op_sla_b(gbc_system *gbc) {
    gbc->cpu->registers->B = shift_left_arith(
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0xCB21: SLA C (Z 0 0 C)
void op_sla_c(gbc_system *gbc) {
    gbc->cpu->registers->C = shift_left_arith(
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0xCB22: SLA D (Z 0 0 C)
void op_sla_d(gbc_system *gbc) {
    gbc->cpu->registers->D = shift_left_arith(
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0xCB23: SLA E (Z 0 0 C)
void op_sla_e(gbc_system *gbc) {
    gbc->cpu->registers->E = shift_left_arith(
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0xCB24: SLA H (Z 0 0 C)
void op_sla_h(gbc_system *gbc) {
    gbc->cpu->registers->H = shift_left_arith(
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0xCB25: SLA L (Z 0 0 C)
void op_sla_l(gbc_system *gbc) {
    gbc->cpu->registers->L = shift_left_arith(
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0xCB26: SLA (HL) (Z 0 0 C)
void op_sla_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        shift_left_arith(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            &gbc->cpu->registers->F
        ),
        1
    );
}

// 0xCB27: SLA A (Z 0 0 C)
void op_sla_a(gbc_system *gbc) {
    gbc->cpu->registers->A = shift_left_arith(
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xCB28: SRA B (Z 0 0 0)
void op_sra_b(gbc_system *gbc) {
    gbc->cpu->registers->B = shift_right_arith(
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0xCB29: SRA C (Z 0 0 0)
void op_sra_c(gbc_system *gbc) {
    gbc->cpu->registers->C = shift_right_arith(
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0xCB2A: SRA D (Z 0 0 0)
void op_sra_d(gbc_system *gbc) {
    gbc->cpu->registers->D = shift_right_arith(
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0xCB2B: SRA E (Z 0 0 0)
void op_sra_e(gbc_system *gbc) {
    gbc->cpu->registers->E = shift_right_arith(
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0xCB2C: SRA H (Z 0 0 0)
void op_sra_h(gbc_system *gbc) {
    gbc->cpu->registers->H = shift_right_arith(
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0xCB2D: SRA L (Z 0 0 0)
void op_sra_l(gbc_system *gbc) {
    gbc->cpu->registers->L = shift_right_arith(
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0xCB2E: SRA (HL) (Z 0 0 0)
void op_sra_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        shift_right_arith(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            &gbc->cpu->registers->F
        ),
        1
    );
}

// 0xCB2F: SRA A (Z 0 0 0)
void op_sra_a(gbc_system *gbc) {
    gbc->cpu->registers->A = shift_right_arith(
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xCB30: SWAP B (Z 0 0 0)
void op_swap_b(gbc_system *gbc) {
    gbc->cpu->registers->B = swap(
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    );
}

// 0xCB31: SWAP C (Z 0 0 0)
void op_swap_c(gbc_system *gbc) {
    gbc->cpu->registers->C = swap(
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    );
}

// 0xCB32: SWAP D (Z 0 0 0)
void op_swap_d(gbc_system *gbc) {
    gbc->cpu->registers->D = swap(
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    );
}

// 0xCB33: SWAP E (Z 0 0 0)
void op_swap_e(gbc_system *gbc) {
    gbc->cpu->registers->E = swap(
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    );
}

// 0xCB34: SWAP H (Z 0 0 0)
void op_swap_h(gbc_system *gbc) {
    gbc->cpu->registers->H = swap(
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    );
}

// 0xCB35: SWAP L (Z 0 0 0)
void op_swap_l(gbc_system *gbc) {
    gbc->cpu->registers->L = swap(
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    );
}

// 0xCB36: SWAP (HL) (Z 0 0 0)
void op_swap_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        swap(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            &gbc->cpu->registers->F
        ),
        1
    );
}

// 0xCB37: SWAP A (Z 0 0 0)
void op_swap_a(gbc_system *gbc) {
    gbc->cpu->registers->A = swap(
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    );
}

// 0xCB38: SRL B (Z 0 0 C)
void op_srl_b(gbc_system *gbc) {
    gbc->cpu->registers->B = shift_right_logic(
        gbc->cpu->registers->B, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB39: SRL C (Z 0 0 C)
void op_srl_c(gbc_system *gbc) {
    gbc->cpu->registers->C = shift_right_logic(
        gbc->cpu->registers->C, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB3A: SRL D (Z 0 0 C)
void op_srl_d(gbc_system *gbc) {
    gbc->cpu->registers->D = shift_right_logic(
        gbc->cpu->registers->D, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB3B: SRL E (Z 0 0 C)
void op_srl_e(gbc_system *gbc) {
    gbc->cpu->registers->E = shift_right_logic(
        gbc->cpu->registers->E, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB3C: SRL H (Z 0 0 C)
void op_srl_h(gbc_system *gbc) {
    gbc->cpu->registers->H = shift_right_logic(
        gbc->cpu->registers->H, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB3D: SRL L (Z 0 0 C)
void op_srl_l(gbc_system *gbc) {
    gbc->cpu->registers->L = shift_right_logic(
        gbc->cpu->registers->L, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB3E: SRL (HL) (Z 0 0 C)
void op_srl_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        shift_right_logic(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            &gbc->cpu->registers->F
        ),
        1
    ); 
}

// 0xCB3F: SRL A (Z 0 0 C)
void op_srl_a(gbc_system *gbc) {
    gbc->cpu->registers->A = shift_right_logic(
        gbc->cpu->registers->A, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB40: BIT 0, B (Z 0 1 -)
void op_bit_0_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 0, &gbc->cpu->registers->F); 
}

// 0xCB41: BIT 0, C (Z 0 1 -)
void op_bit_0_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 0, &gbc->cpu->registers->F); 
}

// 0xCB42: BIT 0, D (Z 0 1 -)
void op_bit_0_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 0, &gbc->cpu->registers->F); 
}

// 0xCB43: BIT 0, E (Z 0 1 -)
void op_bit_0_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 0, &gbc->cpu->registers->F); 
}

// 0xCB44: BIT 0, H (Z 0 1 -)
void op_bit_0_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 0, &gbc->cpu->registers->F); 
}

// 0xCB45: BIT 0, L (Z 0 1 -)
void op_bit_0_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 0, &gbc->cpu->registers->F); 
}

// 0xCB46: BIT 0, (HL) (Z 0 1 -)
void op_bit_0_hlp(gbc_system *gbc) {
    test_bit(
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        0, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB47: BIT 0, A (Z 0 1 -)
void op_bit_0_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 0, &gbc->cpu->registers->F); 
}

// 0xCB48: BIT 1, B (Z 0 1 -)
void op_bit_1_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 1, &gbc->cpu->registers->F); 
}

// 0xCB49: BIT 1, C (Z 0 1 -)
void op_bit_1_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 1, &gbc->cpu->registers->F); 
}

// 0xCB4A: BIT 1, D (Z 0 1 -)
void op_bit_1_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 1, &gbc->cpu->registers->F); 
}

// 0xCB4B: BIT 1, E (Z 0 1 -)
void op_bit_1_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 1, &gbc->cpu->registers->F); 
}

// 0xCB4C: BIT 1, H (Z 0 1 -)
void op_bit_1_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 1, &gbc->cpu->registers->F); 
}

// 0xCB4D: BIT 1, L (Z 0 1 -)
void op_bit_1_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 1, &gbc->cpu->registers->F); 
}

// 0xCB4E: BIT 1, (HL) (Z 0 1 -)
void op_bit_1_hlp(gbc_system *gbc) {
    test_bit(
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        1, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB4F: BIT 1, A (Z 0 1 -)
void op_bit_1_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 1, &gbc->cpu->registers->F); 
}

// 0xCB50: BIT 2, B (Z 0 1 -)
void op_bit_2_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 2, &gbc->cpu->registers->F); 
}

// 0xCB51: BIT 2, C (Z 0 1 -)
void op_bit_2_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 2, &gbc->cpu->registers->F); 
}

// 0xCB52: BIT 2, D (Z 0 1 -)
void op_bit_2_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 2, &gbc->cpu->registers->F); 
}

// 0xCB53: BIT 2, E (Z 0 1 -)
void op_bit_2_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 2, &gbc->cpu->registers->F); 
}

// 0xCB54: BIT 2, H (Z 0 1 -)
void op_bit_2_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 2, &gbc->cpu->registers->F); 
}

// 0xCB55: BIT 2, L (Z 0 1 -)
void op_bit_2_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 2, &gbc->cpu->registers->F); 
}

// 0xCB56: BIT 2, (HL) (Z 0 1 -)
void op_bit_2_hlp(gbc_system *gbc) {
    test_bit(
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        2, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB57: BIT 2, A (Z 0 1 -)
void op_bit_2_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 2, &gbc->cpu->registers->F); 
}

// 0xCB58: BIT 3, B (Z 0 1 -)
void op_bit_3_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 3, &gbc->cpu->registers->F); 
}

// 0xCB59: BIT 3, C (Z 0 1 -)
void op_bit_3_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 3, &gbc->cpu->registers->F); 
}

// 0xCB5A: BIT 3, D (Z 0 1 -)
void op_bit_3_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 3, &gbc->cpu->registers->F); 
}

// 0xCB5B: BIT 3, E (Z 0 1 -)
void op_bit_3_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 3, &gbc->cpu->registers->F); 
}

// 0xCB5C: BIT 3, H (Z 0 1 -)
void op_bit_3_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 3, &gbc->cpu->registers->F); 
}

// 0xCB5D: BIT 3, L (Z 0 1 -)
void op_bit_3_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 3, &gbc->cpu->registers->F); 
}

// 0xCB5E: BIT 3, (HL) (Z 0 1 -)
void op_bit_3_hlp(gbc_system *gbc) {
    test_bit(
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        3, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB5F: BIT 3, A (Z 0 1 -)
void op_bit_3_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 3, &gbc->cpu->registers->F); 
}

// 0xCB60: BIT 4, B (Z 0 1 -)
void op_bit_4_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 4, &gbc->cpu->registers->F); 
}

// 0xCB61: BIT 4, C (Z 0 1 -)
void op_bit_4_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 4, &gbc->cpu->registers->F); 
}

// 0xCB62: BIT 4, D (Z 0 1 -)
void op_bit_4_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 4, &gbc->cpu->registers->F); 
}

// 0xCB63: BIT 4, E (Z 0 1 -)
void op_bit_4_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 4, &gbc->cpu->registers->F); 
}

// 0xCB64: BIT 4, H (Z 0 1 -)
void op_bit_4_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 4, &gbc->cpu->registers->F); 
}

// 0xCB65: BIT 4, L (Z 0 1 -)
void op_bit_4_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 4, &gbc->cpu->registers->F); 
}

// 0xCB66: BIT 4, (HL) (Z 0 1 -)
void op_bit_4_hlp(gbc_system *gbc) {
    test_bit(
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        4, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB67: BIT 4, A (Z 0 1 -)
void op_bit_4_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 4, &gbc->cpu->registers->F); 
}

// 0xCB68: BIT 5, B (Z 0 1 -)
void op_bit_5_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 5, &gbc->cpu->registers->F); 
}

// 0xCB69: BIT 5, C (Z 0 1 -)
void op_bit_5_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 5, &gbc->cpu->registers->F); 
}

// 0xCB6A: BIT 5, D (Z 0 1 -)
void op_bit_5_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 5, &gbc->cpu->registers->F); 
}

// 0xCB6B: BIT 5, E (Z 0 1 -)
void op_bit_5_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 5, &gbc->cpu->registers->F); 
}

// 0xCB6C: BIT 5, H (Z 0 1 -)
void op_bit_5_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 5, &gbc->cpu->registers->F); 
}

// 0xCB6D: BIT 5, L (Z 0 1 -)
void op_bit_5_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 5, &gbc->cpu->registers->F); 
}

// 0xCB6E: BIT 5, (HL) (Z 0 1 -)
void op_bit_5_hlp(gbc_system *gbc) {
    test_bit(
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        5, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB6F: BIT 5, A (Z 0 1 -)
void op_bit_5_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 5, &gbc->cpu->registers->F); 
}

// 0xCB70: BIT 6, B (Z 0 1 -)
void op_bit_6_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 6, &gbc->cpu->registers->F); 
}

// 0xCB71: BIT 6, C (Z 0 1 -)
void op_bit_6_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 6, &gbc->cpu->registers->F); 
}

// 0xCB72: BIT 6, D (Z 0 1 -)
void op_bit_6_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 6, &gbc->cpu->registers->F); 
}

// 0xCB73: BIT 6, E (Z 0 1 -)
void op_bit_6_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 6, &gbc->cpu->registers->F); 
}

// 0xCB74: BIT 6, H (Z 0 1 -)
void op_bit_6_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 6, &gbc->cpu->registers->F); 
}

// 0xCB75: BIT 6, L (Z 0 1 -)
void op_bit_6_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 6, &gbc->cpu->registers->F); 
}

// 0xCB76: BIT 6, (HL) (Z 0 1 -)
void op_bit_6_hlp(gbc_system *gbc) {
    test_bit(
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        6, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB77: BIT 6, A (Z 0 1 -)
void op_bit_6_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 6, &gbc->cpu->registers->F); 
}

// 0xCB78: BIT 7, B (Z 0 1 -)
void op_bit_7_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 7, &gbc->cpu->registers->F); 
}

// 0xCB79: BIT 7, C (Z 0 1 -)
void op_bit_7_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 7, &gbc->cpu->registers->F); 
}

// 0xCB7A: BIT 7, D (Z 0 1 -)
void op_bit_7_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 7, &gbc->cpu->registers->F); 
}

// 0xCB7B: BIT 7, E (Z 0 1 -)
void op_bit_7_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 7, &gbc->cpu->registers->F); 
}

// 0xCB7C: BIT 7, H (Z 0 1 -)
void op_bit_7_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 7, &gbc->cpu->registers->F); 
}

// 0xCB7D: BIT 7, L (Z 0 1 -)
void op_bit_7_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 7, &gbc->cpu->registers->F); 
}

// 0xCB7E: BIT 7, (HL) (Z 0 1 -)
void op_bit_7_hlp(gbc_system *gbc) {
    test_bit(
        read_byte(gbc->ram, gbc->cpu->registers->HL), 
        7, 
        &gbc->cpu->registers->F
    ); 
}

// 0xCB7F: BIT 7, A (Z 0 1 -)
void op_bit_7_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 7, &gbc->cpu->registers->F); 
}

// 0xCB80: RES 0, B (- - - -)
void op_res_0_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 0);
}

// 0xCB81: RES 0, C (- - - -)
void op_res_0_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 0);
}

// 0xCB82: RES 0, D (- - - -)
void op_res_0_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 0);
}

// 0xCB83: RES 0, E (- - - -)
void op_res_0_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 0);
}

// 0xCB84: RES 0, H (- - - -)
void op_res_0_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 0);
}

// 0xCB85: RES 0, L (- - - -)
void op_res_0_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 0);
}

// 0xCB86: RES 0, (HL) (- - - -)
void op_res_0_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        reset_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            0
        ),
        1
    );
}

// 0xCB87: RES 0, A (- - - -)
void op_res_0_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 0);
}

// 0xCB88: RES 1, B (- - - -)
void op_res_1_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 1);
}

// 0xCB89: RES 1, C (- - - -)
void op_res_1_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 1);
}

// 0xCB8A: RES 1, D (- - - -)
void op_res_1_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 1);
}

// 0xCB8B: RES 1, E (- - - -)
void op_res_1_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 1);
}

// 0xCB8C: RES 1, H (- - - -)
void op_res_1_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 1);
}

// 0xCB8D: RES 1, L (- - - -)
void op_res_1_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 1);
}

// 0xCB8E: RES 1, (HL) (- - - -)
void op_res_1_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        reset_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            1
        ),
        1
    );
}

// 0xCB8F: RES 1, A (- - - -)
void op_res_1_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 1);
}

// 0xCB90: RES 2, B (- - - -)
void op_res_2_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 2);
}

// 0xCB91: RES 2, C (- - - -)
void op_res_2_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 2);
}

// 0xCB92: RES 2, D (- - - -)
void op_res_2_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 2);
}

// 0xCB93: RES 2, E (- - - -)
void op_res_2_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 2);
}

// 0xCB94: RES 2, H (- - - -)
void op_res_2_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 2);
}

// 0xCB95: RES 2, L (- - - -)
void op_res_2_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 2);
}

// 0xCB96: RES 2, (HL) (- - - -)
void op_res_2_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        reset_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            2
        ),
        1
    );
}

// 0xCB97: RES 2, A (- - - -)
void op_res_2_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 2);
}

// 0xCB98: RES 3, B (- - - -)
void op_res_3_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 3);
}

// 0xCB99: RES 3, C (- - - -)
void op_res_3_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 3);
}

// 0xCB9A: RES 3, D (- - - -)
void op_res_3_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 3);
}

// 0xCB9B: RES 3, E (- - - -)
void op_res_3_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 3);
}

// 0xCB9C: RES 3, H (- - - -)
void op_res_3_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 3);
}

// 0xCB9D: RES 3, L (- - - -)
void op_res_3_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 3);
}

// 0xCB9E: RES 3, (HL) (- - - -)
void op_res_3_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        reset_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            3
        ),
        1
    );
}

// 0xCB9F: RES 3, A (- - - -)
void op_res_3_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 3);
}

// 0xCBA0: RES 4, B (- - - -)
void op_res_4_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 4);
}

// 0xCBA1: RES 4, C (- - - -)
void op_res_4_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 4);
}

// 0xCBA2: RES 4, D (- - - -)
void op_res_4_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 4);
}

// 0xCBA3: RES 4, E (- - - -)
void op_res_4_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 4);
}

// 0xCBA4: RES 4, H (- - - -)
void op_res_4_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 4);
}

// 0xCBA5: RES 4, L (- - - -)
void op_res_4_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 4);
}

// 0xCBA6: RES 4, (HL) (- - - -)
void op_res_4_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        reset_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            4
        ),
        1
    );
}

// 0xCBA7: RES 4, A (- - - -)
void op_res_4_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 4);
}

// 0xCBA8: RES 5, B (- - - -)
void op_res_5_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 5);
}

// 0xCBA9: RES 5, C (- - - -)
void op_res_5_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 5);
}

// 0xCBAA: RES 5, D (- - - -)
void op_res_5_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 5);
}

// 0xCBAB: RES 5, E (- - - -)
void op_res_5_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 5);
}

// 0xCBAC: RES 5, H (- - - -)
void op_res_5_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 5);
}

// 0xCBAD: RES 5, L (- - - -)
void op_res_5_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 5);
}

// 0xCBAE: RES 5, (HL) (- - - -)
void op_res_5_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        reset_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            5
        ),
        1
    );
}

// 0xCBAF: RES 5, A (- - - -)
void op_res_5_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 5);
}

// 0xCBB0: RES 6, B (- - - -)
void op_res_6_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 6);
}

// 0xCBB1: RES 6, C (- - - -)
void op_res_6_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 6);
}

// 0xCBB2: RES 6, D (- - - -)
void op_res_6_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 6);
}

// 0xCBB3: RES 6, E (- - - -)
void op_res_6_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 6);
}

// 0xCBB4: RES 6, H (- - - -)
void op_res_6_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 6);
}

// 0xCBB5: RES 6, L (- - - -)
void op_res_6_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 6);
}

// 0xCBB6: RES 6, (HL) (- - - -)
void op_res_6_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        reset_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            6
        ),
        1
    );
}

// 0xCBB7: RES 6, A (- - - -)
void op_res_6_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 6);
}

// 0xCBB8: RES 7, B (- - - -)
void op_res_7_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 7);
}

// 0xCBB9: RES 7, C (- - - -)
void op_res_7_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 7);
}

// 0xCBBA: RES 7, D (- - - -)
void op_res_7_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 7);
}

// 0xCBBB: RES 7, E (- - - -)
void op_res_7_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 7);
}

// 0xCBBC: RES 7, H (- - - -)
void op_res_7_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 7);
}

// 0xCBBD: RES 7, L (- - - -)
void op_res_7_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 7);
}

// 0xCBBE: RES 7, (HL) (- - - -)
void op_res_7_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        reset_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            7
        ),
        1
    );
}

// 0xCBBF: RES 7, A (- - - -)
void op_res_7_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 7);
}

// 0xCBC0: SET 0, B (- - - -)
void op_set_0_b(gbc_system *gbc) {
   gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 0);
}

// 0xCBC1: SET 0, C (- - - -)
void op_set_0_c(gbc_system *gbc) {
   gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 0);
}

// 0xCBC2: SET 0, D (- - - -)
void op_set_0_d(gbc_system *gbc) {
   gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 0);
}

// 0xCBC3: SET 0, E (- - - -)
void op_set_0_e(gbc_system *gbc) {
   gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 0);
}

// 0xCBC4: SET 0, H (- - - -)
void op_set_0_h(gbc_system *gbc) {
   gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 0);
}

// 0xCBC5: SET 0, L (- - - -)
void op_set_0_l(gbc_system *gbc) {
   gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 0);
}

// 0xCBC6: SET 0, (HL) (- - - -)
void op_set_0_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        set_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL), 
            0
        ),
        1
    );
}

// 0xCBC7: SET 0, A (- - - -)
void op_set_0_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 0);
}

// 0xCBC8: SET 1, B (- - - -)
void op_set_1_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 1);
}

// 0xCBC9: SET 1, C (- - - -)
void op_set_1_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 1);
}

// 0xCBCA: SET 1, D (- - - -)
void op_set_1_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 1);
}

// 0xCBCB: SET 1, E (- - - -)
void op_set_1_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 1);
}

// 0xCBCC: SET 1, H (- - - -)
void op_set_1_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 1);
}

// 0xCBCD: SET 1, L (- - - -)
void op_set_1_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 1);
}

// 0xCBCE: SET 1, (HL) (- - - -)
void op_set_1_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        set_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            1
        ),
        1
    );
}

// 0xCBCF: SET 1, A (- - - -)
void op_set_1_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 1);
}

// 0xCBD0: SET 2, B (- - - -)
void op_set_2_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 2);
}

// 0xCBD1: SET 2, C (- - - -)
void op_set_2_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 2);
}

// 0xCBD2: SET 2, D (- - - -)
void op_set_2_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 2);
}

// 0xCBD3: SET 2, E (- - - -)
void op_set_2_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 2);
}

// 0xCBD4: SET 2, H (- - - -)
void op_set_2_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 2);
}

// 0xCBD5: SET 2, L (- - - -)
void op_set_2_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 2);
}

// 0xCBD6: SET 2, (HL) (- - - -)
void op_set_2_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        set_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            2
        ),
        1
    );
}

// 0xCBD7: SET 2, A (- - - -)
void op_set_2_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 2);
}

// 0xCBD8: SET 3, B (- - - -)
void op_set_3_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 3);
}

// 0xCBD9: SET 3, C (- - - -)
void op_set_3_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 3);
}

// 0xCBDA: SET 3, D (- - - -)
void op_set_3_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 3);
}

// 0xCBDB: SET 3, E (- - - -)
void op_set_3_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 3);
}

// 0xCBDC: SET 3, H (- - - -)
void op_set_3_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 3);
}

// 0xCBDD: SET 3, L (- - - -)
void op_set_3_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 3);
}

// 0xCBDE: SET 3, (HL) (- - - -)
void op_set_3_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        set_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            3
        ),
        1
    );
}

// 0xCBDF: SET 3, A (- - - -)
void op_set_3_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 3);
}

// 0xCBE0: SET 4, B (- - - -)
void op_set_4_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 4);
}

// 0xCBE1: SET 4, C (- - - -)
void op_set_4_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 4);
}

// 0xCBE2: SET 4, D (- - - -)
void op_set_4_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 4);
}

// 0xCBE3: SET 4, E (- - - -)
void op_set_4_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 4);
}

// 0xCBE4: SET 4, H (- - - -)
void op_set_4_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 4);
}

// 0xCBE5: SET 4, L (- - - -)
void op_set_4_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 4);
}

// 0xCBE6: SET 4, (HL) (- - - -)
void op_set_4_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        set_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            4
        ),
        1
    );
}

// 0xCBE7: SET 4, A (- - - -)
void op_set_4_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 4);
}

// 0xCBE8: SET 5, B (- - - -)
void op_set_5_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 5);
}

// 0xCBE9: SET 5, C (- - - -)
void op_set_5_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 5);
}

// 0xCBEA: SET 5, D (- - - -)
void op_set_5_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 5);
}

// 0xCBEB: SET 5, E (- - - -)
void op_set_5_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 5);
}

// 0xCBEC: SET 5, H (- - - -)
void op_set_5_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 5);
}

// 0xCBED: SET 5, L (- - - -)
void op_set_5_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 5);
}

// 0xCBEE: SET 5, (HL) (- - - -)
void op_set_5_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        set_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            5
        ),
        1
    );
}

// 0xCBEF: SET 5, A (- - - -)
void op_set_5_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 5);
}

// 0xCBF0: SET 6, B (- - - -)
void op_set_6_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 6);
}

// 0xCBF1: SET 6, C (- - - -)
void op_set_6_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 6);
}

// 0xCBF2: SET 6, D (- - - -)
void op_set_6_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 6);
}

// 0xCBF3: SET 6, E (- - - -)
void op_set_6_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 6);
}

// 0xCBF4: SET 6, H (- - - -)
void op_set_6_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 6);
}

// 0xCBF5: SET 6, L (- - - -)
void op_set_6_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 6);
}

// 0xCBF6: SET 6, (HL) (- - - -)
void op_set_6_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        set_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            6
        ),
        1
    );
}

// 0xCBF7: SET 6, A (- - - -)
void op_set_6_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 6);
}

// 0xCBF8: SET 7, B (- - - -)
void op_set_7_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 7);
}

// 0xCBF9: SET 7, C (- - - -)
void op_set_7_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 7);
}

// 0xCBFA: SET 7, D (- - - -)
void op_set_7_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 7);
}

// 0xCBFB: SET 7, E (- - - -)
void op_set_7_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 7);
}

// 0xCBFC: SET 7, H (- - - -)
void op_set_7_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 7);
}

// 0xCBFD: SET 7, L (- - - -)
void op_set_7_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 7);
}

// 0xCBFE: SET 7, (HL) (- - - -)
void op_set_7_hlp(gbc_system *gbc) {
    write_byte(
        gbc->ram,
        gbc->cpu->registers->HL,
        set_bit(
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            7
        ),
        1
    );
}

// 0xCBFF: SET 7, A (- - - -)
void op_set_7_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 7);
}