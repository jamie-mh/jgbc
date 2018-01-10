#include "lxgbc.h"
#include "cpu.h"
#include "ram.h"

void init_cpu(struct gbc_system **gbc) {

    // Initialize the registers
    (*gbc)->registers = malloc(sizeof(struct gbc_registers));

    // Set all the registers to 0
    (*gbc)->registers->A = 0;
    (*gbc)->registers->B = 0;
    (*gbc)->registers->C = 0;
    (*gbc)->registers->D = 0;
    (*gbc)->registers->E = 0;
    (*gbc)->registers->H = 0;
    (*gbc)->registers->L = 0;

    // Set the pointers to their default value
    (*gbc)->registers->PC = DEFAULT_PC;
    (*gbc)->registers->SP = DEFAULT_SP;
}

/*
*   Common CPU operations
*/

unsigned char xor(unsigned char a, unsigned char b, unsigned char *flag) {
    
    unsigned char result = a ^ b;

    if(!result) {
        set_flag('Z', 1, flag);
        set_flag('N', 0, flag);
        set_flag('H', 0, flag);
        set_flag('C', 0, flag);
    }

    return result;
} 

/*
*   CPU instructions
*/

// 0x00: NOP (- - - -)
void op_nop(struct gbc_system **gbc) {
    // Do nothing
}

// 0x01: LD BC, d16 (- - - -)
void op_ld_bc_d16(struct gbc_system **gbc, unsigned short operand) {
    (*gbc)->registers->BC = operand;
}

// 0x02: LD (BC), A (- - - -)
void op_ld_bcp_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (BC), A\n");
}

// 0x03: INC BC (- - - -)
void op_inc_bc(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC BC\n");
}

// 0x04: INC B (Z 0 H -)
void op_inc_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC B\n");
}

// 0x05: DEC B (Z 1 H -)
void op_dec_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC B\n");
}

// 0x06: LD B, d8 (- - - -)
void op_ld_b_d8(struct gbc_system **gbc, unsigned char operand) {
    (*gbc)->registers->B = operand;
}

// 0x07: RLCA (0 0 0 C)
void op_rlca(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RLCA\n");
}

// 0x08: LD (a16), SP (- - - -)
void op_ld_a16p_sp(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: LD (a16), SP\n");
}

// 0x09: ADD HL, BC (- 0 H C)
void op_add_hl_bc(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD HL, BC\n");
}

// 0x0A: LD A, (BC) (- - - -)
void op_ld_a_bcp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, (BC)\n");
}

// 0x0B: DEC BC (- - - -)
void op_dec_bc(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC BC\n");
}

// 0x0C: INC C (Z 0 H -)
void op_inc_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC C\n");
}

// 0x0D: DEC C (Z 1 H -)
void op_dec_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC C\n");
}

// 0x0E: LD C, d8 (- - - -)
void op_ld_c_d8(struct gbc_system **gbc, unsigned char operand) {
    (*gbc)->registers->C = operand;
}

// 0x0F: RRCA (0 0 0 C)
void op_rrca(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RRCA\n");
}

// 0x10: STOP 0 (- - - -)
void op_stop_0(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: STOP 0\n");
}

// 0x11: LD DE, d16 (- - - -)
void op_ld_de_d16(struct gbc_system **gbc, unsigned short operand) {
    (*gbc)->registers->DE = operand;
}

// 0x12: LD (DE), A (- - - -)
void op_ld_dep_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (DE), A\n");
}

// 0x13: INC DE (- - - -)
void op_inc_de(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC DE\n");
}

// 0x14: INC D (Z 0 H -)
void op_inc_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC D\n");
}

// 0x15: DEC D (Z 1 H -)
void op_dec_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC D\n");
}

// 0x16: LD D, d8 (- - - -)
void op_ld_d_d8(struct gbc_system **gbc, unsigned char operand) {
    (*gbc)->registers->D = operand;
}

// 0x17: RLA (0 0 0 C)
void op_rla(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RLA\n");
}

// 0x18: JR r8 (- - - -)
void op_jr_r8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: JR r8\n");
}

// 0x19: ADD HL, DE (- 0 H C)
void op_add_hl_de(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD HL, DE\n");
}

// 0x1A: LD A, (DE) (- - - -)
void op_ld_a_dep(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, (DE)\n");
}

// 0x1B: DEC DE (- - - -)
void op_dec_de(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC DE\n");
}

// 0x1C: INC E (Z 0 H -)
void op_inc_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC E\n");
}

// 0x1D: DEC E (Z 1 H -)
void op_dec_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC E\n");
}

// 0x1E: LD E, d8 (- - - -)
void op_ld_e_d8(struct gbc_system **gbc, unsigned char operand) {
    (*gbc)->registers->E = operand;
}

// 0x1F: RRA (0 0 0 C)
void op_rra(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RRA\n");
}

// 0x20: JR NZ, r8 (- - - -)
void op_jr_nz_r8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: JR NZ, r8\n");
}

// 0x21: LD HL, d16 (- - - -)
void op_ld_hl_d16(struct gbc_system **gbc, unsigned short operand) {
    (*gbc)->registers->HL = operand;
}

// 0x22: LD (HL+), A (- - - -)
void op_ld_hlpp_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (HL+), A\n");
}

// 0x23: INC HL (- - - -)
void op_inc_hl(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC HL\n");
}

// 0x24: INC H (Z 0 H -)
void op_inc_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC H\n");
}

// 0x25: DEC H (Z 1 H -)
void op_dec_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC H\n");
}

// 0x26: LD H, d8 (- - - -)
void op_ld_h_d8(struct gbc_system **gbc, unsigned char operand) {
    (*gbc)->registers->H = operand;
}

// 0x27: DAA (Z - 0 C)
void op_daa(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DAA\n");
}

// 0x28: JR Z, r8 (- - - -)
void op_jr_z_r8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: JR Z, r8\n");
}

// 0x29: ADD HL, HL (- 0 H C)
void op_add_hl_hl(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD HL, HL\n");
}

// 0x2A: LD A, (HL+) (- - - -)
void op_ld_a_hlpp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, (HL+)\n");
}

// 0x2B: DEC HL (- - - -)
void op_dec_hl(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC HL\n");
}

// 0x2C: INC L (Z 0 H -)
void op_inc_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC L\n");
}

// 0x2D: DEC L (Z 1 H -)
void op_dec_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC L\n");
}

// 0x2E: LD L, d8 (- - - -)
void op_ld_l_d8(struct gbc_system **gbc, unsigned char operand) {
    (*gbc)->registers->L = operand;
}

// 0x2F: CPL (- 1 1 -)
void op_cpl(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CPL\n");
}

// 0x30: JR NC, r8 (- - - -)
void op_jr_nc_r8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: JR NC, r8\n");
}

// 0x31: LD SP, d16 (- - - -)
void op_ld_sp_d16(struct gbc_system **gbc, unsigned short operand) {
    (*gbc)->registers->SP = operand;
}

// 0x32: LD (HL-), A (- - - -)
void op_ld_hlmp_a(struct gbc_system **gbc) {
    write_byte(&(*gbc)->ram, (*gbc)->registers->HL, (*gbc)->registers->A);
    (*gbc)->registers->HL--;
}

// 0x33: INC SP (- - - -)
void op_inc_sp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC SP\n");
}

// 0x34: INC (HL) (Z 0 H -)
void op_inc_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC (HL)\n");
}

// 0x35: DEC (HL) (Z 1 H -)
void op_dec_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC (HL)\n");
}

// 0x36: LD (HL), d8 (- - - -)
void op_ld_hlp_d8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: LD (HL), d8\n");
}

// 0x37: SCF (- 0 0 1)
void op_scf(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SCF\n");
}

// 0x38: JR C, r8 (- - - -)
void op_jr_c_r8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: JR C, r8\n");
}

// 0x39: ADD HL, SP (- 0 H C)
void op_add_hl_sp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD HL, SP\n");
}

// 0x3A: LD A, (HL-) (- - - -)
void op_ld_a_hlmp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, (HL-)\n");
}

// 0x3B: DEC SP (- - - -)
void op_dec_sp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC SP\n");
}

// 0x3C: INC A (Z 0 H -)
void op_inc_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: INC A\n");
}

// 0x3D: DEC A (Z 1 H -)
void op_dec_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DEC A\n");
}

// 0x3E: LD A, d8 (- - - -)
void op_ld_a_d8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: LD A, d8\n");
}

// 0x3F: CCF (- 0 0 C)
void op_ccf(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CCF\n");
}

// 0x40: LD B, B (- - - -)
void op_ld_b_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD B, B\n");
}

// 0x41: LD B, C (- - - -)
void op_ld_b_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD B, C\n");
}

// 0x42: LD B, D (- - - -)
void op_ld_b_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD B, D\n");
}

// 0x43: LD B, E (- - - -)
void op_ld_b_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD B, E\n");
}

// 0x44: LD B, H (- - - -)
void op_ld_b_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD B, H\n");
}

// 0x45: LD B, L (- - - -)
void op_ld_b_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD B, L\n");
}

// 0x46: LD B, (HL) (- - - -)
void op_ld_b_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD B, (HL)\n");
}

// 0x47: LD B, A (- - - -)
void op_ld_b_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD B, A\n");
}

// 0x48: LD C, B (- - - -)
void op_ld_c_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD C, B\n");
}

// 0x49: LD C, C (- - - -)
void op_ld_c_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD C, C\n");
}

// 0x4A: LD C, D (- - - -)
void op_ld_c_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD C, D\n");
}

// 0x4B: LD C, E (- - - -)
void op_ld_c_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD C, E\n");
}

// 0x4C: LD C, H (- - - -)
void op_ld_c_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD C, H\n");
}

// 0x4D: LD C, L (- - - -)
void op_ld_c_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD C, L\n");
}

// 0x4E: LD C, (HL) (- - - -)
void op_ld_c_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD C, (HL)\n");
}

// 0x4F: LD C, A (- - - -)
void op_ld_c_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD C, A\n");
}

// 0x50: LD D, B (- - - -)
void op_ld_d_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD D, B\n");
}

// 0x51: LD D, C (- - - -)
void op_ld_d_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD D, C\n");
}

// 0x52: LD D, D (- - - -)
void op_ld_d_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD D, D\n");
}

// 0x53: LD D, E (- - - -)
void op_ld_d_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD D, E\n");
}

// 0x54: LD D, H (- - - -)
void op_ld_d_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD D, H\n");
}

// 0x55: LD D, L (- - - -)
void op_ld_d_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD D, L\n");
}

// 0x56: LD D, (HL) (- - - -)
void op_ld_d_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD D, (HL)\n");
}

// 0x57: LD D, A (- - - -)
void op_ld_d_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD D, A\n");
}

// 0x58: LD E, B (- - - -)
void op_ld_e_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD E, B\n");
}

// 0x59: LD E, C (- - - -)
void op_ld_e_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD E, C\n");
}

// 0x5A: LD E, D (- - - -)
void op_ld_e_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD E, D\n");
}

// 0x5B: LD E, E (- - - -)
void op_ld_e_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD E, E\n");
}

// 0x5C: LD E, H (- - - -)
void op_ld_e_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD E, H\n");
}

// 0x5D: LD E, L (- - - -)
void op_ld_e_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD E, L\n");
}

// 0x5E: LD E, (HL) (- - - -)
void op_ld_e_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD E, (HL)\n");
}

// 0x5F: LD E, A (- - - -)
void op_ld_e_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD E, A\n");
}

// 0x60: LD H, B (- - - -)
void op_ld_h_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD H, B\n");
}

// 0x61: LD H, C (- - - -)
void op_ld_h_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD H, C\n");
}

// 0x62: LD H, D (- - - -)
void op_ld_h_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD H, D\n");
}

// 0x63: LD H, E (- - - -)
void op_ld_h_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD H, E\n");
}

// 0x64: LD H, H (- - - -)
void op_ld_h_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD H, H\n");
}

// 0x65: LD H, L (- - - -)
void op_ld_h_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD H, L\n");
}

// 0x66: LD H, (HL) (- - - -)
void op_ld_h_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD H, (HL)\n");
}

// 0x67: LD H, A (- - - -)
void op_ld_h_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD H, A\n");
}

// 0x68: LD L, B (- - - -)
void op_ld_l_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD L, B\n");
}

// 0x69: LD L, C (- - - -)
void op_ld_l_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD L, C\n");
}

// 0x6A: LD L, D (- - - -)
void op_ld_l_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD L, D\n");
}

// 0x6B: LD L, E (- - - -)
void op_ld_l_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD L, E\n");
}

// 0x6C: LD L, H (- - - -)
void op_ld_l_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD L, H\n");
}

// 0x6D: LD L, L (- - - -)
void op_ld_l_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD L, L\n");
}

// 0x6E: LD L, (HL) (- - - -)
void op_ld_l_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD L, (HL)\n");
}

// 0x6F: LD L, A (- - - -)
void op_ld_l_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD L, A\n");
}

// 0x70: LD (HL), B (- - - -)
void op_ld_hlp_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (HL), B\n");
}

// 0x71: LD (HL), C (- - - -)
void op_ld_hlp_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (HL), C\n");
}

// 0x72: LD (HL), D (- - - -)
void op_ld_hlp_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (HL), D\n");
}

// 0x73: LD (HL), E (- - - -)
void op_ld_hlp_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (HL), E\n");
}

// 0x74: LD (HL), H (- - - -)
void op_ld_hlp_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (HL), H\n");
}

// 0x75: LD (HL), L (- - - -)
void op_ld_hlp_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (HL), L\n");
}

// 0x76: HALT (- - - -)
void op_halt(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: HALT\n");
}

// 0x77: LD (HL), A (- - - -)
void op_ld_hlp_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD (HL), A\n");
}

// 0x78: LD A, B (- - - -)
void op_ld_a_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, B\n");
}

// 0x79: LD A, C (- - - -)
void op_ld_a_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, C\n");
}

// 0x7A: LD A, D (- - - -)
void op_ld_a_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, D\n");
}

// 0x7B: LD A, E (- - - -)
void op_ld_a_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, E\n");
}

// 0x7C: LD A, H (- - - -)
void op_ld_a_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, H\n");
}

// 0x7D: LD A, L (- - - -)
void op_ld_a_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, L\n");
}

// 0x7E: LD A, (HL) (- - - -)
void op_ld_a_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, (HL)\n");
}

// 0x7F: LD A, A (- - - -)
void op_ld_a_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD A, A\n");
}

// 0x80: ADD A, B (Z 0 H C)
void op_add_a_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD A, B\n");
}

// 0x81: ADD A, C (Z 0 H C)
void op_add_a_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD A, C\n");
}

// 0x82: ADD A, D (Z 0 H C)
void op_add_a_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD A, D\n");
}

// 0x83: ADD A, E (Z 0 H C)
void op_add_a_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD A, E\n");
}

// 0x84: ADD A, H (Z 0 H C)
void op_add_a_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD A, H\n");
}

// 0x85: ADD A, L (Z 0 H C)
void op_add_a_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD A, L\n");
}

// 0x86: ADD A, (HL) (Z 0 H C)
void op_add_a_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD A, (HL)\n");
}

// 0x87: ADD A, A (Z 0 H C)
void op_add_a_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADD A, A\n");
}

// 0x88: ADC A, B (Z 0 H C)
void op_adc_a_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADC A, B\n");
}

// 0x89: ADC A, C (Z 0 H C)
void op_adc_a_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADC A, C\n");
}

// 0x8A: ADC A, D (Z 0 H C)
void op_adc_a_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADC A, D\n");
}

// 0x8B: ADC A, E (Z 0 H C)
void op_adc_a_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADC A, E\n");
}

// 0x8C: ADC A, H (Z 0 H C)
void op_adc_a_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADC A, H\n");
}

// 0x8D: ADC A, L (Z 0 H C)
void op_adc_a_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADC A, L\n");
}

// 0x8E: ADC A, (HL) (Z 0 H C)
void op_adc_a_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADC A, (HL)\n");
}

// 0x8F: ADC A, A (Z 0 H C)
void op_adc_a_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: ADC A, A\n");
}

// 0x90: SUB B (Z 1 H C)
void op_sub_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SUB B\n");
}

// 0x91: SUB C (Z 1 H C)
void op_sub_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SUB C\n");
}

// 0x92: SUB D (Z 1 H C)
void op_sub_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SUB D\n");
}

// 0x93: SUB E (Z 1 H C)
void op_sub_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SUB E\n");
}

// 0x94: SUB H (Z 1 H C)
void op_sub_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SUB H\n");
}

// 0x95: SUB L (Z 1 H C)
void op_sub_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SUB L\n");
}

// 0x96: SUB (HL) (Z 1 H C)
void op_sub_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SUB (HL)\n");
}

// 0x97: SUB A (Z 1 H C)
void op_sub_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SUB A\n");
}

// 0x98: SBC A, B (Z 1 H C)
void op_sbc_a_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SBC A, B\n");
}

// 0x99: SBC A, C (Z 1 H C)
void op_sbc_a_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SBC A, C\n");
}

// 0x9A: SBC A, D (Z 1 H C)
void op_sbc_a_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SBC A, D\n");
}

// 0x9B: SBC A, E (Z 1 H C)
void op_sbc_a_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SBC A, E\n");
}

// 0x9C: SBC A, H (Z 1 H C)
void op_sbc_a_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SBC A, H\n");
}

// 0x9D: SBC A, L (Z 1 H C)
void op_sbc_a_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SBC A, L\n");
}

// 0x9E: SBC A, (HL) (Z 1 H C)
void op_sbc_a_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SBC A, (HL)\n");
}

// 0x9F: SBC A, A (Z 1 H C)
void op_sbc_a_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: SBC A, A\n");
}

// 0xA0: AND B (Z 0 1 0)
void op_and_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: AND B\n");
}

// 0xA1: AND C (Z 0 1 0)
void op_and_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: AND C\n");
}

// 0xA2: AND D (Z 0 1 0)
void op_and_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: AND D\n");
}

// 0xA3: AND E (Z 0 1 0)
void op_and_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: AND E\n");
}

// 0xA4: AND H (Z 0 1 0)
void op_and_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: AND H\n");
}

// 0xA5: AND L (Z 0 1 0)
void op_and_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: AND L\n");
}

// 0xA6: AND (HL) (Z 0 1 0)
void op_and_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: AND (HL)\n");
}

// 0xA7: AND A (Z 0 1 0)
void op_and_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: AND A\n");
}

// 0xA8: XOR B (Z 0 0 0)
void op_xor_b(struct gbc_system **gbc) {
    (*gbc)->registers->A = xor((*gbc)->registers->B, (*gbc)->registers->A, &(*gbc)->registers->F);
}

// 0xA9: XOR C (Z 0 0 0)
void op_xor_c(struct gbc_system **gbc) {
    (*gbc)->registers->A = xor((*gbc)->registers->C, (*gbc)->registers->A, &(*gbc)->registers->F);
}

// 0xAA: XOR D (Z 0 0 0)
void op_xor_d(struct gbc_system **gbc) {
    (*gbc)->registers->A = xor((*gbc)->registers->D, (*gbc)->registers->A, &(*gbc)->registers->F);
}

// 0xAB: XOR E (Z 0 0 0)
void op_xor_e(struct gbc_system **gbc) {
    (*gbc)->registers->A = xor((*gbc)->registers->E, (*gbc)->registers->A, &(*gbc)->registers->F);
}

// 0xAC: XOR H (Z 0 0 0)
void op_xor_h(struct gbc_system **gbc) {
    (*gbc)->registers->A = xor((*gbc)->registers->H, (*gbc)->registers->A, &(*gbc)->registers->F);
}

// 0xAD: XOR L (Z 0 0 0)
void op_xor_l(struct gbc_system **gbc) {
    (*gbc)->registers->A = xor((*gbc)->registers->L, (*gbc)->registers->A, &(*gbc)->registers->F);
}

// 0xAE: XOR (HL) (Z 0 0 0)
void op_xor_hlp(struct gbc_system **gbc) {
    (*gbc)->registers->A = xor(read_byte(&(*gbc)->ram, (*gbc)->registers->HL),
                               (*gbc)->registers->A, 
                               &(*gbc)->registers->F);
}

// 0xAF: XOR A (Z 0 0 0)
void op_xor_a(struct gbc_system **gbc) { 
    (*gbc)->registers->A = xor((*gbc)->registers->A, (*gbc)->registers->A, &(*gbc)->registers->F);
}

// 0xB0: OR B (Z 0 0 0)
void op_or_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: OR B\n");
}

// 0xB1: OR C (Z 0 0 0)
void op_or_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: OR C\n");
}

// 0xB2: OR D (Z 0 0 0)
void op_or_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: OR D\n");
}

// 0xB3: OR E (Z 0 0 0)
void op_or_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: OR E\n");
}

// 0xB4: OR H (Z 0 0 0)
void op_or_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: OR H\n");
}

// 0xB5: OR L (Z 0 0 0)
void op_or_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: OR L\n");
}

// 0xB6: OR (HL) (Z 0 0 0)
void op_or_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: OR (HL)\n");
}

// 0xB7: OR A (Z 0 0 0)
void op_or_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: OR A\n");
}

// 0xB8: CP B (Z 1 H C)
void op_cp_b(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CP B\n");
}

// 0xB9: CP C (Z 1 H C)
void op_cp_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CP C\n");
}

// 0xBA: CP D (Z 1 H C)
void op_cp_d(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CP D\n");
}

// 0xBB: CP E (Z 1 H C)
void op_cp_e(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CP E\n");
}

// 0xBC: CP H (Z 1 H C)
void op_cp_h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CP H\n");
}

// 0xBD: CP L (Z 1 H C)
void op_cp_l(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CP L\n");
}

// 0xBE: CP (HL) (Z 1 H C)
void op_cp_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CP (HL)\n");
}

// 0xBF: CP A (Z 1 H C)
void op_cp_a(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: CP A\n");
}

// 0xC0: RET NZ (- - - -)
void op_ret_nz(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RET NZ\n");
}

// 0xC1: POP BC (- - - -)
void op_pop_bc(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: POP BC\n");
}

// 0xC2: JP NZ, a16 (- - - -)
void op_jp_nz_a16(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: JP NZ, a16\n");
}

// 0xC3: JP a16 (- - - -)
void op_jp_a16(struct gbc_system **gbc, unsigned short operand) {
    (*gbc)->registers->PC = operand;
}

// 0xC4: CALL NZ, a16 (- - - -)
void op_call_nz_a16(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: CALL NZ, a16\n");
}

// 0xC5: PUSH BC (- - - -)
void op_push_bc(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: PUSH BC\n");
}

// 0xC6: ADD A, d8 (Z 0 H C)
void op_add_a_d8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: ADD A, d8\n");
}

// 0xC7: RST 00H (- - - -)
void op_rst_00h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RST 00H\n");
}

// 0xC8: RET Z (- - - -)
void op_ret_z(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RET Z\n");
}

// 0xC9: RET (- - - -)
void op_ret(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RET\n");
}

// 0xCA: JP Z, a16 (- - - -)
void op_jp_z_a16(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: JP Z, a16\n");
}

// 0xCB: PREFIX CB (- - - -)
void op_prefix_cb(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: PREFIX CB\n");
}

// 0xCC: CALL Z, a16 (- - - -)
void op_call_z_a16(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: CALL Z, a16\n");
}

// 0xCD: CALL a16 (- - - -)
void op_call_a16(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: CALL a16\n");
}

// 0xCE: ADC A, d8 (Z 0 H C)
void op_adc_a_d8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: ADC A, d8\n");
}

// 0xCF: RST 08H (- - - -)
void op_rst_08h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RST 08H\n");
}

// 0xD0: RET NC (- - - -)
void op_ret_nc(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RET NC\n");
}

// 0xD1: POP DE (- - - -)
void op_pop_de(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: POP DE\n");
}

// 0xD2: JP NC, a16 (- - - -)
void op_jp_nc_a16(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: JP NC, a16\n");
}

// 0xD4: CALL NC, a16 (- - - -)
void op_call_nc_a16(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: CALL NC, a16\n");
}

// 0xD5: PUSH DE (- - - -)
void op_push_de(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: PUSH DE\n");
}

// 0xD6: SUB d8 (Z 1 H C)
void op_sub_d8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SUB d8\n");
}

// 0xD7: RST 10H (- - - -)
void op_rst_10h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RST 10H\n");
}

// 0xD8: RET C (- - - -)
void op_ret_c(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RET C\n");
}

// 0xD9: RETI (- - - -)
void op_reti(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RETI\n");
}

// 0xDA: JP C, a16 (- - - -)
void op_jp_c_a16(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: JP C, a16\n");
}

// 0xDC: CALL C, a16 (- - - -)
void op_call_c_a16(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: CALL C, a16\n");
}

// 0xDE: SBC A, d8 (Z 1 H C)
void op_sbc_a_d8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SBC A, d8\n");
}

// 0xDF: RST 18H (- - - -)
void op_rst_18h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RST 18H\n");
}

// 0xE0: LDH (a8), A (- - - -)
void op_ldh_a8p_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: LDH (a8), A\n");
}

// 0xE1: POP HL (- - - -)
void op_pop_hl(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: POP HL\n");
}

// 0xE2: LD (C), A (- - - -)
void op_ld_cp_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: LD (C), A\n");
}

// 0xE5: PUSH HL (- - - -)
void op_push_hl(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: PUSH HL\n");
}

// 0xE6: AND d8 (Z 0 1 0)
void op_and_d8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: AND d8\n");
}

// 0xE7: RST 20H (- - - -)
void op_rst_20h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RST 20H\n");
}

// 0xE8: ADD SP, r8 (0 0 H C)
void op_add_sp_r8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: ADD SP, r8\n");
}

// 0xE9: JP (HL) (- - - -)
void op_jp_hlp(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: JP (HL)\n");
}

// 0xEA: LD (a16), A (- - - -)
void op_ld_a16p_a(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: LD (a16), A\n");
}

// 0xEE: XOR d8 (Z 0 0 0)
void op_xor_d8(struct gbc_system **gbc, unsigned char operand) {
    (*gbc)->registers->A = xor(operand, (*gbc)->registers->A, &(*gbc)->registers->F);
}

// 0xEF: RST 28H (- - - -)
void op_rst_28h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RST 28H\n");
}

// 0xF0: LDH A, (a8) (- - - -)
void op_ldh_a_a8p(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: LDH A, (a8)\n");
}

// 0xF1: POP AF (Z N H C)
void op_pop_af(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: POP AF\n");
}

// 0xF2: LD A, (C) (- - - -)
void op_ld_a_cp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: LD A, (C)\n");
}

// 0xF3: DI (- - - -)
void op_di(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: DI\n");
}

// 0xF5: PUSH AF (- - - -)
void op_push_af(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: PUSH AF\n");
}

// 0xF6: OR d8 (Z 0 0 0)
void op_or_d8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: OR d8\n");
}

// 0xF7: RST 30H (- - - -)
void op_rst_30h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RST 30H\n");
}

// 0xF8: LD HL, SP+r8 (0 0 H C)
void op_ld_hl_sppr8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: LD HL, SP+r8\n");
}

// 0xF9: LD SP, HL (- - - -)
void op_ld_sp_hl(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: LD SP, HL\n");
}

// 0xFA: LD A, (a16) (- - - -)
void op_ld_a_a16p(struct gbc_system **gbc, unsigned short operand) {
    printf("Unimplemented Instruction: LD A, (a16)\n");
}

// 0xFB: EI (- - - -)
void op_ei(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: EI\n");
}

// 0xFE: CP d8 (Z 1 H C)
void op_cp_d8(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: CP d8\n");
}

// 0xFF: RST 38H (- - - -)
void op_rst_38h(struct gbc_system **gbc) {
    printf("Unimplemented Instruction: RST 38H\n");
}

struct gbc_instr instructions[INSTRUCTION_COUNT] = {
    {"NOP", 1, 4, 1, op_nop}, // 0x00
    {"LD BC, %04x", 3, 12, 1, op_ld_bc_d16}, // 0x01
    {"LD (BC), A", 1, 8, 1, op_ld_bcp_a}, // 0x02
    {"INC BC", 1, 8, 1, op_inc_bc}, // 0x03
    {"INC B", 1, 4, 1, op_inc_b}, // 0x04
    {"DEC B", 1, 4, 1, op_dec_b}, // 0x05
    {"LD B, %02x", 2, 8, 1, op_ld_b_d8}, // 0x06
    {"RLCA", 1, 4, 1, op_rlca}, // 0x07
    {"LD (%04x), SP", 3, 20, 1, op_ld_a16p_sp}, // 0x08
    {"ADD HL, BC", 1, 8, 1, op_add_hl_bc}, // 0x09
    {"LD A, (BC)", 1, 8, 1, op_ld_a_bcp}, // 0x0A
    {"DEC BC", 1, 8, 1, op_dec_bc}, // 0x0B
    {"INC C", 1, 4, 1, op_inc_c}, // 0x0C
    {"DEC C", 1, 4, 1, op_dec_c}, // 0x0D
    {"LD C, %02x", 2, 8, 1, op_ld_c_d8}, // 0x0E
    {"RRCA", 1, 4, 1, op_rrca}, // 0x0F
    {"STOP 0", 2, 4, 1, op_stop_0}, // 0x10
    {"LD DE, %04x", 3, 12, 1, op_ld_de_d16}, // 0x11
    {"LD (DE), A", 1, 8, 1, op_ld_dep_a}, // 0x12
    {"INC DE", 1, 8, 1, op_inc_de}, // 0x13
    {"INC D", 1, 4, 1, op_inc_d}, // 0x14
    {"DEC D", 1, 4, 1, op_dec_d}, // 0x15
    {"LD D, %02x", 2, 8, 1, op_ld_d_d8}, // 0x16
    {"RLA", 1, 4, 1, op_rla}, // 0x17
    {"JR %02x", 2, 12, 1, op_jr_r8}, // 0x18
    {"ADD HL, DE", 1, 8, 1, op_add_hl_de}, // 0x19
    {"LD A, (DE)", 1, 8, 1, op_ld_a_dep}, // 0x1A
    {"DEC DE", 1, 8, 1, op_dec_de}, // 0x1B
    {"INC E", 1, 4, 1, op_inc_e}, // 0x1C
    {"DEC E", 1, 4, 1, op_dec_e}, // 0x1D
    {"LD E, %02x", 2, 8, 1, op_ld_e_d8}, // 0x1E
    {"RRA", 1, 4, 1, op_rra}, // 0x1F
    {"JR NZ, %02x", 2, 12, 1, op_jr_nz_r8}, // 0x20
    {"LD HL, %04x", 3, 12, 1, op_ld_hl_d16}, // 0x21
    {"LD (HL+), A", 1, 8, 1, op_ld_hlp_a}, // 0x22
    {"INC HL", 1, 8, 1, op_inc_hl}, // 0x23
    {"INC H", 1, 4, 1, op_inc_h}, // 0x24
    {"DEC H", 1, 4, 1, op_dec_h}, // 0x25
    {"LD H, %02x", 2, 8, 1, op_ld_h_d8}, // 0x26
    {"DAA", 1, 4, 1, op_daa}, // 0x27
    {"JR Z, %02x", 2, 12, 1, op_jr_z_r8}, // 0x28
    {"ADD HL, HL", 1, 8, 1, op_add_hl_hl}, // 0x29
    {"LD A, (HL+)", 1, 8, 1, op_ld_a_hlp}, // 0x2A
    {"DEC HL", 1, 8, 1, op_dec_hl}, // 0x2B
    {"INC L", 1, 4, 1, op_inc_l}, // 0x2C
    {"DEC L", 1, 4, 1, op_dec_l}, // 0x2D
    {"LD L, %02x", 2, 8, 1, op_ld_l_d8}, // 0x2E
    {"CPL", 1, 4, 1, op_cpl}, // 0x2F
    {"JR NC, %02x", 2, 12, 1, op_jr_nc_r8}, // 0x30
    {"LD SP, %04x", 3, 12, 1, op_ld_sp_d16}, // 0x31
    {"LD (HL-), A", 1, 8, 1, op_ld_hlmp_a}, // 0x32
    {"INC SP", 1, 8, 1, op_inc_sp}, // 0x33
    {"INC (HL)", 1, 12, 1, op_inc_hlp}, // 0x34
    {"DEC (HL)", 1, 12, 1, op_dec_hlp}, // 0x35
    {"LD (HL), %02x", 2, 12, 1, op_ld_hlp_d8}, // 0x36
    {"SCF", 1, 4, 1, op_scf}, // 0x37
    {"JR C, %02x", 2, 12, 1, op_jr_c_r8}, // 0x38
    {"ADD HL, SP", 1, 8, 1, op_add_hl_sp}, // 0x39
    {"LD A, (HL-)", 1, 8, 1, op_ld_a_hlp}, // 0x3A
    {"DEC SP", 1, 8, 1, op_dec_sp}, // 0x3B
    {"INC A", 1, 4, 1, op_inc_a}, // 0x3C
    {"DEC A", 1, 4, 1, op_dec_a}, // 0x3D
    {"LD A, %02x", 2, 8, 1, op_ld_a_d8}, // 0x3E
    {"CCF", 1, 4, 1, op_ccf}, // 0x3F
    {"LD B, B", 1, 4, 1, op_ld_b_b}, // 0x40
    {"LD B, C", 1, 4, 1, op_ld_b_c}, // 0x41
    {"LD B, D", 1, 4, 1, op_ld_b_d}, // 0x42
    {"LD B, E", 1, 4, 1, op_ld_b_e}, // 0x43
    {"LD B, H", 1, 4, 1, op_ld_b_h}, // 0x44
    {"LD B, L", 1, 4, 1, op_ld_b_l}, // 0x45
    {"LD B, (HL)", 1, 8, 1, op_ld_b_hlp}, // 0x46
    {"LD B, A", 1, 4, 1, op_ld_b_a}, // 0x47
    {"LD C, B", 1, 4, 1, op_ld_c_b}, // 0x48
    {"LD C, C", 1, 4, 1, op_ld_c_c}, // 0x49
    {"LD C, D", 1, 4, 1, op_ld_c_d}, // 0x4A
    {"LD C, E", 1, 4, 1, op_ld_c_e}, // 0x4B
    {"LD C, H", 1, 4, 1, op_ld_c_h}, // 0x4C
    {"LD C, L", 1, 4, 1, op_ld_c_l}, // 0x4D
    {"LD C, (HL)", 1, 8, 1, op_ld_c_hlp}, // 0x4E
    {"LD C, A", 1, 4, 1, op_ld_c_a}, // 0x4F
    {"LD D, B", 1, 4, 1, op_ld_d_b}, // 0x50
    {"LD D, C", 1, 4, 1, op_ld_d_c}, // 0x51
    {"LD D, D", 1, 4, 1, op_ld_d_d}, // 0x52
    {"LD D, E", 1, 4, 1, op_ld_d_e}, // 0x53
    {"LD D, H", 1, 4, 1, op_ld_d_h}, // 0x54
    {"LD D, L", 1, 4, 1, op_ld_d_l}, // 0x55
    {"LD D, (HL)", 1, 8, 1, op_ld_d_hlp}, // 0x56
    {"LD D, A", 1, 4, 1, op_ld_d_a}, // 0x57
    {"LD E, B", 1, 4, 1, op_ld_e_b}, // 0x58
    {"LD E, C", 1, 4, 1, op_ld_e_c}, // 0x59
    {"LD E, D", 1, 4, 1, op_ld_e_d}, // 0x5A
    {"LD E, E", 1, 4, 1, op_ld_e_e}, // 0x5B
    {"LD E, H", 1, 4, 1, op_ld_e_h}, // 0x5C
    {"LD E, L", 1, 4, 1, op_ld_e_l}, // 0x5D
    {"LD E, (HL)", 1, 8, 1, op_ld_e_hlp}, // 0x5E
    {"LD E, A", 1, 4, 1, op_ld_e_a}, // 0x5F
    {"LD H, B", 1, 4, 1, op_ld_h_b}, // 0x60
    {"LD H, C", 1, 4, 1, op_ld_h_c}, // 0x61
    {"LD H, D", 1, 4, 1, op_ld_h_d}, // 0x62
    {"LD H, E", 1, 4, 1, op_ld_h_e}, // 0x63
    {"LD H, H", 1, 4, 1, op_ld_h_h}, // 0x64
    {"LD H, L", 1, 4, 1, op_ld_h_l}, // 0x65
    {"LD H, (HL)", 1, 8, 1, op_ld_h_hlp}, // 0x66
    {"LD H, A", 1, 4, 1, op_ld_h_a}, // 0x67
    {"LD L, B", 1, 4, 1, op_ld_l_b}, // 0x68
    {"LD L, C", 1, 4, 1, op_ld_l_c}, // 0x69
    {"LD L, D", 1, 4, 1, op_ld_l_d}, // 0x6A
    {"LD L, E", 1, 4, 1, op_ld_l_e}, // 0x6B
    {"LD L, H", 1, 4, 1, op_ld_l_h}, // 0x6C
    {"LD L, L", 1, 4, 1, op_ld_l_l}, // 0x6D
    {"LD L, (HL)", 1, 8, 1, op_ld_l_hlp}, // 0x6E
    {"LD L, A", 1, 4, 1, op_ld_l_a}, // 0x6F
    {"LD (HL), B", 1, 8, 1, op_ld_hlp_b}, // 0x70
    {"LD (HL), C", 1, 8, 1, op_ld_hlp_c}, // 0x71
    {"LD (HL), D", 1, 8, 1, op_ld_hlp_d}, // 0x72
    {"LD (HL), E", 1, 8, 1, op_ld_hlp_e}, // 0x73
    {"LD (HL), H", 1, 8, 1, op_ld_hlp_h}, // 0x74
    {"LD (HL), L", 1, 8, 1, op_ld_hlp_l}, // 0x75
    {"HALT", 1, 4, 1, op_halt}, // 0x76
    {"LD (HL), A", 1, 8, 1, op_ld_hlp_a}, // 0x77
    {"LD A, B", 1, 4, 1, op_ld_a_b}, // 0x78
    {"LD A, C", 1, 4, 1, op_ld_a_c}, // 0x79
    {"LD A, D", 1, 4, 1, op_ld_a_d}, // 0x7A
    {"LD A, E", 1, 4, 1, op_ld_a_e}, // 0x7B
    {"LD A, H", 1, 4, 1, op_ld_a_h}, // 0x7C
    {"LD A, L", 1, 4, 1, op_ld_a_l}, // 0x7D
    {"LD A, (HL)", 1, 8, 1, op_ld_a_hlp}, // 0x7E
    {"LD A, A", 1, 4, 1, op_ld_a_a}, // 0x7F
    {"ADD A, B", 1, 4, 1, op_add_a_b}, // 0x80
    {"ADD A, C", 1, 4, 1, op_add_a_c}, // 0x81
    {"ADD A, D", 1, 4, 1, op_add_a_d}, // 0x82
    {"ADD A, E", 1, 4, 1, op_add_a_e}, // 0x83
    {"ADD A, H", 1, 4, 1, op_add_a_h}, // 0x84
    {"ADD A, L", 1, 4, 1, op_add_a_l}, // 0x85
    {"ADD A, (HL)", 1, 8, 1, op_add_a_hlp}, // 0x86
    {"ADD A, A", 1, 4, 1, op_add_a_a}, // 0x87
    {"ADC A, B", 1, 4, 1, op_adc_a_b}, // 0x88
    {"ADC A, C", 1, 4, 1, op_adc_a_c}, // 0x89
    {"ADC A, D", 1, 4, 1, op_adc_a_d}, // 0x8A
    {"ADC A, E", 1, 4, 1, op_adc_a_e}, // 0x8B
    {"ADC A, H", 1, 4, 1, op_adc_a_h}, // 0x8C
    {"ADC A, L", 1, 4, 1, op_adc_a_l}, // 0x8D
    {"ADC A, (HL)", 1, 8, 1, op_adc_a_hlp}, // 0x8E
    {"ADC A, A", 1, 4, 1, op_adc_a_a}, // 0x8F
    {"SUB B", 1, 4, 1, op_sub_b}, // 0x90
    {"SUB C", 1, 4, 1, op_sub_c}, // 0x91
    {"SUB D", 1, 4, 1, op_sub_d}, // 0x92
    {"SUB E", 1, 4, 1, op_sub_e}, // 0x93
    {"SUB H", 1, 4, 1, op_sub_h}, // 0x94
    {"SUB L", 1, 4, 1, op_sub_l}, // 0x95
    {"SUB (HL)", 1, 8, 1, op_sub_hlp}, // 0x96
    {"SUB A", 1, 4, 1, op_sub_a}, // 0x97
    {"SBC A, B", 1, 4, 1, op_sbc_a_b}, // 0x98
    {"SBC A, C", 1, 4, 1, op_sbc_a_c}, // 0x99
    {"SBC A, D", 1, 4, 1, op_sbc_a_d}, // 0x9A
    {"SBC A, E", 1, 4, 1, op_sbc_a_e}, // 0x9B
    {"SBC A, H", 1, 4, 1, op_sbc_a_h}, // 0x9C
    {"SBC A, L", 1, 4, 1, op_sbc_a_l}, // 0x9D
    {"SBC A, (HL)", 1, 8, 1, op_sbc_a_hlp}, // 0x9E
    {"SBC A, A", 1, 4, 1, op_sbc_a_a}, // 0x9F
    {"AND B", 1, 4, 1, op_and_b}, // 0xA0
    {"AND C", 1, 4, 1, op_and_c}, // 0xA1
    {"AND D", 1, 4, 1, op_and_d}, // 0xA2
    {"AND E", 1, 4, 1, op_and_e}, // 0xA3
    {"AND H", 1, 4, 1, op_and_h}, // 0xA4
    {"AND L", 1, 4, 1, op_and_l}, // 0xA5
    {"AND (HL)", 1, 8, 1, op_and_hlp}, // 0xA6
    {"AND A", 1, 4, 1, op_and_a}, // 0xA7
    {"XOR B", 1, 4, 1, op_xor_b}, // 0xA8
    {"XOR C", 1, 4, 1, op_xor_c}, // 0xA9
    {"XOR D", 1, 4, 1, op_xor_d}, // 0xAA
    {"XOR E", 1, 4, 1, op_xor_e}, // 0xAB
    {"XOR H", 1, 4, 1, op_xor_h}, // 0xAC
    {"XOR L", 1, 4, 1, op_xor_l}, // 0xAD
    {"XOR (HL)", 1, 8, 1, op_xor_hlp}, // 0xAE
    {"XOR A", 1, 4, 1, op_xor_a}, // 0xAF
    {"OR B", 1, 4, 1, op_or_b}, // 0xB0
    {"OR C", 1, 4, 1, op_or_c}, // 0xB1
    {"OR D", 1, 4, 1, op_or_d}, // 0xB2
    {"OR E", 1, 4, 1, op_or_e}, // 0xB3
    {"OR H", 1, 4, 1, op_or_h}, // 0xB4
    {"OR L", 1, 4, 1, op_or_l}, // 0xB5
    {"OR (HL)", 1, 8, 1, op_or_hlp}, // 0xB6
    {"OR A", 1, 4, 1, op_or_a}, // 0xB7
    {"CP B", 1, 4, 1, op_cp_b}, // 0xB8
    {"CP C", 1, 4, 1, op_cp_c}, // 0xB9
    {"CP D", 1, 4, 1, op_cp_d}, // 0xBA
    {"CP E", 1, 4, 1, op_cp_e}, // 0xBB
    {"CP H", 1, 4, 1, op_cp_h}, // 0xBC
    {"CP L", 1, 4, 1, op_cp_l}, // 0xBD
    {"CP (HL)", 1, 8, 1, op_cp_hlp}, // 0xBE
    {"CP A", 1, 4, 1, op_cp_a}, // 0xBF
    {"RET NZ", 1, 20, 0, op_ret_nz}, // 0xC0
    {"POP BC", 1, 12, 1, op_pop_bc}, // 0xC1
    {"JP NZ, %04x", 3, 16, 0, op_jp_nz_a16}, // 0xC2
    {"JP %04x", 3, 16, 0, op_jp_a16}, // 0xC3
    {"CALL NZ, %04x", 3, 24, 0, op_call_nz_a16}, // 0xC4
    {"PUSH BC", 1, 16, 1, op_push_bc}, // 0xC5
    {"ADD A, %02x", 2, 8, 1, op_add_a_d8}, // 0xC6
    {"RST 00H", 1, 16, 0, op_rst_00h}, // 0xC7
    {"RET Z", 1, 20, 0, op_ret_z}, // 0xC8
    {"RET", 1, 16, 0, op_ret}, // 0xC9
    {"JP Z, %04x", 3, 16, 0, op_jp_z_a16}, // 0xCA
    {"PREFIX CB", 1, 4, 1, op_prefix_cb}, // 0xCB
    {"CALL Z, %04x", 3, 24, 0, op_call_z_a16}, // 0xCC
    {"CALL %04x", 3, 24, 0, op_call_a16}, // 0xCD
    {"ADC A, %02x", 2, 8, 1, op_adc_a_d8}, // 0xCE
    {"RST 08H", 1, 16, 0, op_rst_08h}, // 0xCF
    {"RET NC", 1, 20, 0, op_ret_nc}, // 0xD0
    {"POP DE", 1, 12, 1, op_pop_de}, // 0xD1
    {"JP NC, %04x", 3, 16, 0, op_jp_nc_a16}, // 0xD2
    {"NOP", 1, 4, 1, op_nop}, // 0xD3
    {"CALL NC, %04x", 3, 24, 0, op_call_nc_a16}, // 0xD4
    {"PUSH DE", 1, 16, 1, op_push_de}, // 0xD5
    {"SUB %02x", 2, 8, 1, op_sub_d8}, // 0xD6
    {"RST 10H", 1, 16, 0, op_rst_10h}, // 0xD7
    {"RET C", 1, 20, 0, op_ret_c}, // 0xD8
    {"RETI", 1, 16, 0, op_reti}, // 0xD9
    {"JP C, %04x", 3, 16, 0, op_jp_c_a16}, // 0xDA
    {"NOP", 1, 4, 1, op_nop}, // 0xDB
    {"CALL C, %04x", 3, 24, 0, op_call_c_a16}, // 0xDC
    {"NOP", 1, 4, 1, op_nop}, // 0xDD
    {"SBC A, %02x", 2, 8, 1, op_sbc_a_d8}, // 0xDE
    {"RST 18H", 1, 16, 0, op_rst_18h}, // 0xDF
    {"LDH (%02x), A", 2, 12, 1, op_ldh_a8p_a}, // 0xE0
    {"POP HL", 1, 12, 1, op_pop_hl}, // 0xE1
    {"LD (C), A", 2, 8, 1, op_ld_cp_a}, // 0xE2
    {"NOP", 1, 4, 1, op_nop}, // 0xE3
    {"NOP", 1, 4, 1, op_nop}, // 0xE4
    {"PUSH HL", 1, 16, 1, op_push_hl}, // 0xE5
    {"AND %02x", 2, 8, 1, op_and_d8}, // 0xE6
    {"RST 20H", 1, 16, 0, op_rst_20h}, // 0xE7
    {"ADD SP, %02x", 2, 16, 1, op_add_sp_r8}, // 0xE8
    {"JP (HL)", 1, 4, 0, op_jp_hlp}, // 0xE9
    {"LD (%04x), A", 3, 16, 1, op_ld_a16p_a}, // 0xEA
    {"NOP", 1, 4, 1, op_nop}, // 0xEB
    {"NOP", 1, 4, 1, op_nop}, // 0xEC
    {"NOP", 1, 4, 1, op_nop}, // 0xED
    {"XOR %02x", 2, 8, 1, op_xor_d8}, // 0xEE
    {"RST 28H", 1, 16, 0, op_rst_28h}, // 0xEF
    {"LDH A, (%02x)", 2, 12, 1, op_ldh_a_a8p}, // 0xF0
    {"POP AF", 1, 12, 1, op_pop_af}, // 0xF1
    {"LD A, (C)", 2, 8, 1, op_ld_a_cp}, // 0xF2
    {"DI", 1, 4, 1, op_di}, // 0xF3
    {"NOP", 1, 4, 1, op_nop}, // 0xF4
    {"PUSH AF", 1, 16, 1, op_push_af}, // 0xF5
    {"OR %02x", 2, 8, 1, op_or_d8}, // 0xF6
    {"RST 30H", 1, 16, 0, op_rst_30h}, // 0xF7
    {"LD HL, SP+%02x", 2, 12, 1, op_ld_hl_sppr8}, // 0xF8
    {"LD SP, HL", 1, 8, 1, op_ld_sp_hl}, // 0xF9
    {"LD A, (%04x)", 3, 16, 1, op_ld_a_a16p}, // 0xFA
    {"EI", 1, 4, 1, op_ei}, // 0xFB
    {"NOP", 1, 4, 1, op_nop}, // 0xFC
    {"NOP", 1, 4, 1, op_nop}, // 0xFD
    {"CP %02x", 2, 8, 1, op_cp_d8}, // 0xFE
    {"RST 38H", 1, 16, 0, op_rst_38h} // 0xFF
};

// 0xCB00: RLC B (Z 0 0 C)
void op_rlc_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RLC B\n");
}

// 0xCB01: RLC C (Z 0 0 C)
void op_rlc_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RLC C\n");
}

// 0xCB02: RLC D (Z 0 0 C)
void op_rlc_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RLC D\n");
}

// 0xCB03: RLC E (Z 0 0 C)
void op_rlc_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RLC E\n");
}

// 0xCB04: RLC H (Z 0 0 C)
void op_rlc_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RLC H\n");
}

// 0xCB05: RLC L (Z 0 0 C)
void op_rlc_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RLC L\n");
}

// 0xCB06: RLC (HL) (Z 0 0 C)
void op_rlc_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RLC (HL)\n");
}

// 0xCB07: RLC A (Z 0 0 C)
void op_rlc_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RLC A\n");
}

// 0xCB08: RRC B (Z 0 0 C)
void op_rrc_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RRC B\n");
}

// 0xCB09: RRC C (Z 0 0 C)
void op_rrc_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RRC C\n");
}

// 0xCB0A: RRC D (Z 0 0 C)
void op_rrc_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RRC D\n");
}

// 0xCB0B: RRC E (Z 0 0 C)
void op_rrc_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RRC E\n");
}

// 0xCB0C: RRC H (Z 0 0 C)
void op_rrc_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RRC H\n");
}

// 0xCB0D: RRC L (Z 0 0 C)
void op_rrc_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RRC L\n");
}

// 0xCB0E: RRC (HL) (Z 0 0 C)
void op_rrc_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RRC (HL)\n");
}

// 0xCB0F: RRC A (Z 0 0 C)
void op_rrc_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RRC A\n");
}

// 0xCB10: RL B (Z 0 0 C)
void op_rl_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RL B\n");
}

// 0xCB11: RL C (Z 0 0 C)
void op_rl_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RL C\n");
}

// 0xCB12: RL D (Z 0 0 C)
void op_rl_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RL D\n");
}

// 0xCB13: RL E (Z 0 0 C)
void op_rl_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RL E\n");
}

// 0xCB14: RL H (Z 0 0 C)
void op_rl_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RL H\n");
}

// 0xCB15: RL L (Z 0 0 C)
void op_rl_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RL L\n");
}

// 0xCB16: RL (HL) (Z 0 0 C)
void op_rl_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RL (HL)\n");
}

// 0xCB17: RL A (Z 0 0 C)
void op_rl_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RL A\n");
}

// 0xCB18: RR B (Z 0 0 C)
void op_rr_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RR B\n");
}

// 0xCB19: RR C (Z 0 0 C)
void op_rr_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RR C\n");
}

// 0xCB1A: RR D (Z 0 0 C)
void op_rr_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RR D\n");
}

// 0xCB1B: RR E (Z 0 0 C)
void op_rr_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RR E\n");
}

// 0xCB1C: RR H (Z 0 0 C)
void op_rr_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RR H\n");
}

// 0xCB1D: RR L (Z 0 0 C)
void op_rr_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RR L\n");
}

// 0xCB1E: RR (HL) (Z 0 0 C)
void op_rr_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RR (HL)\n");
}

// 0xCB1F: RR A (Z 0 0 C)
void op_rr_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RR A\n");
}

// 0xCB20: SLA B (Z 0 0 C)
void op_sla_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SLA B\n");
}

// 0xCB21: SLA C (Z 0 0 C)
void op_sla_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SLA C\n");
}

// 0xCB22: SLA D (Z 0 0 C)
void op_sla_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SLA D\n");
}

// 0xCB23: SLA E (Z 0 0 C)
void op_sla_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SLA E\n");
}

// 0xCB24: SLA H (Z 0 0 C)
void op_sla_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SLA H\n");
}

// 0xCB25: SLA L (Z 0 0 C)
void op_sla_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SLA L\n");
}

// 0xCB26: SLA (HL) (Z 0 0 C)
void op_sla_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SLA (HL)\n");
}

// 0xCB27: SLA A (Z 0 0 C)
void op_sla_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SLA A\n");
}

// 0xCB28: SRA B (Z 0 0 0)
void op_sra_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRA B\n");
}

// 0xCB29: SRA C (Z 0 0 0)
void op_sra_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRA C\n");
}

// 0xCB2A: SRA D (Z 0 0 0)
void op_sra_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRA D\n");
}

// 0xCB2B: SRA E (Z 0 0 0)
void op_sra_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRA E\n");
}

// 0xCB2C: SRA H (Z 0 0 0)
void op_sra_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRA H\n");
}

// 0xCB2D: SRA L (Z 0 0 0)
void op_sra_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRA L\n");
}

// 0xCB2E: SRA (HL) (Z 0 0 0)
void op_sra_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRA (HL)\n");
}

// 0xCB2F: SRA A (Z 0 0 0)
void op_sra_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRA A\n");
}

// 0xCB30: SWAP B (Z 0 0 0)
void op_swap_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SWAP B\n");
}

// 0xCB31: SWAP C (Z 0 0 0)
void op_swap_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SWAP C\n");
}

// 0xCB32: SWAP D (Z 0 0 0)
void op_swap_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SWAP D\n");
}

// 0xCB33: SWAP E (Z 0 0 0)
void op_swap_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SWAP E\n");
}

// 0xCB34: SWAP H (Z 0 0 0)
void op_swap_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SWAP H\n");
}

// 0xCB35: SWAP L (Z 0 0 0)
void op_swap_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SWAP L\n");
}

// 0xCB36: SWAP (HL) (Z 0 0 0)
void op_swap_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SWAP (HL)\n");
}

// 0xCB37: SWAP A (Z 0 0 0)
void op_swap_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SWAP A\n");
}

// 0xCB38: SRL B (Z 0 0 C)
void op_srl_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRL B\n");
}

// 0xCB39: SRL C (Z 0 0 C)
void op_srl_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRL C\n");
}

// 0xCB3A: SRL D (Z 0 0 C)
void op_srl_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRL D\n");
}

// 0xCB3B: SRL E (Z 0 0 C)
void op_srl_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRL E\n");
}

// 0xCB3C: SRL H (Z 0 0 C)
void op_srl_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRL H\n");
}

// 0xCB3D: SRL L (Z 0 0 C)
void op_srl_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRL L\n");
}

// 0xCB3E: SRL (HL) (Z 0 0 C)
void op_srl_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRL (HL)\n");
}

// 0xCB3F: SRL A (Z 0 0 C)
void op_srl_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SRL A\n");
}

// 0xCB40: BIT 0, B (Z 0 1 -)
void op_bit_0_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 0, B\n");
}

// 0xCB41: BIT 0, C (Z 0 1 -)
void op_bit_0_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 0, C\n");
}

// 0xCB42: BIT 0, D (Z 0 1 -)
void op_bit_0_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 0, D\n");
}

// 0xCB43: BIT 0, E (Z 0 1 -)
void op_bit_0_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 0, E\n");
}

// 0xCB44: BIT 0, H (Z 0 1 -)
void op_bit_0_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 0, H\n");
}

// 0xCB45: BIT 0, L (Z 0 1 -)
void op_bit_0_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 0, L\n");
}

// 0xCB46: BIT 0, (HL) (Z 0 1 -)
void op_bit_0_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 0, (HL)\n");
}

// 0xCB47: BIT 0, A (Z 0 1 -)
void op_bit_0_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 0, A\n");
}

// 0xCB48: BIT 1, B (Z 0 1 -)
void op_bit_1_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 1, B\n");
}

// 0xCB49: BIT 1, C (Z 0 1 -)
void op_bit_1_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 1, C\n");
}

// 0xCB4A: BIT 1, D (Z 0 1 -)
void op_bit_1_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 1, D\n");
}

// 0xCB4B: BIT 1, E (Z 0 1 -)
void op_bit_1_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 1, E\n");
}

// 0xCB4C: BIT 1, H (Z 0 1 -)
void op_bit_1_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 1, H\n");
}

// 0xCB4D: BIT 1, L (Z 0 1 -)
void op_bit_1_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 1, L\n");
}

// 0xCB4E: BIT 1, (HL) (Z 0 1 -)
void op_bit_1_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 1, (HL)\n");
}

// 0xCB4F: BIT 1, A (Z 0 1 -)
void op_bit_1_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 1, A\n");
}

// 0xCB50: BIT 2, B (Z 0 1 -)
void op_bit_2_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 2, B\n");
}

// 0xCB51: BIT 2, C (Z 0 1 -)
void op_bit_2_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 2, C\n");
}

// 0xCB52: BIT 2, D (Z 0 1 -)
void op_bit_2_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 2, D\n");
}

// 0xCB53: BIT 2, E (Z 0 1 -)
void op_bit_2_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 2, E\n");
}

// 0xCB54: BIT 2, H (Z 0 1 -)
void op_bit_2_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 2, H\n");
}

// 0xCB55: BIT 2, L (Z 0 1 -)
void op_bit_2_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 2, L\n");
}

// 0xCB56: BIT 2, (HL) (Z 0 1 -)
void op_bit_2_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 2, (HL)\n");
}

// 0xCB57: BIT 2, A (Z 0 1 -)
void op_bit_2_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 2, A\n");
}

// 0xCB58: BIT 3, B (Z 0 1 -)
void op_bit_3_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 3, B\n");
}

// 0xCB59: BIT 3, C (Z 0 1 -)
void op_bit_3_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 3, C\n");
}

// 0xCB5A: BIT 3, D (Z 0 1 -)
void op_bit_3_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 3, D\n");
}

// 0xCB5B: BIT 3, E (Z 0 1 -)
void op_bit_3_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 3, E\n");
}

// 0xCB5C: BIT 3, H (Z 0 1 -)
void op_bit_3_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 3, H\n");
}

// 0xCB5D: BIT 3, L (Z 0 1 -)
void op_bit_3_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 3, L\n");
}

// 0xCB5E: BIT 3, (HL) (Z 0 1 -)
void op_bit_3_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 3, (HL)\n");
}

// 0xCB5F: BIT 3, A (Z 0 1 -)
void op_bit_3_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 3, A\n");
}

// 0xCB60: BIT 4, B (Z 0 1 -)
void op_bit_4_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 4, B\n");
}

// 0xCB61: BIT 4, C (Z 0 1 -)
void op_bit_4_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 4, C\n");
}

// 0xCB62: BIT 4, D (Z 0 1 -)
void op_bit_4_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 4, D\n");
}

// 0xCB63: BIT 4, E (Z 0 1 -)
void op_bit_4_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 4, E\n");
}

// 0xCB64: BIT 4, H (Z 0 1 -)
void op_bit_4_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 4, H\n");
}

// 0xCB65: BIT 4, L (Z 0 1 -)
void op_bit_4_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 4, L\n");
}

// 0xCB66: BIT 4, (HL) (Z 0 1 -)
void op_bit_4_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 4, (HL)\n");
}

// 0xCB67: BIT 4, A (Z 0 1 -)
void op_bit_4_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 4, A\n");
}

// 0xCB68: BIT 5, B (Z 0 1 -)
void op_bit_5_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 5, B\n");
}

// 0xCB69: BIT 5, C (Z 0 1 -)
void op_bit_5_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 5, C\n");
}

// 0xCB6A: BIT 5, D (Z 0 1 -)
void op_bit_5_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 5, D\n");
}

// 0xCB6B: BIT 5, E (Z 0 1 -)
void op_bit_5_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 5, E\n");
}

// 0xCB6C: BIT 5, H (Z 0 1 -)
void op_bit_5_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 5, H\n");
}

// 0xCB6D: BIT 5, L (Z 0 1 -)
void op_bit_5_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 5, L\n");
}

// 0xCB6E: BIT 5, (HL) (Z 0 1 -)
void op_bit_5_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 5, (HL)\n");
}

// 0xCB6F: BIT 5, A (Z 0 1 -)
void op_bit_5_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 5, A\n");
}

// 0xCB70: BIT 6, B (Z 0 1 -)
void op_bit_6_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 6, B\n");
}

// 0xCB71: BIT 6, C (Z 0 1 -)
void op_bit_6_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 6, C\n");
}

// 0xCB72: BIT 6, D (Z 0 1 -)
void op_bit_6_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 6, D\n");
}

// 0xCB73: BIT 6, E (Z 0 1 -)
void op_bit_6_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 6, E\n");
}

// 0xCB74: BIT 6, H (Z 0 1 -)
void op_bit_6_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 6, H\n");
}

// 0xCB75: BIT 6, L (Z 0 1 -)
void op_bit_6_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 6, L\n");
}

// 0xCB76: BIT 6, (HL) (Z 0 1 -)
void op_bit_6_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 6, (HL)\n");
}

// 0xCB77: BIT 6, A (Z 0 1 -)
void op_bit_6_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 6, A\n");
}

// 0xCB78: BIT 7, B (Z 0 1 -)
void op_bit_7_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 7, B\n");
}

// 0xCB79: BIT 7, C (Z 0 1 -)
void op_bit_7_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 7, C\n");
}

// 0xCB7A: BIT 7, D (Z 0 1 -)
void op_bit_7_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 7, D\n");
}

// 0xCB7B: BIT 7, E (Z 0 1 -)
void op_bit_7_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 7, E\n");
}

// 0xCB7C: BIT 7, H (Z 0 1 -)
void op_bit_7_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 7, H\n");
}

// 0xCB7D: BIT 7, L (Z 0 1 -)
void op_bit_7_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 7, L\n");
}

// 0xCB7E: BIT 7, (HL) (Z 0 1 -)
void op_bit_7_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 7, (HL)\n");
}

// 0xCB7F: BIT 7, A (Z 0 1 -)
void op_bit_7_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: BIT 7, A\n");
}

// 0xCB80: RES 0, B (- - - -)
void op_res_0_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 0, B\n");
}

// 0xCB81: RES 0, C (- - - -)
void op_res_0_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 0, C\n");
}

// 0xCB82: RES 0, D (- - - -)
void op_res_0_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 0, D\n");
}

// 0xCB83: RES 0, E (- - - -)
void op_res_0_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 0, E\n");
}

// 0xCB84: RES 0, H (- - - -)
void op_res_0_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 0, H\n");
}

// 0xCB85: RES 0, L (- - - -)
void op_res_0_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 0, L\n");
}

// 0xCB86: RES 0, (HL) (- - - -)
void op_res_0_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 0, (HL)\n");
}

// 0xCB87: RES 0, A (- - - -)
void op_res_0_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 0, A\n");
}

// 0xCB88: RES 1, B (- - - -)
void op_res_1_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 1, B\n");
}

// 0xCB89: RES 1, C (- - - -)
void op_res_1_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 1, C\n");
}

// 0xCB8A: RES 1, D (- - - -)
void op_res_1_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 1, D\n");
}

// 0xCB8B: RES 1, E (- - - -)
void op_res_1_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 1, E\n");
}

// 0xCB8C: RES 1, H (- - - -)
void op_res_1_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 1, H\n");
}

// 0xCB8D: RES 1, L (- - - -)
void op_res_1_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 1, L\n");
}

// 0xCB8E: RES 1, (HL) (- - - -)
void op_res_1_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 1, (HL)\n");
}

// 0xCB8F: RES 1, A (- - - -)
void op_res_1_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 1, A\n");
}

// 0xCB90: RES 2, B (- - - -)
void op_res_2_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 2, B\n");
}

// 0xCB91: RES 2, C (- - - -)
void op_res_2_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 2, C\n");
}

// 0xCB92: RES 2, D (- - - -)
void op_res_2_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 2, D\n");
}

// 0xCB93: RES 2, E (- - - -)
void op_res_2_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 2, E\n");
}

// 0xCB94: RES 2, H (- - - -)
void op_res_2_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 2, H\n");
}

// 0xCB95: RES 2, L (- - - -)
void op_res_2_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 2, L\n");
}

// 0xCB96: RES 2, (HL) (- - - -)
void op_res_2_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 2, (HL)\n");
}

// 0xCB97: RES 2, A (- - - -)
void op_res_2_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 2, A\n");
}

// 0xCB98: RES 3, B (- - - -)
void op_res_3_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 3, B\n");
}

// 0xCB99: RES 3, C (- - - -)
void op_res_3_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 3, C\n");
}

// 0xCB9A: RES 3, D (- - - -)
void op_res_3_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 3, D\n");
}

// 0xCB9B: RES 3, E (- - - -)
void op_res_3_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 3, E\n");
}

// 0xCB9C: RES 3, H (- - - -)
void op_res_3_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 3, H\n");
}

// 0xCB9D: RES 3, L (- - - -)
void op_res_3_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 3, L\n");
}

// 0xCB9E: RES 3, (HL) (- - - -)
void op_res_3_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 3, (HL)\n");
}

// 0xCB9F: RES 3, A (- - - -)
void op_res_3_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 3, A\n");
}

// 0xCBA0: RES 4, B (- - - -)
void op_res_4_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 4, B\n");
}

// 0xCBA1: RES 4, C (- - - -)
void op_res_4_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 4, C\n");
}

// 0xCBA2: RES 4, D (- - - -)
void op_res_4_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 4, D\n");
}

// 0xCBA3: RES 4, E (- - - -)
void op_res_4_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 4, E\n");
}

// 0xCBA4: RES 4, H (- - - -)
void op_res_4_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 4, H\n");
}

// 0xCBA5: RES 4, L (- - - -)
void op_res_4_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 4, L\n");
}

// 0xCBA6: RES 4, (HL) (- - - -)
void op_res_4_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 4, (HL)\n");
}

// 0xCBA7: RES 4, A (- - - -)
void op_res_4_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 4, A\n");
}

// 0xCBA8: RES 5, B (- - - -)
void op_res_5_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 5, B\n");
}

// 0xCBA9: RES 5, C (- - - -)
void op_res_5_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 5, C\n");
}

// 0xCBAA: RES 5, D (- - - -)
void op_res_5_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 5, D\n");
}

// 0xCBAB: RES 5, E (- - - -)
void op_res_5_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 5, E\n");
}

// 0xCBAC: RES 5, H (- - - -)
void op_res_5_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 5, H\n");
}

// 0xCBAD: RES 5, L (- - - -)
void op_res_5_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 5, L\n");
}

// 0xCBAE: RES 5, (HL) (- - - -)
void op_res_5_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 5, (HL)\n");
}

// 0xCBAF: RES 5, A (- - - -)
void op_res_5_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 5, A\n");
}

// 0xCBB0: RES 6, B (- - - -)
void op_res_6_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 6, B\n");
}

// 0xCBB1: RES 6, C (- - - -)
void op_res_6_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 6, C\n");
}

// 0xCBB2: RES 6, D (- - - -)
void op_res_6_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 6, D\n");
}

// 0xCBB3: RES 6, E (- - - -)
void op_res_6_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 6, E\n");
}

// 0xCBB4: RES 6, H (- - - -)
void op_res_6_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 6, H\n");
}

// 0xCBB5: RES 6, L (- - - -)
void op_res_6_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 6, L\n");
}

// 0xCBB6: RES 6, (HL) (- - - -)
void op_res_6_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 6, (HL)\n");
}

// 0xCBB7: RES 6, A (- - - -)
void op_res_6_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 6, A\n");
}

// 0xCBB8: RES 7, B (- - - -)
void op_res_7_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 7, B\n");
}

// 0xCBB9: RES 7, C (- - - -)
void op_res_7_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 7, C\n");
}

// 0xCBBA: RES 7, D (- - - -)
void op_res_7_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 7, D\n");
}

// 0xCBBB: RES 7, E (- - - -)
void op_res_7_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 7, E\n");
}

// 0xCBBC: RES 7, H (- - - -)
void op_res_7_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 7, H\n");
}

// 0xCBBD: RES 7, L (- - - -)
void op_res_7_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 7, L\n");
}

// 0xCBBE: RES 7, (HL) (- - - -)
void op_res_7_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 7, (HL)\n");
}

// 0xCBBF: RES 7, A (- - - -)
void op_res_7_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: RES 7, A\n");
}

// 0xCBC0: SET 0, B (- - - -)
void op_set_0_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 0, B\n");
}

// 0xCBC1: SET 0, C (- - - -)
void op_set_0_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 0, C\n");
}

// 0xCBC2: SET 0, D (- - - -)
void op_set_0_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 0, D\n");
}

// 0xCBC3: SET 0, E (- - - -)
void op_set_0_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 0, E\n");
}

// 0xCBC4: SET 0, H (- - - -)
void op_set_0_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 0, H\n");
}

// 0xCBC5: SET 0, L (- - - -)
void op_set_0_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 0, L\n");
}

// 0xCBC6: SET 0, (HL) (- - - -)
void op_set_0_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 0, (HL)\n");
}

// 0xCBC7: SET 0, A (- - - -)
void op_set_0_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 0, A\n");
}

// 0xCBC8: SET 1, B (- - - -)
void op_set_1_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 1, B\n");
}

// 0xCBC9: SET 1, C (- - - -)
void op_set_1_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 1, C\n");
}

// 0xCBCA: SET 1, D (- - - -)
void op_set_1_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 1, D\n");
}

// 0xCBCB: SET 1, E (- - - -)
void op_set_1_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 1, E\n");
}

// 0xCBCC: SET 1, H (- - - -)
void op_set_1_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 1, H\n");
}

// 0xCBCD: SET 1, L (- - - -)
void op_set_1_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 1, L\n");
}

// 0xCBCE: SET 1, (HL) (- - - -)
void op_set_1_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 1, (HL)\n");
}

// 0xCBCF: SET 1, A (- - - -)
void op_set_1_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 1, A\n");
}

// 0xCBD0: SET 2, B (- - - -)
void op_set_2_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 2, B\n");
}

// 0xCBD1: SET 2, C (- - - -)
void op_set_2_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 2, C\n");
}

// 0xCBD2: SET 2, D (- - - -)
void op_set_2_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 2, D\n");
}

// 0xCBD3: SET 2, E (- - - -)
void op_set_2_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 2, E\n");
}

// 0xCBD4: SET 2, H (- - - -)
void op_set_2_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 2, H\n");
}

// 0xCBD5: SET 2, L (- - - -)
void op_set_2_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 2, L\n");
}

// 0xCBD6: SET 2, (HL) (- - - -)
void op_set_2_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 2, (HL)\n");
}

// 0xCBD7: SET 2, A (- - - -)
void op_set_2_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 2, A\n");
}

// 0xCBD8: SET 3, B (- - - -)
void op_set_3_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 3, B\n");
}

// 0xCBD9: SET 3, C (- - - -)
void op_set_3_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 3, C\n");
}

// 0xCBDA: SET 3, D (- - - -)
void op_set_3_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 3, D\n");
}

// 0xCBDB: SET 3, E (- - - -)
void op_set_3_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 3, E\n");
}

// 0xCBDC: SET 3, H (- - - -)
void op_set_3_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 3, H\n");
}

// 0xCBDD: SET 3, L (- - - -)
void op_set_3_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 3, L\n");
}

// 0xCBDE: SET 3, (HL) (- - - -)
void op_set_3_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 3, (HL)\n");
}

// 0xCBDF: SET 3, A (- - - -)
void op_set_3_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 3, A\n");
}

// 0xCBE0: SET 4, B (- - - -)
void op_set_4_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 4, B\n");
}

// 0xCBE1: SET 4, C (- - - -)
void op_set_4_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 4, C\n");
}

// 0xCBE2: SET 4, D (- - - -)
void op_set_4_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 4, D\n");
}

// 0xCBE3: SET 4, E (- - - -)
void op_set_4_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 4, E\n");
}

// 0xCBE4: SET 4, H (- - - -)
void op_set_4_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 4, H\n");
}

// 0xCBE5: SET 4, L (- - - -)
void op_set_4_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 4, L\n");
}

// 0xCBE6: SET 4, (HL) (- - - -)
void op_set_4_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 4, (HL)\n");
}

// 0xCBE7: SET 4, A (- - - -)
void op_set_4_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 4, A\n");
}

// 0xCBE8: SET 5, B (- - - -)
void op_set_5_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 5, B\n");
}

// 0xCBE9: SET 5, C (- - - -)
void op_set_5_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 5, C\n");
}

// 0xCBEA: SET 5, D (- - - -)
void op_set_5_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 5, D\n");
}

// 0xCBEB: SET 5, E (- - - -)
void op_set_5_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 5, E\n");
}

// 0xCBEC: SET 5, H (- - - -)
void op_set_5_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 5, H\n");
}

// 0xCBED: SET 5, L (- - - -)
void op_set_5_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 5, L\n");
}

// 0xCBEE: SET 5, (HL) (- - - -)
void op_set_5_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 5, (HL)\n");
}

// 0xCBEF: SET 5, A (- - - -)
void op_set_5_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 5, A\n");
}

// 0xCBF0: SET 6, B (- - - -)
void op_set_6_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 6, B\n");
}

// 0xCBF1: SET 6, C (- - - -)
void op_set_6_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 6, C\n");
}

// 0xCBF2: SET 6, D (- - - -)
void op_set_6_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 6, D\n");
}

// 0xCBF3: SET 6, E (- - - -)
void op_set_6_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 6, E\n");
}

// 0xCBF4: SET 6, H (- - - -)
void op_set_6_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 6, H\n");
}

// 0xCBF5: SET 6, L (- - - -)
void op_set_6_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 6, L\n");
}

// 0xCBF6: SET 6, (HL) (- - - -)
void op_set_6_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 6, (HL)\n");
}

// 0xCBF7: SET 6, A (- - - -)
void op_set_6_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 6, A\n");
}

// 0xCBF8: SET 7, B (- - - -)
void op_set_7_b(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 7, B\n");
}

// 0xCBF9: SET 7, C (- - - -)
void op_set_7_c(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 7, C\n");
}

// 0xCBFA: SET 7, D (- - - -)
void op_set_7_d(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 7, D\n");
}

// 0xCBFB: SET 7, E (- - - -)
void op_set_7_e(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 7, E\n");
}

// 0xCBFC: SET 7, H (- - - -)
void op_set_7_h(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 7, H\n");
}

// 0xCBFD: SET 7, L (- - - -)
void op_set_7_l(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 7, L\n");
}

// 0xCBFE: SET 7, (HL) (- - - -)
void op_set_7_hlp(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 7, (HL)\n");
}

// 0xCBFF: SET 7, A (- - - -)
void op_set_7_a(struct gbc_system **gbc, unsigned char operand) {
    printf("Unimplemented Instruction: SET 7, A\n");
}

struct gbc_instr cb_instructions[CB_INSTRUCTION_COUNT] = {
    {"RLC B", 2, 8, 1, op_rlc_b}, // 0xCB00
    {"RLC C", 2, 8, 1, op_rlc_c}, // 0xCB01
    {"RLC D", 2, 8, 1, op_rlc_d}, // 0xCB02
    {"RLC E", 2, 8, 1, op_rlc_e}, // 0xCB03
    {"RLC H", 2, 8, 1, op_rlc_h}, // 0xCB04
    {"RLC L", 2, 8, 1, op_rlc_l}, // 0xCB05
    {"RLC (HL)", 2, 16, 1, op_rlc_hlp}, // 0xCB06
    {"RLC A", 2, 8, 1, op_rlc_a}, // 0xCB07
    {"RRC B", 2, 8, 1, op_rrc_b}, // 0xCB08
    {"RRC C", 2, 8, 1, op_rrc_c}, // 0xCB09
    {"RRC D", 2, 8, 1, op_rrc_d}, // 0xCB0A
    {"RRC E", 2, 8, 1, op_rrc_e}, // 0xCB0B
    {"RRC H", 2, 8, 1, op_rrc_h}, // 0xCB0C
    {"RRC L", 2, 8, 1, op_rrc_l}, // 0xCB0D
    {"RRC (HL)", 2, 16, 1, op_rrc_hlp}, // 0xCB0E
    {"RRC A", 2, 8, 1, op_rrc_a}, // 0xCB0F
    {"RL B", 2, 8, 1, op_rl_b}, // 0xCB10
    {"RL C", 2, 8, 1, op_rl_c}, // 0xCB11
    {"RL D", 2, 8, 1, op_rl_d}, // 0xCB12
    {"RL E", 2, 8, 1, op_rl_e}, // 0xCB13
    {"RL H", 2, 8, 1, op_rl_h}, // 0xCB14
    {"RL L", 2, 8, 1, op_rl_l}, // 0xCB15
    {"RL (HL)", 2, 16, 1, op_rl_hlp}, // 0xCB16
    {"RL A", 2, 8, 1, op_rl_a}, // 0xCB17
    {"RR B", 2, 8, 1, op_rr_b}, // 0xCB18
    {"RR C", 2, 8, 1, op_rr_c}, // 0xCB19
    {"RR D", 2, 8, 1, op_rr_d}, // 0xCB1A
    {"RR E", 2, 8, 1, op_rr_e}, // 0xCB1B
    {"RR H", 2, 8, 1, op_rr_h}, // 0xCB1C
    {"RR L", 2, 8, 1, op_rr_l}, // 0xCB1D
    {"RR (HL)", 2, 16, 1, op_rr_hlp}, // 0xCB1E
    {"RR A", 2, 8, 1, op_rr_a}, // 0xCB1F
    {"SLA B", 2, 8, 1, op_sla_b}, // 0xCB20
    {"SLA C", 2, 8, 1, op_sla_c}, // 0xCB21
    {"SLA D", 2, 8, 1, op_sla_d}, // 0xCB22
    {"SLA E", 2, 8, 1, op_sla_e}, // 0xCB23
    {"SLA H", 2, 8, 1, op_sla_h}, // 0xCB24
    {"SLA L", 2, 8, 1, op_sla_l}, // 0xCB25
    {"SLA (HL)", 2, 16, 1, op_sla_hlp}, // 0xCB26
    {"SLA A", 2, 8, 1, op_sla_a}, // 0xCB27
    {"SRA B", 2, 8, 1, op_sra_b}, // 0xCB28
    {"SRA C", 2, 8, 1, op_sra_c}, // 0xCB29
    {"SRA D", 2, 8, 1, op_sra_d}, // 0xCB2A
    {"SRA E", 2, 8, 1, op_sra_e}, // 0xCB2B
    {"SRA H", 2, 8, 1, op_sra_h}, // 0xCB2C
    {"SRA L", 2, 8, 1, op_sra_l}, // 0xCB2D
    {"SRA (HL)", 2, 16, 1, op_sra_hlp}, // 0xCB2E
    {"SRA A", 2, 8, 1, op_sra_a}, // 0xCB2F
    {"SWAP B", 2, 8, 1, op_swap_b}, // 0xCB30
    {"SWAP C", 2, 8, 1, op_swap_c}, // 0xCB31
    {"SWAP D", 2, 8, 1, op_swap_d}, // 0xCB32
    {"SWAP E", 2, 8, 1, op_swap_e}, // 0xCB33
    {"SWAP H", 2, 8, 1, op_swap_h}, // 0xCB34
    {"SWAP L", 2, 8, 1, op_swap_l}, // 0xCB35
    {"SWAP (HL)", 2, 16, 1, op_swap_hlp}, // 0xCB36
    {"SWAP A", 2, 8, 1, op_swap_a}, // 0xCB37
    {"SRL B", 2, 8, 1, op_srl_b}, // 0xCB38
    {"SRL C", 2, 8, 1, op_srl_c}, // 0xCB39
    {"SRL D", 2, 8, 1, op_srl_d}, // 0xCB3A
    {"SRL E", 2, 8, 1, op_srl_e}, // 0xCB3B
    {"SRL H", 2, 8, 1, op_srl_h}, // 0xCB3C
    {"SRL L", 2, 8, 1, op_srl_l}, // 0xCB3D
    {"SRL (HL)", 2, 16, 1, op_srl_hlp}, // 0xCB3E
    {"SRL A", 2, 8, 1, op_srl_a}, // 0xCB3F
    {"BIT 0, B", 2, 8, 1, op_bit_0_b}, // 0xCB40
    {"BIT 0, C", 2, 8, 1, op_bit_0_c}, // 0xCB41
    {"BIT 0, D", 2, 8, 1, op_bit_0_d}, // 0xCB42
    {"BIT 0, E", 2, 8, 1, op_bit_0_e}, // 0xCB43
    {"BIT 0, H", 2, 8, 1, op_bit_0_h}, // 0xCB44
    {"BIT 0, L", 2, 8, 1, op_bit_0_l}, // 0xCB45
    {"BIT 0, (HL)", 2, 16, 1, op_bit_0_hlp}, // 0xCB46
    {"BIT 0, A", 2, 8, 1, op_bit_0_a}, // 0xCB47
    {"BIT 1, B", 2, 8, 1, op_bit_1_b}, // 0xCB48
    {"BIT 1, C", 2, 8, 1, op_bit_1_c}, // 0xCB49
    {"BIT 1, D", 2, 8, 1, op_bit_1_d}, // 0xCB4A
    {"BIT 1, E", 2, 8, 1, op_bit_1_e}, // 0xCB4B
    {"BIT 1, H", 2, 8, 1, op_bit_1_h}, // 0xCB4C
    {"BIT 1, L", 2, 8, 1, op_bit_1_l}, // 0xCB4D
    {"BIT 1, (HL)", 2, 16, 1, op_bit_1_hlp}, // 0xCB4E
    {"BIT 1, A", 2, 8, 1, op_bit_1_a}, // 0xCB4F
    {"BIT 2, B", 2, 8, 1, op_bit_2_b}, // 0xCB50
    {"BIT 2, C", 2, 8, 1, op_bit_2_c}, // 0xCB51
    {"BIT 2, D", 2, 8, 1, op_bit_2_d}, // 0xCB52
    {"BIT 2, E", 2, 8, 1, op_bit_2_e}, // 0xCB53
    {"BIT 2, H", 2, 8, 1, op_bit_2_h}, // 0xCB54
    {"BIT 2, L", 2, 8, 1, op_bit_2_l}, // 0xCB55
    {"BIT 2, (HL)", 2, 16, 1, op_bit_2_hlp}, // 0xCB56
    {"BIT 2, A", 2, 8, 1, op_bit_2_a}, // 0xCB57
    {"BIT 3, B", 2, 8, 1, op_bit_3_b}, // 0xCB58
    {"BIT 3, C", 2, 8, 1, op_bit_3_c}, // 0xCB59
    {"BIT 3, D", 2, 8, 1, op_bit_3_d}, // 0xCB5A
    {"BIT 3, E", 2, 8, 1, op_bit_3_e}, // 0xCB5B
    {"BIT 3, H", 2, 8, 1, op_bit_3_h}, // 0xCB5C
    {"BIT 3, L", 2, 8, 1, op_bit_3_l}, // 0xCB5D
    {"BIT 3, (HL)", 2, 16, 1, op_bit_3_hlp}, // 0xCB5E
    {"BIT 3, A", 2, 8, 1, op_bit_3_a}, // 0xCB5F
    {"BIT 4, B", 2, 8, 1, op_bit_4_b}, // 0xCB60
    {"BIT 4, C", 2, 8, 1, op_bit_4_c}, // 0xCB61
    {"BIT 4, D", 2, 8, 1, op_bit_4_d}, // 0xCB62
    {"BIT 4, E", 2, 8, 1, op_bit_4_e}, // 0xCB63
    {"BIT 4, H", 2, 8, 1, op_bit_4_h}, // 0xCB64
    {"BIT 4, L", 2, 8, 1, op_bit_4_l}, // 0xCB65
    {"BIT 4, (HL)", 2, 16, 1, op_bit_4_hlp}, // 0xCB66
    {"BIT 4, A", 2, 8, 1, op_bit_4_a}, // 0xCB67
    {"BIT 5, B", 2, 8, 1, op_bit_5_b}, // 0xCB68
    {"BIT 5, C", 2, 8, 1, op_bit_5_c}, // 0xCB69
    {"BIT 5, D", 2, 8, 1, op_bit_5_d}, // 0xCB6A
    {"BIT 5, E", 2, 8, 1, op_bit_5_e}, // 0xCB6B
    {"BIT 5, H", 2, 8, 1, op_bit_5_h}, // 0xCB6C
    {"BIT 5, L", 2, 8, 1, op_bit_5_l}, // 0xCB6D
    {"BIT 5, (HL)", 2, 16, 1, op_bit_5_hlp}, // 0xCB6E
    {"BIT 5, A", 2, 8, 1, op_bit_5_a}, // 0xCB6F
    {"BIT 6, B", 2, 8, 1, op_bit_6_b}, // 0xCB70
    {"BIT 6, C", 2, 8, 1, op_bit_6_c}, // 0xCB71
    {"BIT 6, D", 2, 8, 1, op_bit_6_d}, // 0xCB72
    {"BIT 6, E", 2, 8, 1, op_bit_6_e}, // 0xCB73
    {"BIT 6, H", 2, 8, 1, op_bit_6_h}, // 0xCB74
    {"BIT 6, L", 2, 8, 1, op_bit_6_l}, // 0xCB75
    {"BIT 6, (HL)", 2, 16, 1, op_bit_6_hlp}, // 0xCB76
    {"BIT 6, A", 2, 8, 1, op_bit_6_a}, // 0xCB77
    {"BIT 7, B", 2, 8, 1, op_bit_7_b}, // 0xCB78
    {"BIT 7, C", 2, 8, 1, op_bit_7_c}, // 0xCB79
    {"BIT 7, D", 2, 8, 1, op_bit_7_d}, // 0xCB7A
    {"BIT 7, E", 2, 8, 1, op_bit_7_e}, // 0xCB7B
    {"BIT 7, H", 2, 8, 1, op_bit_7_h}, // 0xCB7C
    {"BIT 7, L", 2, 8, 1, op_bit_7_l}, // 0xCB7D
    {"BIT 7, (HL)", 2, 16, 1, op_bit_7_hlp}, // 0xCB7E
    {"BIT 7, A", 2, 8, 1, op_bit_7_a}, // 0xCB7F
    {"RES 0, B", 2, 8, 1, op_res_0_b}, // 0xCB80
    {"RES 0, C", 2, 8, 1, op_res_0_c}, // 0xCB81
    {"RES 0, D", 2, 8, 1, op_res_0_d}, // 0xCB82
    {"RES 0, E", 2, 8, 1, op_res_0_e}, // 0xCB83
    {"RES 0, H", 2, 8, 1, op_res_0_h}, // 0xCB84
    {"RES 0, L", 2, 8, 1, op_res_0_l}, // 0xCB85
    {"RES 0, (HL)", 2, 16, 1, op_res_0_hlp}, // 0xCB86
    {"RES 0, A", 2, 8, 1, op_res_0_a}, // 0xCB87
    {"RES 1, B", 2, 8, 1, op_res_1_b}, // 0xCB88
    {"RES 1, C", 2, 8, 1, op_res_1_c}, // 0xCB89
    {"RES 1, D", 2, 8, 1, op_res_1_d}, // 0xCB8A
    {"RES 1, E", 2, 8, 1, op_res_1_e}, // 0xCB8B
    {"RES 1, H", 2, 8, 1, op_res_1_h}, // 0xCB8C
    {"RES 1, L", 2, 8, 1, op_res_1_l}, // 0xCB8D
    {"RES 1, (HL)", 2, 16, 1, op_res_1_hlp}, // 0xCB8E
    {"RES 1, A", 2, 8, 1, op_res_1_a}, // 0xCB8F
    {"RES 2, B", 2, 8, 1, op_res_2_b}, // 0xCB90
    {"RES 2, C", 2, 8, 1, op_res_2_c}, // 0xCB91
    {"RES 2, D", 2, 8, 1, op_res_2_d}, // 0xCB92
    {"RES 2, E", 2, 8, 1, op_res_2_e}, // 0xCB93
    {"RES 2, H", 2, 8, 1, op_res_2_h}, // 0xCB94
    {"RES 2, L", 2, 8, 1, op_res_2_l}, // 0xCB95
    {"RES 2, (HL)", 2, 16, 1, op_res_2_hlp}, // 0xCB96
    {"RES 2, A", 2, 8, 1, op_res_2_a}, // 0xCB97
    {"RES 3, B", 2, 8, 1, op_res_3_b}, // 0xCB98
    {"RES 3, C", 2, 8, 1, op_res_3_c}, // 0xCB99
    {"RES 3, D", 2, 8, 1, op_res_3_d}, // 0xCB9A
    {"RES 3, E", 2, 8, 1, op_res_3_e}, // 0xCB9B
    {"RES 3, H", 2, 8, 1, op_res_3_h}, // 0xCB9C
    {"RES 3, L", 2, 8, 1, op_res_3_l}, // 0xCB9D
    {"RES 3, (HL)", 2, 16, 1, op_res_3_hlp}, // 0xCB9E
    {"RES 3, A", 2, 8, 1, op_res_3_a}, // 0xCB9F
    {"RES 4, B", 2, 8, 1, op_res_4_b}, // 0xCBA0
    {"RES 4, C", 2, 8, 1, op_res_4_c}, // 0xCBA1
    {"RES 4, D", 2, 8, 1, op_res_4_d}, // 0xCBA2
    {"RES 4, E", 2, 8, 1, op_res_4_e}, // 0xCBA3
    {"RES 4, H", 2, 8, 1, op_res_4_h}, // 0xCBA4
    {"RES 4, L", 2, 8, 1, op_res_4_l}, // 0xCBA5
    {"RES 4, (HL)", 2, 16, 1, op_res_4_hlp}, // 0xCBA6
    {"RES 4, A", 2, 8, 1, op_res_4_a}, // 0xCBA7
    {"RES 5, B", 2, 8, 1, op_res_5_b}, // 0xCBA8
    {"RES 5, C", 2, 8, 1, op_res_5_c}, // 0xCBA9
    {"RES 5, D", 2, 8, 1, op_res_5_d}, // 0xCBAA
    {"RES 5, E", 2, 8, 1, op_res_5_e}, // 0xCBAB
    {"RES 5, H", 2, 8, 1, op_res_5_h}, // 0xCBAC
    {"RES 5, L", 2, 8, 1, op_res_5_l}, // 0xCBAD
    {"RES 5, (HL)", 2, 16, 1, op_res_5_hlp}, // 0xCBAE
    {"RES 5, A", 2, 8, 1, op_res_5_a}, // 0xCBAF
    {"RES 6, B", 2, 8, 1, op_res_6_b}, // 0xCBB0
    {"RES 6, C", 2, 8, 1, op_res_6_c}, // 0xCBB1
    {"RES 6, D", 2, 8, 1, op_res_6_d}, // 0xCBB2
    {"RES 6, E", 2, 8, 1, op_res_6_e}, // 0xCBB3
    {"RES 6, H", 2, 8, 1, op_res_6_h}, // 0xCBB4
    {"RES 6, L", 2, 8, 1, op_res_6_l}, // 0xCBB5
    {"RES 6, (HL)", 2, 16, 1, op_res_6_hlp}, // 0xCBB6
    {"RES 6, A", 2, 8, 1, op_res_6_a}, // 0xCBB7
    {"RES 7, B", 2, 8, 1, op_res_7_b}, // 0xCBB8
    {"RES 7, C", 2, 8, 1, op_res_7_c}, // 0xCBB9
    {"RES 7, D", 2, 8, 1, op_res_7_d}, // 0xCBBA
    {"RES 7, E", 2, 8, 1, op_res_7_e}, // 0xCBBB
    {"RES 7, H", 2, 8, 1, op_res_7_h}, // 0xCBBC
    {"RES 7, L", 2, 8, 1, op_res_7_l}, // 0xCBBD
    {"RES 7, (HL)", 2, 16, 1, op_res_7_hlp}, // 0xCBBE
    {"RES 7, A", 2, 8, 1, op_res_7_a}, // 0xCBBF
    {"SET 0, B", 2, 8, 1, op_set_0_b}, // 0xCBC0
    {"SET 0, C", 2, 8, 1, op_set_0_c}, // 0xCBC1
    {"SET 0, D", 2, 8, 1, op_set_0_d}, // 0xCBC2
    {"SET 0, E", 2, 8, 1, op_set_0_e}, // 0xCBC3
    {"SET 0, H", 2, 8, 1, op_set_0_h}, // 0xCBC4
    {"SET 0, L", 2, 8, 1, op_set_0_l}, // 0xCBC5
    {"SET 0, (HL)", 2, 16, 1, op_set_0_hlp}, // 0xCBC6
    {"SET 0, A", 2, 8, 1, op_set_0_a}, // 0xCBC7
    {"SET 1, B", 2, 8, 1, op_set_1_b}, // 0xCBC8
    {"SET 1, C", 2, 8, 1, op_set_1_c}, // 0xCBC9
    {"SET 1, D", 2, 8, 1, op_set_1_d}, // 0xCBCA
    {"SET 1, E", 2, 8, 1, op_set_1_e}, // 0xCBCB
    {"SET 1, H", 2, 8, 1, op_set_1_h}, // 0xCBCC
    {"SET 1, L", 2, 8, 1, op_set_1_l}, // 0xCBCD
    {"SET 1, (HL)", 2, 16, 1, op_set_1_hlp}, // 0xCBCE
    {"SET 1, A", 2, 8, 1, op_set_1_a}, // 0xCBCF
    {"SET 2, B", 2, 8, 1, op_set_2_b}, // 0xCBD0
    {"SET 2, C", 2, 8, 1, op_set_2_c}, // 0xCBD1
    {"SET 2, D", 2, 8, 1, op_set_2_d}, // 0xCBD2
    {"SET 2, E", 2, 8, 1, op_set_2_e}, // 0xCBD3
    {"SET 2, H", 2, 8, 1, op_set_2_h}, // 0xCBD4
    {"SET 2, L", 2, 8, 1, op_set_2_l}, // 0xCBD5
    {"SET 2, (HL)", 2, 16, 1, op_set_2_hlp}, // 0xCBD6
    {"SET 2, A", 2, 8, 1, op_set_2_a}, // 0xCBD7
    {"SET 3, B", 2, 8, 1, op_set_3_b}, // 0xCBD8
    {"SET 3, C", 2, 8, 1, op_set_3_c}, // 0xCBD9
    {"SET 3, D", 2, 8, 1, op_set_3_d}, // 0xCBDA
    {"SET 3, E", 2, 8, 1, op_set_3_e}, // 0xCBDB
    {"SET 3, H", 2, 8, 1, op_set_3_h}, // 0xCBDC
    {"SET 3, L", 2, 8, 1, op_set_3_l}, // 0xCBDD
    {"SET 3, (HL)", 2, 16, 1, op_set_3_hlp}, // 0xCBDE
    {"SET 3, A", 2, 8, 1, op_set_3_a}, // 0xCBDF
    {"SET 4, B", 2, 8, 1, op_set_4_b}, // 0xCBE0
    {"SET 4, C", 2, 8, 1, op_set_4_c}, // 0xCBE1
    {"SET 4, D", 2, 8, 1, op_set_4_d}, // 0xCBE2
    {"SET 4, E", 2, 8, 1, op_set_4_e}, // 0xCBE3
    {"SET 4, H", 2, 8, 1, op_set_4_h}, // 0xCBE4
    {"SET 4, L", 2, 8, 1, op_set_4_l}, // 0xCBE5
    {"SET 4, (HL)", 2, 16, 1, op_set_4_hlp}, // 0xCBE6
    {"SET 4, A", 2, 8, 1, op_set_4_a}, // 0xCBE7
    {"SET 5, B", 2, 8, 1, op_set_5_b}, // 0xCBE8
    {"SET 5, C", 2, 8, 1, op_set_5_c}, // 0xCBE9
    {"SET 5, D", 2, 8, 1, op_set_5_d}, // 0xCBEA
    {"SET 5, E", 2, 8, 1, op_set_5_e}, // 0xCBEB
    {"SET 5, H", 2, 8, 1, op_set_5_h}, // 0xCBEC
    {"SET 5, L", 2, 8, 1, op_set_5_l}, // 0xCBED
    {"SET 5, (HL)", 2, 16, 1, op_set_5_hlp}, // 0xCBEE
    {"SET 5, A", 2, 8, 1, op_set_5_a}, // 0xCBEF
    {"SET 6, B", 2, 8, 1, op_set_6_b}, // 0xCBF0
    {"SET 6, C", 2, 8, 1, op_set_6_c}, // 0xCBF1
    {"SET 6, D", 2, 8, 1, op_set_6_d}, // 0xCBF2
    {"SET 6, E", 2, 8, 1, op_set_6_e}, // 0xCBF3
    {"SET 6, H", 2, 8, 1, op_set_6_h}, // 0xCBF4
    {"SET 6, L", 2, 8, 1, op_set_6_l}, // 0xCBF5
    {"SET 6, (HL)", 2, 16, 1, op_set_6_hlp}, // 0xCBF6
    {"SET 6, A", 2, 8, 1, op_set_6_a}, // 0xCBF7
    {"SET 7, B", 2, 8, 1, op_set_7_b}, // 0xCBF8
    {"SET 7, C", 2, 8, 1, op_set_7_c}, // 0xCBF9
    {"SET 7, D", 2, 8, 1, op_set_7_d}, // 0xCBFA
    {"SET 7, E", 2, 8, 1, op_set_7_e}, // 0xCBFB
    {"SET 7, H", 2, 8, 1, op_set_7_h}, // 0xCBFC
    {"SET 7, L", 2, 8, 1, op_set_7_l}, // 0xCBFD
    {"SET 7, (HL)", 2, 16, 1, op_set_7_hlp}, // 0xCBFE
    {"SET 7, A", 2, 8, 1, op_set_7_a} // 0xCBFF
};

struct gbc_instr find_instr(unsigned char opcode, struct gbc_system **gbc) {
    // If the opcode has the CB prefix
    if(opcode == 0xCB) {
        unsigned char next_opcode = read_byte(&(*gbc)->ram, (*gbc)->registers->PC + 1);
        return cb_instructions[next_opcode];
    } 
    // Standard instruction
    else {
        return instructions[opcode];
    }
}

void execute_instr(struct gbc_system **gbc) {

    // Get the opcode at the program counter
    unsigned char opcode = read_byte(&(*gbc)->ram, (*gbc)->registers->PC);

    // Get the instruction structure
    struct gbc_instr instruction = find_instr(opcode, gbc);

    // Create a pointer to the function to execute
    void (*opcode_function)();
    opcode_function = instruction.execute;

    // Execute the instruction based on the length of the operand
    char opcode_len = instruction.length - 1;
    unsigned short operand;

    switch(opcode_len) {

        case 0:
            opcode_function(gbc);
            break;

        case 1:
            operand = read_byte(&(*gbc)->ram, (*gbc)->registers->PC + 1);
            opcode_function(gbc, (unsigned char) operand);
            break;

        case 2:
            operand = read_short(&(*gbc)->ram, (*gbc)->registers->PC + 1);
            opcode_function(gbc, operand);
            break;
    }

    // If the instruction is not a jump, return, etc... Increment the PC
    if(instruction.increment_PC) {
        (*gbc)->registers->PC += instruction.length;
    }
}

void set_flag(char flag, unsigned char value, unsigned char *regis) {

    char offset;

    // Find the offset of the bit
    switch(flag) {
        case 'Z': offset = 7; break;
        case 'N': offset = 6; break;
        case 'H': offset = 5; break;
        case 'C': offset = 4; break;
        default: return;
    }

    // Set or clear the bit
    if(value == 0) {
        *regis &= ~(1 << offset);
    } else if(value == 1) {
        *regis |= 1 << offset;          
    }
}
