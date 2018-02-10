#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"

// Initialises the CPU registers
void init_cpu(gbc_cpu *cpu) {

    // Initialize the registers
    cpu->registers = malloc(sizeof(gbc_registers));

    // Default register values
    cpu->registers->AF = DEFAULT_AF;
    cpu->registers->BC = DEFAULT_BC;
    cpu->registers->DE = DEFAULT_DE;
    cpu->registers->HL = DEFAULT_HL;

    // Set the pointers to their default value
    cpu->registers->PC = DEFAULT_PC;
    cpu->registers->SP = DEFAULT_SP;

    // Enable interrupts
    cpu->registers->IME = 1;

    // Reset the clock
    cpu->clock = 0;
    cpu->run_for = 0;
}

/*
*   Common CPU operations
*/

static unsigned char and(const unsigned char a, const unsigned char b, unsigned char *flag) {
    
    unsigned char result = a & b;

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

static unsigned char or(const unsigned char a, const unsigned char b, unsigned char *flag) {

    unsigned char result = a | b;

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

static unsigned char xor(const unsigned char a, const unsigned char b, unsigned char *flag) {
    
    unsigned char result = a ^ b;

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

static unsigned char inc(const unsigned char operand, unsigned char *flag) {

    unsigned char result = operand + 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }
    
    set_flag(FLAG_SUBTRACT, 0, flag);

    if(operand & 0xF == 0xF) {
        set_flag(FLAG_HALFCARRY, 1, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag);
    } 

    return result;
}

static unsigned char dec(const unsigned char operand, unsigned char *flag) {

    unsigned char result = operand - 1;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    if(operand & 0x0f) {
        set_flag(FLAG_HALFCARRY, 0, flag);
    } else {
        set_flag(FLAG_HALFCARRY, 1, flag);
    }

    return result;
}

static unsigned char add_byte(const unsigned char a, const unsigned char b, unsigned char *flag) {
    
    unsigned short result = a + b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);

    // Check for a carry from bit 3
    if(((a & 0xF) + (b & 0xF)) > 0xF) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    // Check for carry
    if((result & 0xFF00) > 0) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }
    
    return result & 0xFF;
}

static unsigned char sub_byte(const unsigned char a, const unsigned char b, unsigned char *flag) {

    unsigned short result = a - b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag); 
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 1, flag);

    if((a & 0xF) > (b & 0xF)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    if((result & 0xFF00) > 0) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result & 0xFF;
}

static unsigned short add_short(const unsigned short a, const unsigned short b, unsigned char *flag) {
    
    unsigned int result = a + b;

    if(result == 0) {
        set_flag(FLAG_ZERO, 1, flag);
    } else {
        set_flag(FLAG_ZERO, 0, flag); 
    }

    set_flag(FLAG_SUBTRACT, 0, flag);

    if((a & 0xF) > (b & 0xF)) {
        set_flag(FLAG_HALFCARRY, 1, flag); 
    } else {
        set_flag(FLAG_HALFCARRY, 0, flag); 
    }

    if((result & 0xFFFF0000) > 0) {
        set_flag(FLAG_CARRY, 1, flag);
    } else {
        set_flag(FLAG_CARRY, 0, flag); 
    }

    return result & 0xFFFF;
}

static unsigned char rotate_left(const unsigned char operand, unsigned char *flag) {

}

static unsigned char rotate_right(const unsigned char operand, unsigned char *flag) {

}

static unsigned char rotate_left_carry(const unsigned char operand, unsigned char *flag) {

}

static unsigned char rotate_right_carry(const unsigned char operand, unsigned char *flag) {

}

static unsigned char shift_left_arith(const unsigned char operand, unsigned char *flag) {

}

static unsigned char shift_right_arith(const unsigned char operand, unsigned char *flag) {

}

static unsigned char shift_right_logic(const unsigned char operand, unsigned char *flag) {

}

static unsigned char swap(const unsigned char operand, unsigned char *flag) {

}

static void test_bit(const unsigned char regis, const unsigned char bit, unsigned char *flag) {

}

static unsigned char reset_bit(const unsigned char regis, const unsigned char bit, unsigned char *flag) {

}

static unsigned char set_bit(const unsigned char regis, const unsigned char bit, unsigned char *flag) {

}

/*
*   CPU instructions
*/

// 0x00: NOP (- - - -)
static void op_nop(gbc_system *gbc) {
    // Do nothing
}

// 0x01: LD BC, d16 (- - - -)
static void op_ld_bc_d16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->BC = operand;
}

// 0x02: LD (BC), A (- - - -)
static void op_ld_bcp_a(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->BC, gbc->cpu->registers->A);
}

// 0x03: INC BC (- - - -)
static void op_inc_bc(gbc_system *gbc) {
    gbc->cpu->registers->BC++;
}

// 0x04: INC B (Z 0 H -)
static void op_inc_b(gbc_system *gbc) {
    gbc->cpu->registers->B = inc(gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0x05: DEC B (Z 1 H -)
static void op_dec_b(gbc_system *gbc) {
    gbc->cpu->registers->B = dec(gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0x06: LD B, d8 (- - - -)
static void op_ld_b_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->B = operand;
}

// 0x07: RLCA (0 0 0 C)
static void op_rlca(gbc_system *gbc) {
    printf("Unimplemented Instruction: RLCA\n");
}

// 0x08: LD (a16), SP (- - - -)
static void op_ld_a16p_sp(gbc_system *gbc, unsigned short operand) {
    write_byte(gbc->ram, operand, gbc->cpu->registers->SP);
}

// 0x09: ADD HL, BC (- 0 H C)
static void op_add_hl_bc(gbc_system *gbc) {
    gbc->cpu->registers->HL = add_short(gbc->cpu->registers->HL, gbc->cpu->registers->BC, &gbc->cpu->registers->F);
}

// 0x0A: LD A, (BC) (- - - -)
static void op_ld_a_bcp(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(gbc->ram, gbc->cpu->registers->BC);
}

// 0x0B: DEC BC (- - - -)
static void op_dec_bc(gbc_system *gbc) {
    gbc->cpu->registers->BC--;
}

// 0x0C: INC C (Z 0 H -)
static void op_inc_c(gbc_system *gbc) {
    gbc->cpu->registers->C = inc(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0x0D: DEC C (Z 1 H -)
static void op_dec_c(gbc_system *gbc) {
    gbc->cpu->registers->C = dec(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0x0E: LD C, d8 (- - - -)
static void op_ld_c_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->C = operand;
}

// 0x0F: RRCA (0 0 0 C)
static void op_rrca(gbc_system *gbc) {
    printf("Unimplemented Instruction: RRCA\n");
}

// 0x10: STOP 0 (- - - -)
static void op_stop_0(gbc_system *gbc, unsigned char operand) {
    printf("Unimplemented Instruction: STOP 0\n");
}

// 0x11: LD DE, d16 (- - - -)
static void op_ld_de_d16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->DE = operand;
}

// 0x12: LD (DE), A (- - - -)
static void op_ld_dep_a(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->DE, gbc->cpu->registers->A);
}

// 0x13: INC DE (- - - -)
static void op_inc_de(gbc_system *gbc) {
    gbc->cpu->registers->DE++;
}

// 0x14: INC D (Z 0 H -)
static void op_inc_d(gbc_system *gbc) {
    gbc->cpu->registers->D = inc(gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0x15: DEC D (Z 1 H -)
static void op_dec_d(gbc_system *gbc) {
    gbc->cpu->registers->D = dec(gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0x16: LD D, d8 (- - - -)
static void op_ld_d_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->D = operand;
}

// 0x17: RLA (0 0 0 C)
static void op_rla(gbc_system *gbc) {
    printf("Unimplemented Instruction: RLA\n");
}

// 0x18: JR r8 (- - - -)
static void op_jr_r8(gbc_system *gbc, char operand) {
    gbc->cpu->registers->PC += operand;
}

// 0x19: ADD HL, DE (- 0 H C)
static void op_add_hl_de(gbc_system *gbc) {
    gbc->cpu->registers->HL = add_short(gbc->cpu->registers->HL, gbc->cpu->registers->DE, &gbc->cpu->registers->F);
}

// 0x1A: LD A, (DE) (- - - -)
static void op_ld_a_dep(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(gbc->ram, gbc->cpu->registers->DE);
}

// 0x1B: DEC DE (- - - -)
static void op_dec_de(gbc_system *gbc) {
    gbc->cpu->registers->DE--;
}

// 0x1C: INC E (Z 0 H -)
static void op_inc_e(gbc_system *gbc) {
    gbc->cpu->registers->E = inc(gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0x1D: DEC E (Z 1 H -)
static void op_dec_e(gbc_system *gbc) {
    gbc->cpu->registers->E = dec(gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0x1E: LD E, d8 (- - - -)
static void op_ld_e_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->E = operand;
}

// 0x1F: RRA (0 0 0 C)
static void op_rra(gbc_system *gbc) {
    printf("Unimplemented Instruction: RRA\n");
}

// 0x20: JR NZ, r8 (- - - -)
static void op_jr_nz_r8(gbc_system *gbc, char operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 0) {
        gbc->cpu->registers->PC += operand;
    }
}

// 0x21: LD HL, d16 (- - - -)
static void op_ld_hl_d16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->HL = operand;
}

// 0x22: LD (HL+), A (- - - -)
static void op_ld_hlpp_a(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, gbc->cpu->registers->A);
    gbc->cpu->registers->HL++;
}

// 0x23: INC HL (- - - -)
static void op_inc_hl(gbc_system *gbc) {
    gbc->cpu->registers->HL++;
}

// 0x24: INC H (Z 0 H -)
static void op_inc_h(gbc_system *gbc) {
    gbc->cpu->registers->H = inc(gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0x25: DEC H (Z 1 H -)
static void op_dec_h(gbc_system *gbc) {
    gbc->cpu->registers->H = dec(gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0x26: LD H, d8 (- - - -)
static void op_ld_h_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->H = operand;
}

// 0x27: DAA (Z - 0 C)
static void op_daa(gbc_system *gbc) {
    printf("Unimplemented Instruction: DAA\n");
}

// 0x28: JR Z, r8 (- - - -)
static void op_jr_z_r8(gbc_system *gbc, char operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F) == 1) {
        gbc->cpu->registers->PC += operand;
    }
}

// 0x29: ADD HL, HL (- 0 H C)
static void op_add_hl_hl(gbc_system *gbc) {
    gbc->cpu->registers->HL = add_short(gbc->cpu->registers->HL, gbc->cpu->registers->HL, &gbc->cpu->registers->F);
}

// 0x2A: LD A, (HL+) (- - - -)
static void op_ld_a_hlpp(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(gbc->ram, gbc->cpu->registers->HL);
    gbc->cpu->registers->HL++;
}

// 0x2B: DEC HL (- - - -)
static void op_dec_hl(gbc_system *gbc) {
    gbc->cpu->registers->HL--;
}

// 0x2C: INC L (Z 0 H -)
static void op_inc_l(gbc_system *gbc) {
    gbc->cpu->registers->L = inc(gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0x2D: DEC L (Z 1 H -)
static void op_dec_l(gbc_system *gbc) {
    gbc->cpu->registers->L = dec(gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0x2E: LD L, d8 (- - - -)
static void op_ld_l_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->L = operand;
}

// 0x2F: CPL (- 1 1 -)
static void op_cpl(gbc_system *gbc) {
    gbc->cpu->registers->A = ~(gbc->cpu->registers->A);
    set_flag(FLAG_SUBTRACT, 1, &gbc->cpu->registers->F);
    set_flag(FLAG_HALFCARRY, 1, &gbc->cpu->registers->F);
}

// 0x30: JR NC, r8 (- - - -)
static void op_jr_nc_r8(gbc_system *gbc, char operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 0) {
        gbc->cpu->registers->PC += operand;
    }
}

// 0x31: LD SP, d16 (- - - -)
static void op_ld_sp_d16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->SP = operand;
}

// 0x32: LD (HL-), A (- - - -)
static void op_ld_hlmp_a(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, gbc->cpu->registers->A);
    gbc->cpu->registers->HL--;
}

// 0x33: INC SP (- - - -)
static void op_inc_sp(gbc_system *gbc) {
    gbc->cpu->registers->SP++;
}

// 0x34: INC (HL) (Z 0 H -)
static void op_inc_hlp(gbc_system *gbc) {
    write_byte(gbc->ram, 
               gbc->cpu->registers->HL, 
               inc(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F));
}

// 0x35: DEC (HL) (Z 1 H -)
static void op_dec_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               dec(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F));
}

// 0x36: LD (HL), d8 (- - - -)
static void op_ld_hlp_d8(gbc_system *gbc, unsigned char operand) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, operand);
}

// 0x37: SCF (- 0 0 1)
static void op_scf(gbc_system *gbc) {
    set_flag(FLAG_SUBTRACT, 0, &gbc->cpu->registers->F);
    set_flag(FLAG_HALFCARRY, 0, &gbc->cpu->registers->F);
    set_flag(FLAG_CARRY, 1, &gbc->cpu->registers->F);
}

// 0x38: JR C, r8 (- - - -)
static void op_jr_c_r8(gbc_system *gbc, char operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F) == 1) {
        gbc->cpu->registers->F += operand;
    }
}

// 0x39: ADD HL, SP (- 0 H C)
static void op_add_hl_sp(gbc_system *gbc) {
    gbc->cpu->registers->HL = add_short(gbc->cpu->registers->HL, gbc->cpu->registers->SP, &gbc->cpu->registers->F);
}

// 0x3A: LD A, (HL-) (- - - -)
static void op_ld_a_hlmp(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(gbc->ram, gbc->cpu->registers->HL);
    gbc->cpu->registers->HL--;
}

// 0x3B: DEC SP (- - - -)
static void op_dec_sp(gbc_system *gbc) {
    gbc->cpu->registers->SP--;
}

// 0x3C: INC A (Z 0 H -)
static void op_inc_a(gbc_system *gbc) {
    gbc->cpu->registers->A = inc(gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0x3D: DEC A (Z 1 H -)
static void op_dec_a(gbc_system *gbc) {
    gbc->cpu->registers->A = dec(gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0x3E: LD A, d8 (- - - -)
static void op_ld_a_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = operand;
}

// 0x3F: CCF (- 0 0 C)
static void op_ccf(gbc_system *gbc) {
    set_flag(FLAG_SUBTRACT, 0, &gbc->cpu->registers->F);
    set_flag(FLAG_HALFCARRY, 0, &gbc->cpu->registers->F);
    set_flag(FLAG_CARRY, !get_flag(FLAG_CARRY, gbc->cpu->registers->F), &gbc->cpu->registers->F);
}

// 0x40: LD B, B (- - - -)
static void op_ld_b_b(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->B;
}

// 0x41: LD B, C (- - - -)
static void op_ld_b_c(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->C;
}

// 0x42: LD B, D (- - - -)
static void op_ld_b_d(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->D;
}

// 0x43: LD B, E (- - - -)
static void op_ld_b_e(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->E;
}

// 0x44: LD B, H (- - - -)
static void op_ld_b_h(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->H;
}

// 0x45: LD B, L (- - - -)
static void op_ld_b_l(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->L;
}

// 0x46: LD B, (HL) (- - - -)
static void op_ld_b_hlp(gbc_system *gbc) {
    gbc->cpu->registers->B = read_byte(gbc->ram, gbc->cpu->registers->HL);
}

// 0x47: LD B, A (- - - -)
static void op_ld_b_a(gbc_system *gbc) {
    gbc->cpu->registers->B = gbc->cpu->registers->A;
}

// 0x48: LD C, B (- - - -)
static void op_ld_c_b(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->B;
}

// 0x49: LD C, C (- - - -)
static void op_ld_c_c(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->C;
}

// 0x4A: LD C, D (- - - -)
static void op_ld_c_d(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->D;
}

// 0x4B: LD C, E (- - - -)
static void op_ld_c_e(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->E;
}

// 0x4C: LD C, H (- - - -)
static void op_ld_c_h(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->H;
}

// 0x4D: LD C, L (- - - -)
static void op_ld_c_l(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->L;
}

// 0x4E: LD C, (HL) (- - - -)
static void op_ld_c_hlp(gbc_system *gbc) {
    gbc->cpu->registers->C = read_byte(gbc->ram, gbc->cpu->registers->HL);
}

// 0x4F: LD C, A (- - - -)
static void op_ld_c_a(gbc_system *gbc) {
    gbc->cpu->registers->C = gbc->cpu->registers->A;
}

// 0x50: LD D, B (- - - -)
static void op_ld_d_b(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->B;
}

// 0x51: LD D, C (- - - -)
static void op_ld_d_c(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->C;
}

// 0x52: LD D, D (- - - -)
static void op_ld_d_d(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->D;
}

// 0x53: LD D, E (- - - -)
static void op_ld_d_e(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->E;
}

// 0x54: LD D, H (- - - -)
static void op_ld_d_h(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->H;
}

// 0x55: LD D, L (- - - -)
static void op_ld_d_l(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->L;
}

// 0x56: LD D, (HL) (- - - -)
static void op_ld_d_hlp(gbc_system *gbc) {
    gbc->cpu->registers->D = read_byte(gbc->ram, gbc->cpu->registers->HL);
}

// 0x57: LD D, A (- - - -)
static void op_ld_d_a(gbc_system *gbc) {
    gbc->cpu->registers->D = gbc->cpu->registers->A;
}

// 0x58: LD E, B (- - - -)
static void op_ld_e_b(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->B;
}

// 0x59: LD E, C (- - - -)
static void op_ld_e_c(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->C;
}

// 0x5A: LD E, D (- - - -)
static void op_ld_e_d(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->D;
}

// 0x5B: LD E, E (- - - -)
static void op_ld_e_e(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->E;
}

// 0x5C: LD E, H (- - - -)
static void op_ld_e_h(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->H;
}

// 0x5D: LD E, L (- - - -)
static void op_ld_e_l(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->L;
}

// 0x5E: LD E, (HL) (- - - -)
static void op_ld_e_hlp(gbc_system *gbc) {
    gbc->cpu->registers->E = read_byte(gbc->ram, gbc->cpu->registers->HL);
}

// 0x5F: LD E, A (- - - -)
static void op_ld_e_a(gbc_system *gbc) {
    gbc->cpu->registers->E = gbc->cpu->registers->A;
}

// 0x60: LD H, B (- - - -)
static void op_ld_h_b(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->B;
}

// 0x61: LD H, C (- - - -)
static void op_ld_h_c(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->C;
}

// 0x62: LD H, D (- - - -)
static void op_ld_h_d(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->D;
}

// 0x63: LD H, E (- - - -)
static void op_ld_h_e(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->E;
}

// 0x64: LD H, H (- - - -)
static void op_ld_h_h(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->H;
}

// 0x65: LD H, L (- - - -)
static void op_ld_h_l(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->L;
}

// 0x66: LD H, (HL) (- - - -)
static void op_ld_h_hlp(gbc_system *gbc) {
    gbc->cpu->registers->H = read_byte(gbc->ram, gbc->cpu->registers->HL);
}

// 0x67: LD H, A (- - - -)
static void op_ld_h_a(gbc_system *gbc) {
    gbc->cpu->registers->H = gbc->cpu->registers->A;
}

// 0x68: LD L, B (- - - -)
static void op_ld_l_b(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->B;
}

// 0x69: LD L, C (- - - -)
static void op_ld_l_c(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->C;
}

// 0x6A: LD L, D (- - - -)
static void op_ld_l_d(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->D;
}

// 0x6B: LD L, E (- - - -)
static void op_ld_l_e(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->E;
}

// 0x6C: LD L, H (- - - -)
static void op_ld_l_h(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->H;
}

// 0x6D: LD L, L (- - - -)
static void op_ld_l_l(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->L;
}

// 0x6E: LD L, (HL) (- - - -)
static void op_ld_l_hlp(gbc_system *gbc) {
    gbc->cpu->registers->L = read_byte(gbc->ram, gbc->cpu->registers->HL);
}

// 0x6F: LD L, A (- - - -)
static void op_ld_l_a(gbc_system *gbc) {
    gbc->cpu->registers->L = gbc->cpu->registers->A;
}

// 0x70: LD (HL), B (- - - -)
static void op_ld_hlp_b(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, gbc->cpu->registers->B);
}

// 0x71: LD (HL), C (- - - -)
static void op_ld_hlp_c(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, gbc->cpu->registers->C);
}

// 0x72: LD (HL), D (- - - -)
static void op_ld_hlp_d(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, gbc->cpu->registers->D);
}

// 0x73: LD (HL), E (- - - -)
static void op_ld_hlp_e(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, gbc->cpu->registers->E);
}

// 0x74: LD (HL), H (- - - -)
static void op_ld_hlp_h(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, gbc->cpu->registers->H);
}

// 0x75: LD (HL), L (- - - -)
static void op_ld_hlp_l(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, gbc->cpu->registers->L);
}

// 0x76: HALT (- - - -)
static void op_halt(gbc_system *gbc) {
    printf("Unimplemented Instruction: HALT\n");
}

// 0x77: LD (HL), A (- - - -)
static void op_ld_hlp_a(gbc_system *gbc) {
    write_byte(gbc->ram, gbc->cpu->registers->HL, gbc->cpu->registers->A);
}

// 0x78: LD A, B (- - - -)
static void op_ld_a_b(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->B;
}

// 0x79: LD A, C (- - - -)
static void op_ld_a_c(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->C;
}

// 0x7A: LD A, D (- - - -)
static void op_ld_a_d(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->D;
}

// 0x7B: LD A, E (- - - -)
static void op_ld_a_e(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->E;
}

// 0x7C: LD A, H (- - - -)
static void op_ld_a_h(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->H;
}

// 0x7D: LD A, L (- - - -)
static void op_ld_a_l(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->L;
}

// 0x7E: LD A, (HL) (- - - -)
static void op_ld_a_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = read_byte(gbc->ram, gbc->cpu->registers->HL);
}

// 0x7F: LD A, A (- - - -)
static void op_ld_a_a(gbc_system *gbc) {
    gbc->cpu->registers->A = gbc->cpu->registers->A;
}

// 0x80: ADD A, B (Z 0 H C)
static void op_add_a_b(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0x81: ADD A, C (Z 0 H C)
static void op_add_a_c(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0x82: ADD A, D (Z 0 H C)
static void op_add_a_d(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0x83: ADD A, E (Z 0 H C)
static void op_add_a_e(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0x84: ADD A, H (Z 0 H C)
static void op_add_a_h(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0x85: ADD A, L (Z 0 H C)
static void op_add_a_l(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0x86: ADD A, (HL) (Z 0 H C)
static void op_add_a_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F);
}

// 0x87: ADD A, A (Z 0 H C)
static void op_add_a_a(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0x88: ADC A, B (Z 0 H C)
static void op_adc_a_b(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->B + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x89: ADC A, C (Z 0 H C)
static void op_adc_a_c(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->C + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x8A: ADC A, D (Z 0 H C)
static void op_adc_a_d(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->D + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x8B: ADC A, E (Z 0 H C)
static void op_adc_a_e(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->E + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x8C: ADC A, H (Z 0 H C)
static void op_adc_a_h(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->H + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x8D: ADC A, L (Z 0 H C)
static void op_adc_a_l(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->L + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x8E: ADC A, (HL) (Z 0 H C)
static void op_adc_a_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, 
                                 read_byte(gbc->ram, gbc->cpu->registers->HL) + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x8F: ADC A, A (Z 0 H C)
static void op_adc_a_a(gbc_system *gbc) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->A + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x90: SUB B (Z 1 H C)
static void op_sub_b(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0x91: SUB C (Z 1 H C)
static void op_sub_c(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0x92: SUB D (Z 1 H C)
static void op_sub_d(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0x93: SUB E (Z 1 H C)
static void op_sub_e(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0x94: SUB H (Z 1 H C)
static void op_sub_h(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0x95: SUB L (Z 1 H C)
static void op_sub_l(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0x96: SUB (HL) (Z 1 H C)
static void op_sub_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F);
}

// 0x97: SUB A (Z 1 H C)
static void op_sub_a(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0x98: SBC A, B (Z 1 H C)
static void op_sbc_a_b(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->B + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x99: SBC A, C (Z 1 H C)
static void op_sbc_a_c(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->C + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x9A: SBC A, D (Z 1 H C)
static void op_sbc_a_d(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->D + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x9B: SBC A, E (Z 1 H C)
static void op_sbc_a_e(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->E + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x9C: SBC A, H (Z 1 H C)
static void op_sbc_a_h(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->H + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x9D: SBC A, L (Z 1 H C)
static void op_sbc_a_l(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->L + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x9E: SBC A, (HL) (Z 1 H C)
static void op_sbc_a_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, 
                                 read_byte(gbc->ram, gbc->cpu->registers->HL) + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0x9F: SBC A, A (Z 1 H C)
static void op_sbc_a_a(gbc_system *gbc) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, 
                                 gbc->cpu->registers->A + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0xA0: AND B (Z 0 1 0)
static void op_and_b(gbc_system *gbc) {
    gbc->cpu->registers->A = and(gbc->cpu->registers->A, gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xA1: AND C (Z 0 1 0)
static void op_and_c(gbc_system *gbc) {
    gbc->cpu->registers->A = and(gbc->cpu->registers->A, gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xA2: AND D (Z 0 1 0)
static void op_and_d(gbc_system *gbc) {
    gbc->cpu->registers->A = and(gbc->cpu->registers->A, gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xA3: AND E (Z 0 1 0)
static void op_and_e(gbc_system *gbc) {
    gbc->cpu->registers->A = and(gbc->cpu->registers->A, gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xA4: AND H (Z 0 1 0)
static void op_and_h(gbc_system *gbc) {
    gbc->cpu->registers->A = and(gbc->cpu->registers->A, gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xA5: AND L (Z 0 1 0)
static void op_and_l(gbc_system *gbc) {
    gbc->cpu->registers->A = and(gbc->cpu->registers->A, gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xA6: AND (HL) (Z 0 1 0)
static void op_and_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = and(gbc->cpu->registers->A, read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F);
}

// 0xA7: AND A (Z 0 1 0)
static void op_and_a(gbc_system *gbc) {
    gbc->cpu->registers->A = and(gbc->cpu->registers->A, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xA8: XOR B (Z 0 0 0)
static void op_xor_b(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(gbc->cpu->registers->B, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xA9: XOR C (Z 0 0 0)
static void op_xor_c(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(gbc->cpu->registers->C, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xAA: XOR D (Z 0 0 0)
static void op_xor_d(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(gbc->cpu->registers->D, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xAB: XOR E (Z 0 0 0)
static void op_xor_e(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(gbc->cpu->registers->E, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xAC: XOR H (Z 0 0 0)
static void op_xor_h(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(gbc->cpu->registers->H, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xAD: XOR L (Z 0 0 0)
static void op_xor_l(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(gbc->cpu->registers->L, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xAE: XOR (HL) (Z 0 0 0)
static void op_xor_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = xor(read_byte(gbc->ram, gbc->cpu->registers->HL),
                               gbc->cpu->registers->A, 
                               &gbc->cpu->registers->F);
}

// 0xAF: XOR A (Z 0 0 0)
static void op_xor_a(gbc_system *gbc) { 
    gbc->cpu->registers->A = xor(gbc->cpu->registers->A, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xB0: OR B (Z 0 0 0)
static void op_or_b(gbc_system *gbc) {
    gbc->cpu->registers->A = or(gbc->cpu->registers->A, gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xB1: OR C (Z 0 0 0)
static void op_or_c(gbc_system *gbc) {
    gbc->cpu->registers->A = or(gbc->cpu->registers->A, gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xB2: OR D (Z 0 0 0)
static void op_or_d(gbc_system *gbc) {
    gbc->cpu->registers->A = or(gbc->cpu->registers->A, gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xB3: OR E (Z 0 0 0)
static void op_or_e(gbc_system *gbc) {
    gbc->cpu->registers->A = or(gbc->cpu->registers->A, gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xB4: OR H (Z 0 0 0)
static void op_or_h(gbc_system *gbc) {
    gbc->cpu->registers->A = or(gbc->cpu->registers->A, gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xB5: OR L (Z 0 0 0)
static void op_or_l(gbc_system *gbc) {
    gbc->cpu->registers->A = or(gbc->cpu->registers->A, gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xB6: OR (HL) (Z 0 0 0)
static void op_or_hlp(gbc_system *gbc) {
    gbc->cpu->registers->A = or(gbc->cpu->registers->A, read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F);
}

// 0xB7: OR A (Z 0 0 0)
static void op_or_a(gbc_system *gbc) {
    gbc->cpu->registers->A = or(gbc->cpu->registers->A, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xB8: CP B (Z 1 H C)
static void op_cp_b(gbc_system *gbc) {
    sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xB9: CP C (Z 1 H C)
static void op_cp_c(gbc_system *gbc) {
    sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xBA: CP D (Z 1 H C)
static void op_cp_d(gbc_system *gbc) {
    sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xBB: CP E (Z 1 H C)
static void op_cp_e(gbc_system *gbc) {
    sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xBC: CP H (Z 1 H C)
static void op_cp_h(gbc_system *gbc) {
    sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xBD: CP L (Z 1 H C)
static void op_cp_l(gbc_system *gbc) {
    sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xBE: CP (HL) (Z 1 H C)
static void op_cp_hlp(gbc_system *gbc) {
    sub_byte(gbc->cpu->registers->A,
            read_byte(gbc->ram, gbc->cpu->registers->HL),
            &gbc->cpu->registers->F);
}

// 0xBF: CP A (Z 1 H C)
static void op_cp_a(gbc_system *gbc) {
    sub_byte(gbc->cpu->registers->A, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xC0: RET NZ (- - - -)
static void op_ret_nz(gbc_system *gbc) {
    if(!get_flag(FLAG_ZERO, gbc->cpu->registers->F)) {
        op_ret(gbc); 
    } 
}

// 0xC1: POP BC (- - - -)
static void op_pop_bc(gbc_system *gbc) {
    gbc->cpu->registers->BC = stack_pop(gbc->ram, &gbc->cpu->registers->SP) | (stack_pop(gbc->ram, &gbc->cpu->registers->SP) << 8);
}

// 0xC2: JP NZ, a16 (- - - -)
static void op_jp_nz_a16(gbc_system *gbc, unsigned short operand) {
    if(!get_flag(FLAG_ZERO, gbc->cpu->registers->F)) {
        gbc->cpu->registers->PC = operand; 
    } 
}

// 0xC3: JP a16 (- - - -)
static void op_jp_a16(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->PC = operand;
}

// 0xC4: CALL NZ, a16 (- - - -)
static void op_call_nz_a16(gbc_system *gbc, unsigned short operand) {
    if(!get_flag(FLAG_ZERO, gbc->cpu->registers->F)) {
        op_call_a16(gbc, operand); 
    } 
}

// 0xC5: PUSH BC (- - - -)
static void op_push_bc(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->B, gbc->ram, &gbc->cpu->registers->SP);
    stack_push(gbc->cpu->registers->C, gbc->ram, &gbc->cpu->registers->SP);
}

// 0xC6: ADD A, d8 (Z 0 H C)
static void op_add_a_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A, operand, &gbc->cpu->registers->F);
}

// 0xC7: RST 00H (- - - -)
static void op_rst_00h(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->PC, gbc->ram, &gbc->cpu->registers->SP);
    gbc->cpu->registers->PC = 0x0;
}

// 0xC8: RET Z (- - - -)
static void op_ret_z(gbc_system *gbc) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F)) {
        op_ret(gbc); 
    } 
}

// 0xC9: RET (- - - -)
static void op_ret(gbc_system *gbc) {
    gbc->cpu->registers->PC = stack_pop(gbc->ram, &gbc->cpu->registers->SP) | (stack_pop(gbc->ram, &gbc->cpu->registers->SP) << 8);
}

// 0xCA: JP Z, a16 (- - - -)
static void op_jp_z_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F)) {
        gbc->cpu->registers->PC = operand; 
    } 
}

// 0xCB: PREFIX CB (- - - -)
static void op_prefix_cb(gbc_system *gbc) {
    // Do nothing, placeholder function
}

// 0xCC: CALL Z, a16 (- - - -)
static void op_call_z_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_ZERO, gbc->cpu->registers->F)) {
        op_call_a16(gbc, operand); 
    } 
}

// 0xCD: CALL a16 (- - - -)
static void op_call_a16(gbc_system *gbc, unsigned short operand) {
    stack_push(((gbc->cpu->registers->PC + 3 * sizeof(char)) & 0xFF00) >> 8, gbc->ram, &gbc->cpu->registers->SP);
    stack_push(((gbc->cpu->registers->PC + 3 * sizeof(char)) & 0x00FF), gbc->ram, &gbc->cpu->registers->SP);
    gbc->cpu->registers->PC = operand;
}

// 0xCE: ADC A, d8 (Z 0 H C)
static void op_adc_a_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = add_byte(gbc->cpu->registers->A,
                                 operand + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0xCF: RST 08H (- - - -)
static void op_rst_08h(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->PC, gbc->ram, &gbc->cpu->registers->SP);
    gbc->cpu->registers->PC = 0x8;
}

// 0xD0: RET NC (- - - -)
static void op_ret_nc(gbc_system *gbc) {
    if(!get_flag(FLAG_CARRY, gbc->cpu->registers->F)) {
        op_ret(gbc); 
    }
}

// 0xD1: POP DE (- - - -)
static void op_pop_de(gbc_system *gbc) {
    gbc->cpu->registers->DE = stack_pop(gbc->ram, &gbc->cpu->registers->SP) | (stack_pop(gbc->ram, &gbc->cpu->registers->SP) << 8);
}

// 0xD2: JP NC, a16 (- - - -)
static void op_jp_nc_a16(gbc_system *gbc, unsigned short operand) {
    if(!get_flag(FLAG_CARRY, gbc->cpu->registers->F)) {
        gbc->cpu->registers->PC = operand; 
    }
}

// 0xD4: CALL NC, a16 (- - - -)
static void op_call_nc_a16(gbc_system *gbc, unsigned short operand) {
    if(!get_flag(FLAG_CARRY, gbc->cpu->registers->F)) {
        op_call_a16(gbc, operand); 
    }
}

// 0xD5: PUSH DE (- - - -)
static void op_push_de(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->D, gbc->ram, &gbc->cpu->registers->SP);
    stack_push(gbc->cpu->registers->E, gbc->ram, &gbc->cpu->registers->SP);
}

// 0xD6: SUB d8 (Z 1 H C)
static void op_sub_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A, operand, &gbc->cpu->registers->F);
}

// 0xD7: RST 10H (- - - -)
static void op_rst_10h(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->PC, gbc->ram, &gbc->cpu->registers->SP);
    gbc->cpu->registers->PC = 0x10;
}

// 0xD8: RET C (- - - -)
static void op_ret_c(gbc_system *gbc) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F)) {
        op_ret(gbc); 
    }
}

// 0xD9: RETI (- - - -)
static void op_reti(gbc_system *gbc) {
    op_ret(gbc);
    op_ei(gbc);
}

// 0xDA: JP C, a16 (- - - -)
static void op_jp_c_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F)) {
        gbc->cpu->registers->PC = operand; 
    }
}

// 0xDC: CALL C, a16 (- - - -)
static void op_call_c_a16(gbc_system *gbc, unsigned short operand) {
    if(get_flag(FLAG_CARRY, gbc->cpu->registers->F)) {
        op_call_a16(gbc, operand); 
    }
}

// 0xDE: SBC A, d8 (Z 1 H C)
static void op_sbc_a_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = sub_byte(gbc->cpu->registers->A,
                                 operand + get_flag(FLAG_CARRY, gbc->cpu->registers->F),
                                 &gbc->cpu->registers->F);
}

// 0xDF: RST 18H (- - - -)
static void op_rst_18h(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->PC, gbc->ram, &gbc->cpu->registers->SP);
    gbc->cpu->registers->PC = 0x18;
}

// 0xE0: LDH (a8), A (- - - -)
static void op_ldh_a8p_a(gbc_system *gbc, unsigned char operand) {
    write_byte(gbc->ram, 0xFF00 + operand, gbc->cpu->registers->A);
}

// 0xE1: POP HL (- - - -)
static void op_pop_hl(gbc_system *gbc) {
    gbc->cpu->registers->HL = stack_pop(gbc->ram, &gbc->cpu->registers->SP) | (stack_pop(gbc->ram, &gbc->cpu->registers->SP) << 8);
}

// 0xE2: LD (C), A (- - - -)
static void op_ld_cp_a(gbc_system *gbc, unsigned char operand) {
    write_byte(gbc->ram, 0xFF00 + gbc->cpu->registers->C, gbc->cpu->registers->A);
}

// 0xE5: PUSH HL (- - - -)
static void op_push_hl(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->H, gbc->ram, &gbc->cpu->registers->SP);
    stack_push(gbc->cpu->registers->L, gbc->ram, &gbc->cpu->registers->SP);
}

// 0xE6: AND d8 (Z 0 1 0)
static void op_and_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = and(gbc->cpu->registers->A, operand, &gbc->cpu->registers->F);
}

// 0xE7: RST 20H (- - - -)
static void op_rst_20h(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->PC, gbc->ram, &gbc->cpu->registers->SP);
    gbc->cpu->registers->PC = 0x20;
}

// 0xE8: ADD SP, r8 (0 0 H C)
static void op_add_sp_r8(gbc_system *gbc, char operand) {
    gbc->cpu->registers->SP = add_short(gbc->cpu->registers->SP, operand, &gbc->cpu->registers->F); 
}

// 0xE9: JP (HL) (- - - -)
static void op_jp_hlp(gbc_system *gbc) {
    gbc->cpu->registers->PC = gbc->cpu->registers->HL;
}

// 0xEA: LD (a16), A (- - - -)
static void op_ld_a16p_a(gbc_system *gbc, unsigned short operand) {
    write_byte(gbc->ram, operand, gbc->cpu->registers->A);
}

// 0xEE: XOR d8 (Z 0 0 0)
static void op_xor_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = xor(operand, gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xEF: RST 28H (- - - -)
static void op_rst_28h(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->PC, gbc->ram, &gbc->cpu->registers->SP);
    gbc->cpu->registers->PC = 0x28;
}

// 0xF0: LDH A, (a8) (- - - -)
static void op_ldh_a_a8p(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = read_byte(gbc->ram, 0xFF00 + operand);
}

// 0xF1: POP AF (Z N H C)
static void op_pop_af(gbc_system *gbc) {
    gbc->cpu->registers->AF = stack_pop(gbc->ram, &gbc->cpu->registers->SP) | (stack_pop(gbc->ram, &gbc->cpu->registers->SP) << 8);
}

// 0xF2: LD A, (C) (- - - -)
static void op_ld_a_cp(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = read_byte(gbc->ram, 0xFF00 + gbc->cpu->registers->C);
}

// 0xF3: DI (- - - -)
static void op_di(gbc_system *gbc) {
   gbc->cpu->registers->IME = 0; 
}

// 0xF5: PUSH AF (- - - -)
static void op_push_af(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->A, gbc->ram, &gbc->cpu->registers->SP);
    stack_push(gbc->cpu->registers->F, gbc->ram, &gbc->cpu->registers->SP);
}

// 0xF6: OR d8 (Z 0 0 0)
static void op_or_d8(gbc_system *gbc, unsigned char operand) {
    gbc->cpu->registers->A = or(gbc->cpu->registers->A, operand, &gbc->cpu->registers->F);
}

// 0xF7: RST 30H (- - - -)
static void op_rst_30h(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->PC, gbc->ram, &gbc->cpu->registers->SP);
    gbc->cpu->registers->PC = 0x30;
}

// 0xF8: LD HL, SP+r8 (0 0 H C)
static void op_ld_hl_sppr8(gbc_system *gbc, char operand) {
    printf("Unimplemented Instruction: LD HL, SP+r8\n");
}

// 0xF9: LD SP, HL (- - - -)
static void op_ld_sp_hl(gbc_system *gbc) {
    gbc->cpu->registers->SP = gbc->cpu->registers->HL;
}

// 0xFA: LD A, (a16) (- - - -)
static void op_ld_a_a16p(gbc_system *gbc, unsigned short operand) {
    gbc->cpu->registers->A = read_byte(gbc->ram, operand);
}

// 0xFB: EI (- - - -)
static void op_ei(gbc_system *gbc) {
    gbc->cpu->registers->IME = 1;
}

// 0xFE: CP d8 (Z 1 H C)
static void op_cp_d8(gbc_system *gbc, unsigned char operand) {
    sub_byte(gbc->cpu->registers->A, operand, &gbc->cpu->registers->F);
}

// 0xFF: RST 38H (- - - -)
static void op_rst_38h(gbc_system *gbc) {
    stack_push(gbc->cpu->registers->PC, gbc->ram, &gbc->cpu->registers->SP);
    gbc->cpu->registers->PC = 0x38;
}

gbc_instr instructions[INSTRUCTION_COUNT] = {
    {"NOP", 1, 4, 1, 0, op_nop}, // 0x00
    {"LD BC, %04x", 3, 12, 1, 0, op_ld_bc_d16}, // 0x01
    {"LD (BC), A", 1, 8, 1, 0, op_ld_bcp_a}, // 0x02
    {"INC BC", 1, 8, 1, 0, op_inc_bc}, // 0x03
    {"INC B", 1, 4, 1, 0, op_inc_b}, // 0x04
    {"DEC B", 1, 4, 1, 0, op_dec_b}, // 0x05
    {"LD B, %02x", 2, 8, 1, 0, op_ld_b_d8}, // 0x06
    {"RLCA", 1, 4, 1, 0, op_rlca}, // 0x07
    {"LD (%04x), SP", 3, 20, 1, 0, op_ld_a16p_sp}, // 0x08
    {"ADD HL, BC", 1, 8, 1, 0, op_add_hl_bc}, // 0x09
    {"LD A, (BC)", 1, 8, 1, 0, op_ld_a_bcp}, // 0x0A
    {"DEC BC", 1, 8, 1, 0, op_dec_bc}, // 0x0B
    {"INC C", 1, 4, 1, 0, op_inc_c}, // 0x0C
    {"DEC C", 1, 4, 1, 0, op_dec_c}, // 0x0D
    {"LD C, %02x", 2, 8, 1, 0, op_ld_c_d8}, // 0x0E
    {"RRCA", 1, 4, 1, 0, op_rrca}, // 0x0F
    {"STOP 0", 2, 4, 1, 0, op_stop_0}, // 0x10
    {"LD DE, %04x", 3, 12, 1, 0, op_ld_de_d16}, // 0x11
    {"LD (DE), A", 1, 8, 1, 0, op_ld_dep_a}, // 0x12
    {"INC DE", 1, 8, 1, 0, op_inc_de}, // 0x13
    {"INC D", 1, 4, 1, 0, op_inc_d}, // 0x14
    {"DEC D", 1, 4, 1, 0, op_dec_d}, // 0x15
    {"LD D, %02x", 2, 8, 1, 0, op_ld_d_d8}, // 0x16
    {"RLA", 1, 4, 1, 0, op_rla}, // 0x17
    {"JR %02x", 2, 12, 1, 1, op_jr_r8}, // 0x18
    {"ADD HL, DE", 1, 8, 1, 0, op_add_hl_de}, // 0x19
    {"LD A, (DE)", 1, 8, 1, 0, op_ld_a_dep}, // 0x1A
    {"DEC DE", 1, 8, 1, 0, op_dec_de}, // 0x1B
    {"INC E", 1, 4, 1, 0, op_inc_e}, // 0x1C
    {"DEC E", 1, 4, 1, 0, op_dec_e}, // 0x1D
    {"LD E, %02x", 2, 8, 1, 0, op_ld_e_d8}, // 0x1E
    {"RRA", 1, 4, 1, 0, op_rra}, // 0x1F
    {"JR NZ, %02x", 2, 12, 1, 1, op_jr_nz_r8}, // 0x20
    {"LD HL, %04x", 3, 12, 1, 0, op_ld_hl_d16}, // 0x21
    {"LD (HL+), A", 1, 8, 1, 0, op_ld_hlpp_a}, // 0x22
    {"INC HL", 1, 8, 1, 0, op_inc_hl}, // 0x23
    {"INC H", 1, 4, 1, 0, op_inc_h}, // 0x24
    {"DEC H", 1, 4, 1, 0, op_dec_h}, // 0x25
    {"LD H, %02x", 2, 8, 1, 0, op_ld_h_d8}, // 0x26
    {"DAA", 1, 4, 1, 0, op_daa}, // 0x27
    {"JR Z, %02x", 2, 12, 1, 1, op_jr_z_r8}, // 0x28
    {"ADD HL, HL", 1, 8, 1, 0, op_add_hl_hl}, // 0x29
    {"LD A, (HL+)", 1, 8, 1, 0, op_ld_a_hlpp}, // 0x2A
    {"DEC HL", 1, 8, 1, 0, op_dec_hl}, // 0x2B
    {"INC L", 1, 4, 1, 0, op_inc_l}, // 0x2C
    {"DEC L", 1, 4, 1, 0, op_dec_l}, // 0x2D
    {"LD L, %02x", 2, 8, 1, 0, op_ld_l_d8}, // 0x2E
    {"CPL", 1, 4, 1, 0, op_cpl}, // 0x2F
    {"JR NC, %02x", 2, 12, 1, 1, op_jr_nc_r8}, // 0x30
    {"LD SP, %04x", 3, 12, 1, 0, op_ld_sp_d16}, // 0x31
    {"LD (HL-), A", 1, 8, 1, 0, op_ld_hlmp_a}, // 0x32
    {"INC SP", 1, 8, 1, 0, op_inc_sp}, // 0x33
    {"INC (HL)", 1, 12, 1, 0, op_inc_hlp}, // 0x34
    {"DEC (HL)", 1, 12, 1, 0, op_dec_hlp}, // 0x35
    {"LD (HL), %02x", 2, 12, 1, 0, op_ld_hlp_d8}, // 0x36
    {"SCF", 1, 4, 1, 0, op_scf}, // 0x37
    {"JR C, %02x", 2, 12, 1, 1, op_jr_c_r8}, // 0x38
    {"ADD HL, SP", 1, 8, 1, 0, op_add_hl_sp}, // 0x39
    {"LD A, (HL-)", 1, 8, 1, 0, op_ld_a_hlmp}, // 0x3A
    {"DEC SP", 1, 8, 1, 0, op_dec_sp}, // 0x3B
    {"INC A", 1, 4, 1, 0, op_inc_a}, // 0x3C
    {"DEC A", 1, 4, 1, 0, op_dec_a}, // 0x3D
    {"LD A, %02x", 2, 8, 1, 0, op_ld_a_d8}, // 0x3E
    {"CCF", 1, 4, 1, 0, op_ccf}, // 0x3F
    {"LD B, B", 1, 4, 1, 0, op_ld_b_b}, // 0x40
    {"LD B, C", 1, 4, 1, 0, op_ld_b_c}, // 0x41
    {"LD B, D", 1, 4, 1, 0, op_ld_b_d}, // 0x42
    {"LD B, E", 1, 4, 1, 0, op_ld_b_e}, // 0x43
    {"LD B, H", 1, 4, 1, 0, op_ld_b_h}, // 0x44
    {"LD B, L", 1, 4, 1, 0, op_ld_b_l}, // 0x45
    {"LD B, (HL)", 1, 8, 1, 0, op_ld_b_hlp}, // 0x46
    {"LD B, A", 1, 4, 1, 0, op_ld_b_a}, // 0x47
    {"LD C, B", 1, 4, 1, 0, op_ld_c_b}, // 0x48
    {"LD C, C", 1, 4, 1, 0, op_ld_c_c}, // 0x49
    {"LD C, D", 1, 4, 1, 0, op_ld_c_d}, // 0x4A
    {"LD C, E", 1, 4, 1, 0, op_ld_c_e}, // 0x4B
    {"LD C, H", 1, 4, 1, 0, op_ld_c_h}, // 0x4C
    {"LD C, L", 1, 4, 1, 0, op_ld_c_l}, // 0x4D
    {"LD C, (HL)", 1, 8, 1, 0, op_ld_c_hlp}, // 0x4E
    {"LD C, A", 1, 4, 1, 0, op_ld_c_a}, // 0x4F
    {"LD D, B", 1, 4, 1, 0, op_ld_d_b}, // 0x50
    {"LD D, C", 1, 4, 1, 0, op_ld_d_c}, // 0x51
    {"LD D, D", 1, 4, 1, 0, op_ld_d_d}, // 0x52
    {"LD D, E", 1, 4, 1, 0, op_ld_d_e}, // 0x53
    {"LD D, H", 1, 4, 1, 0, op_ld_d_h}, // 0x54
    {"LD D, L", 1, 4, 1, 0, op_ld_d_l}, // 0x55
    {"LD D, (HL)", 1, 8, 1, 0, op_ld_d_hlp}, // 0x56
    {"LD D, A", 1, 4, 1, 0, op_ld_d_a}, // 0x57
    {"LD E, B", 1, 4, 1, 0, op_ld_e_b}, // 0x58
    {"LD E, C", 1, 4, 1, 0, op_ld_e_c}, // 0x59
    {"LD E, D", 1, 4, 1, 0, op_ld_e_d}, // 0x5A
    {"LD E, E", 1, 4, 1, 0, op_ld_e_e}, // 0x5B
    {"LD E, H", 1, 4, 1, 0, op_ld_e_h}, // 0x5C
    {"LD E, L", 1, 4, 1, 0, op_ld_e_l}, // 0x5D
    {"LD E, (HL)", 1, 8, 1, 0, op_ld_e_hlp}, // 0x5E
    {"LD E, A", 1, 4, 1, 0, op_ld_e_a}, // 0x5F
    {"LD H, B", 1, 4, 1, 0, op_ld_h_b}, // 0x60
    {"LD H, C", 1, 4, 1, 0, op_ld_h_c}, // 0x61
    {"LD H, D", 1, 4, 1, 0, op_ld_h_d}, // 0x62
    {"LD H, E", 1, 4, 1, 0, op_ld_h_e}, // 0x63
    {"LD H, H", 1, 4, 1, 0, op_ld_h_h}, // 0x64
    {"LD H, L", 1, 4, 1, 0, op_ld_h_l}, // 0x65
    {"LD H, (HL)", 1, 8, 1, 0, op_ld_h_hlp}, // 0x66
    {"LD H, A", 1, 4, 1, 0, op_ld_h_a}, // 0x67
    {"LD L, B", 1, 4, 1, 0, op_ld_l_b}, // 0x68
    {"LD L, C", 1, 4, 1, 0, op_ld_l_c}, // 0x69
    {"LD L, D", 1, 4, 1, 0, op_ld_l_d}, // 0x6A
    {"LD L, E", 1, 4, 1, 0, op_ld_l_e}, // 0x6B
    {"LD L, H", 1, 4, 1, 0, op_ld_l_h}, // 0x6C
    {"LD L, L", 1, 4, 1, 0, op_ld_l_l}, // 0x6D
    {"LD L, (HL)", 1, 8, 1, 0, op_ld_l_hlp}, // 0x6E
    {"LD L, A", 1, 4, 1, 0, op_ld_l_a}, // 0x6F
    {"LD (HL), B", 1, 8, 1, 0, op_ld_hlp_b}, // 0x70
    {"LD (HL), C", 1, 8, 1, 0, op_ld_hlp_c}, // 0x71
    {"LD (HL), D", 1, 8, 1, 0, op_ld_hlp_d}, // 0x72
    {"LD (HL), E", 1, 8, 1, 0, op_ld_hlp_e}, // 0x73
    {"LD (HL), H", 1, 8, 1, 0, op_ld_hlp_h}, // 0x74
    {"LD (HL), L", 1, 8, 1, 0, op_ld_hlp_l}, // 0x75
    {"HALT", 1, 4, 1, 0, op_halt}, // 0x76
    {"LD (HL), A", 1, 8, 1, 0, op_ld_hlp_a}, // 0x77
    {"LD A, B", 1, 4, 1, 0, op_ld_a_b}, // 0x78
    {"LD A, C", 1, 4, 1, 0, op_ld_a_c}, // 0x79
    {"LD A, D", 1, 4, 1, 0, op_ld_a_d}, // 0x7A
    {"LD A, E", 1, 4, 1, 0, op_ld_a_e}, // 0x7B
    {"LD A, H", 1, 4, 1, 0, op_ld_a_h}, // 0x7C
    {"LD A, L", 1, 4, 1, 0, op_ld_a_l}, // 0x7D
    {"LD A, (HL)", 1, 8, 1, 0, op_ld_a_hlp}, // 0x7E
    {"LD A, A", 1, 4, 1, 0, op_ld_a_a}, // 0x7F
    {"ADD A, B", 1, 4, 1, 0, op_add_a_b}, // 0x80
    {"ADD A, C", 1, 4, 1, 0, op_add_a_c}, // 0x81
    {"ADD A, D", 1, 4, 1, 0, op_add_a_d}, // 0x82
    {"ADD A, E", 1, 4, 1, 0, op_add_a_e}, // 0x83
    {"ADD A, H", 1, 4, 1, 0, op_add_a_h}, // 0x84
    {"ADD A, L", 1, 4, 1, 0, op_add_a_l}, // 0x85
    {"ADD A, (HL)", 1, 8, 1, 0, op_add_a_hlp}, // 0x86
    {"ADD A, A", 1, 4, 1, 0, op_add_a_a}, // 0x87
    {"ADC A, B", 1, 4, 1, 0, op_adc_a_b}, // 0x88
    {"ADC A, C", 1, 4, 1, 0, op_adc_a_c}, // 0x89
    {"ADC A, D", 1, 4, 1, 0, op_adc_a_d}, // 0x8A
    {"ADC A, E", 1, 4, 1, 0, op_adc_a_e}, // 0x8B
    {"ADC A, H", 1, 4, 1, 0, op_adc_a_h}, // 0x8C
    {"ADC A, L", 1, 4, 1, 0, op_adc_a_l}, // 0x8D
    {"ADC A, (HL)", 1, 8, 1, 0, op_adc_a_hlp}, // 0x8E
    {"ADC A, A", 1, 4, 1, 0, op_adc_a_a}, // 0x8F
    {"SUB B", 1, 4, 1, 0, op_sub_b}, // 0x90
    {"SUB C", 1, 4, 1, 0, op_sub_c}, // 0x91
    {"SUB D", 1, 4, 1, 0, op_sub_d}, // 0x92
    {"SUB E", 1, 4, 1, 0, op_sub_e}, // 0x93
    {"SUB H", 1, 4, 1, 0, op_sub_h}, // 0x94
    {"SUB L", 1, 4, 1, 0, op_sub_l}, // 0x95
    {"SUB (HL)", 1, 8, 1, 0, op_sub_hlp}, // 0x96
    {"SUB A", 1, 4, 1, 0, op_sub_a}, // 0x97
    {"SBC A, B", 1, 4, 1, 0, op_sbc_a_b}, // 0x98
    {"SBC A, C", 1, 4, 1, 0, op_sbc_a_c}, // 0x99
    {"SBC A, D", 1, 4, 1, 0, op_sbc_a_d}, // 0x9A
    {"SBC A, E", 1, 4, 1, 0, op_sbc_a_e}, // 0x9B
    {"SBC A, H", 1, 4, 1, 0, op_sbc_a_h}, // 0x9C
    {"SBC A, L", 1, 4, 1, 0, op_sbc_a_l}, // 0x9D
    {"SBC A, (HL)", 1, 8, 1, 0, op_sbc_a_hlp}, // 0x9E
    {"SBC A, A", 1, 4, 1, 0, op_sbc_a_a}, // 0x9F
    {"AND B", 1, 4, 1, 0, op_and_b}, // 0xA0
    {"AND C", 1, 4, 1, 0, op_and_c}, // 0xA1
    {"AND D", 1, 4, 1, 0, op_and_d}, // 0xA2
    {"AND E", 1, 4, 1, 0, op_and_e}, // 0xA3
    {"AND H", 1, 4, 1, 0, op_and_h}, // 0xA4
    {"AND L", 1, 4, 1, 0, op_and_l}, // 0xA5
    {"AND (HL)", 1, 8, 1, 0, op_and_hlp}, // 0xA6
    {"AND A", 1, 4, 1, 0, op_and_a}, // 0xA7
    {"XOR B", 1, 4, 1, 0, op_xor_b}, // 0xA8
    {"XOR C", 1, 4, 1, 0, op_xor_c}, // 0xA9
    {"XOR D", 1, 4, 1, 0, op_xor_d}, // 0xAA
    {"XOR E", 1, 4, 1, 0, op_xor_e}, // 0xAB
    {"XOR H", 1, 4, 1, 0, op_xor_h}, // 0xAC
    {"XOR L", 1, 4, 1, 0, op_xor_l}, // 0xAD
    {"XOR (HL)", 1, 8, 1, 0, op_xor_hlp}, // 0xAE
    {"XOR A", 1, 4, 1, 0, op_xor_a}, // 0xAF
    {"OR B", 1, 4, 1, 0, op_or_b}, // 0xB0
    {"OR C", 1, 4, 1, 0, op_or_c}, // 0xB1
    {"OR D", 1, 4, 1, 0, op_or_d}, // 0xB2
    {"OR E", 1, 4, 1, 0, op_or_e}, // 0xB3
    {"OR H", 1, 4, 1, 0, op_or_h}, // 0xB4
    {"OR L", 1, 4, 1, 0, op_or_l}, // 0xB5
    {"OR (HL)", 1, 8, 1, 0, op_or_hlp}, // 0xB6
    {"OR A", 1, 4, 1, 0, op_or_a}, // 0xB7
    {"CP B", 1, 4, 1, 0, op_cp_b}, // 0xB8
    {"CP C", 1, 4, 1, 0, op_cp_c}, // 0xB9
    {"CP D", 1, 4, 1, 0, op_cp_d}, // 0xBA
    {"CP E", 1, 4, 1, 0, op_cp_e}, // 0xBB
    {"CP H", 1, 4, 1, 0, op_cp_h}, // 0xBC
    {"CP L", 1, 4, 1, 0, op_cp_l}, // 0xBD
    {"CP (HL)", 1, 8, 1, 0, op_cp_hlp}, // 0xBE
    {"CP A", 1, 4, 1, 0, op_cp_a}, // 0xBF
    {"RET NZ", 1, 20, 0, 0, op_ret_nz}, // 0xC0
    {"POP BC", 1, 12, 1, 0, op_pop_bc}, // 0xC1
    {"JP NZ, %04x", 3, 16, 0, 0, op_jp_nz_a16}, // 0xC2
    {"JP %04x", 3, 16, 0, 0, op_jp_a16}, // 0xC3
    {"CALL NZ, %04x", 3, 24, 0, 0, op_call_nz_a16}, // 0xC4
    {"PUSH BC", 1, 16, 1, 0, op_push_bc}, // 0xC5
    {"ADD A, %02x", 2, 8, 1, 0, op_add_a_d8}, // 0xC6
    {"RST 00H", 1, 16, 0, 0, op_rst_00h}, // 0xC7
    {"RET Z", 1, 20, 0, 0, op_ret_z}, // 0xC8
    {"RET", 1, 16, 0, 0, op_ret}, // 0xC9
    {"JP Z, %04x", 3, 16, 0, 0, op_jp_z_a16}, // 0xCA
    {"PREFIX CB", 1, 4, 1, 0, op_prefix_cb}, // 0xCB
    {"CALL Z, %04x", 3, 24, 0, 0, op_call_z_a16}, // 0xCC
    {"CALL %04x", 3, 24, 0, 0, op_call_a16}, // 0xCD
    {"ADC A, %02x", 2, 8, 1, 0, op_adc_a_d8}, // 0xCE
    {"RST 08H", 1, 16, 0, 0, op_rst_08h}, // 0xCF
    {"RET NC", 1, 20, 0, 0, op_ret_nc}, // 0xD0
    {"POP DE", 1, 12, 1, 0, op_pop_de}, // 0xD1
    {"JP NC, %04x", 3, 16, 0, 0, op_jp_nc_a16}, // 0xD2
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xD3
    {"CALL NC, %04x", 3, 24, 0, 0, op_call_nc_a16}, // 0xD4
    {"PUSH DE", 1, 16, 1, 0, op_push_de}, // 0xD5
    {"SUB %02x", 2, 8, 1, 0, op_sub_d8}, // 0xD6
    {"RST 10H", 1, 16, 0, 0, op_rst_10h}, // 0xD7
    {"RET C", 1, 20, 0, 0, op_ret_c}, // 0xD8
    {"RETI", 1, 16, 0, 0, op_reti}, // 0xD9
    {"JP C, %04x", 3, 16, 0, 0, op_jp_c_a16}, // 0xDA
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xDB
    {"CALL C, %04x", 3, 24, 0, 0, op_call_c_a16}, // 0xDC
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xDD
    {"SBC A, %02x", 2, 8, 1, 0, op_sbc_a_d8}, // 0xDE
    {"RST 18H", 1, 16, 0, 0, op_rst_18h}, // 0xDF
    {"LDH (%02x), A", 2, 12, 1, 0, op_ldh_a8p_a}, // 0xE0
    {"POP HL", 1, 12, 1, 0, op_pop_hl}, // 0xE1
    {"LD (C), A", 1, 8, 1, 0, op_ld_cp_a}, // 0xE2
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xE3
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xE4
    {"PUSH HL", 1, 16, 1, 0, op_push_hl}, // 0xE5
    {"AND %02x", 2, 8, 1, 0, op_and_d8}, // 0xE6
    {"RST 20H", 1, 16, 0, 0, op_rst_20h}, // 0xE7
    {"ADD SP, %02x", 2, 16, 1, 1, op_add_sp_r8}, // 0xE8
    {"JP (HL)", 1, 4, 0, 0, op_jp_hlp}, // 0xE9
    {"LD (%04x), A", 3, 16, 1, 0, op_ld_a16p_a}, // 0xEA
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xEB
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xEC
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xED
    {"XOR %02x", 2, 8, 1, 0, op_xor_d8}, // 0xEE
    {"RST 28H", 1, 16, 0, 0, op_rst_28h}, // 0xEF
    {"LDH A, (%02x)", 2, 12, 1, 0, op_ldh_a_a8p}, // 0xF0
    {"POP AF", 1, 12, 1, 0, op_pop_af}, // 0xF1
    {"LD A, (C)", 2, 8, 1, 0, op_ld_a_cp}, // 0xF2
    {"DI", 1, 4, 1, 0, op_di}, // 0xF3
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xF4
    {"PUSH AF", 1, 16, 1, 0, op_push_af}, // 0xF5
    {"OR %02x", 2, 8, 1, 0, op_or_d8}, // 0xF6
    {"RST 30H", 1, 16, 0, 0, op_rst_30h}, // 0xF7
    {"LD HL, SP+%02x", 2, 12, 1, 1, op_ld_hl_sppr8}, // 0xF8
    {"LD SP, HL", 1, 8, 1, 0, op_ld_sp_hl}, // 0xF9
    {"LD A, (%04x)", 3, 16, 1, 0, op_ld_a_a16p}, // 0xFA
    {"EI", 1, 4, 1, 0, op_ei}, // 0xFB
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xFC
    {"NOP", 1, 4, 1, 0, op_nop}, // 0xFD
    {"CP %02x", 2, 8, 1, 0, op_cp_d8}, // 0xFE
    {"RST 38H", 1, 16, 0, 0, op_rst_38h} // 0xFF
};

// 0xCB00: RLC B (Z 0 0 C)
static void op_rlc_b(gbc_system *gbc) {
    gbc->cpu->registers->B = rotate_left(gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xCB01: RLC C (Z 0 0 C)
static void op_rlc_c(gbc_system *gbc) {
    gbc->cpu->registers->C = rotate_left(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xCB02: RLC D (Z 0 0 C)
static void op_rlc_d(gbc_system *gbc) {
    gbc->cpu->registers->D = rotate_left(gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xCB03: RLC E (Z 0 0 C)
static void op_rlc_e(gbc_system *gbc) {
    gbc->cpu->registers->E = rotate_left(gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xCB04: RLC H (Z 0 0 C)
static void op_rlc_h(gbc_system *gbc) {
    gbc->cpu->registers->H = rotate_left(gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xCB05: RLC L (Z 0 0 C)
static void op_rlc_l(gbc_system *gbc) {
    gbc->cpu->registers->L = rotate_left(gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xCB06: RLC (HL) (Z 0 0 C)
static void op_rlc_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               rotate_left(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F));
}

// 0xCB07: RLC A (Z 0 0 C)
static void op_rlc_a(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_left(gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xCB08: RRC B (Z 0 0 C)
static void op_rrc_b(gbc_system *gbc) {
    gbc->cpu->registers->B = rotate_right(gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xCB09: RRC C (Z 0 0 C)
static void op_rrc_c(gbc_system *gbc) {
    gbc->cpu->registers->C = rotate_right(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xCB0A: RRC D (Z 0 0 C)
static void op_rrc_d(gbc_system *gbc) {
    gbc->cpu->registers->D = rotate_right(gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xCB0B: RRC E (Z 0 0 C)
static void op_rrc_e(gbc_system *gbc) {
    gbc->cpu->registers->E = rotate_right(gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xCB0C: RRC H (Z 0 0 C)
static void op_rrc_h(gbc_system *gbc) {
    gbc->cpu->registers->H = rotate_right(gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xCB0D: RRC L (Z 0 0 C)
static void op_rrc_l(gbc_system *gbc) {
    gbc->cpu->registers->L = rotate_right(gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xCB0E: RRC (HL) (Z 0 0 C)
static void op_rrc_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               rotate_right(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F));
}

// 0xCB0F: RRC A (Z 0 0 C)
static void op_rrc_a(gbc_system *gbc) {
    gbc->cpu->registers->C = rotate_right(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xCB10: RL B (Z 0 0 C)
static void op_rl_b(gbc_system *gbc) {
    gbc->cpu->registers->B = rotate_left_carry(gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xCB11: RL C (Z 0 0 C)
static void op_rl_c(gbc_system *gbc) {
    gbc->cpu->registers->C = rotate_left_carry(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xCB12: RL D (Z 0 0 C)
static void op_rl_d(gbc_system *gbc) {
    gbc->cpu->registers->D = rotate_left_carry(gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xCB13: RL E (Z 0 0 C)
static void op_rl_e(gbc_system *gbc) {
    gbc->cpu->registers->E = rotate_left_carry(gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xCB14: RL H (Z 0 0 C)
static void op_rl_h(gbc_system *gbc) {
    gbc->cpu->registers->H = rotate_left_carry(gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xCB15: RL L (Z 0 0 C)
static void op_rl_l(gbc_system *gbc) {
    gbc->cpu->registers->L = rotate_left_carry(gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xCB16: RL (HL) (Z 0 0 C)
static void op_rl_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               rotate_left_carry(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F));
}

// 0xCB17: RL A (Z 0 0 C)
static void op_rl_a(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_left_carry(gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xCB18: RR B (Z 0 0 C)
static void op_rr_b(gbc_system *gbc) {
    gbc->cpu->registers->B = rotate_right_carry(gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xCB19: RR C (Z 0 0 C)
static void op_rr_c(gbc_system *gbc) {
    gbc->cpu->registers->C = rotate_right_carry(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xCB1A: RR D (Z 0 0 C)
static void op_rr_d(gbc_system *gbc) {
    gbc->cpu->registers->D = rotate_right_carry(gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xCB1B: RR E (Z 0 0 C)
static void op_rr_e(gbc_system *gbc) {
    gbc->cpu->registers->E = rotate_right_carry(gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xCB1C: RR H (Z 0 0 C)
static void op_rr_h(gbc_system *gbc) {
    gbc->cpu->registers->H = rotate_right_carry(gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xCB1D: RR L (Z 0 0 C)
static void op_rr_l(gbc_system *gbc) {
    gbc->cpu->registers->L = rotate_right_carry(gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xCB1E: RR (HL) (Z 0 0 C)
static void op_rr_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               rotate_right_carry(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F));
}

// 0xCB1F: RR A (Z 0 0 C)
static void op_rr_a(gbc_system *gbc) {
    gbc->cpu->registers->A = rotate_right_carry(gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xCB20: SLA B (Z 0 0 C)
static void op_sla_b(gbc_system *gbc) {
    gbc->cpu->registers->B = shift_left_arith(gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xCB21: SLA C (Z 0 0 C)
static void op_sla_c(gbc_system *gbc) {
    gbc->cpu->registers->C = shift_left_arith(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xCB22: SLA D (Z 0 0 C)
static void op_sla_d(gbc_system *gbc) {
    gbc->cpu->registers->D = shift_left_arith(gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xCB23: SLA E (Z 0 0 C)
static void op_sla_e(gbc_system *gbc) {
    gbc->cpu->registers->E = shift_left_arith(gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xCB24: SLA H (Z 0 0 C)
static void op_sla_h(gbc_system *gbc) {
    gbc->cpu->registers->H = shift_left_arith(gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xCB25: SLA L (Z 0 0 C)
static void op_sla_l(gbc_system *gbc) {
    gbc->cpu->registers->L = shift_left_arith(gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xCB26: SLA (HL) (Z 0 0 C)
static void op_sla_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               shift_left_arith(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F));
}

// 0xCB27: SLA A (Z 0 0 C)
static void op_sla_a(gbc_system *gbc) {
    gbc->cpu->registers->A = shift_left_arith(gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xCB28: SRA B (Z 0 0 0)
static void op_sra_b(gbc_system *gbc) {
    gbc->cpu->registers->B = shift_right_arith(gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xCB29: SRA C (Z 0 0 0)
static void op_sra_c(gbc_system *gbc) {
    gbc->cpu->registers->C = shift_right_arith(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xCB2A: SRA D (Z 0 0 0)
static void op_sra_d(gbc_system *gbc) {
    gbc->cpu->registers->D = shift_right_arith(gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xCB2B: SRA E (Z 0 0 0)
static void op_sra_e(gbc_system *gbc) {
    gbc->cpu->registers->E = shift_right_arith(gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xCB2C: SRA H (Z 0 0 0)
static void op_sra_h(gbc_system *gbc) {
    gbc->cpu->registers->H = shift_right_arith(gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xCB2D: SRA L (Z 0 0 0)
static void op_sra_l(gbc_system *gbc) {
    gbc->cpu->registers->L = shift_right_arith(gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xCB2E: SRA (HL) (Z 0 0 0)
static void op_sra_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               shift_right_arith(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F));
}

// 0xCB2F: SRA A (Z 0 0 0)
static void op_sra_a(gbc_system *gbc) {
    gbc->cpu->registers->A = shift_right_arith(gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xCB30: SWAP B (Z 0 0 0)
static void op_swap_b(gbc_system *gbc) {
    gbc->cpu->registers->B = swap(gbc->cpu->registers->B, &gbc->cpu->registers->F);
}

// 0xCB31: SWAP C (Z 0 0 0)
static void op_swap_c(gbc_system *gbc) {
    gbc->cpu->registers->C = swap(gbc->cpu->registers->C, &gbc->cpu->registers->F);
}

// 0xCB32: SWAP D (Z 0 0 0)
static void op_swap_d(gbc_system *gbc) {
    gbc->cpu->registers->D = swap(gbc->cpu->registers->D, &gbc->cpu->registers->F);
}

// 0xCB33: SWAP E (Z 0 0 0)
static void op_swap_e(gbc_system *gbc) {
    gbc->cpu->registers->E = swap(gbc->cpu->registers->E, &gbc->cpu->registers->F);
}

// 0xCB34: SWAP H (Z 0 0 0)
static void op_swap_h(gbc_system *gbc) {
    gbc->cpu->registers->H = swap(gbc->cpu->registers->H, &gbc->cpu->registers->F);
}

// 0xCB35: SWAP L (Z 0 0 0)
static void op_swap_l(gbc_system *gbc) {
    gbc->cpu->registers->L = swap(gbc->cpu->registers->L, &gbc->cpu->registers->F);
}

// 0xCB36: SWAP (HL) (Z 0 0 0)
static void op_swap_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               swap(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F));
}

// 0xCB37: SWAP A (Z 0 0 0)
static void op_swap_a(gbc_system *gbc) {
    gbc->cpu->registers->A = swap(gbc->cpu->registers->A, &gbc->cpu->registers->F);
}

// 0xCB38: SRL B (Z 0 0 C)
static void op_srl_b(gbc_system *gbc) {
    gbc->cpu->registers->B = shift_right_logic(gbc->cpu->registers->B, &gbc->cpu->registers->F); 
}

// 0xCB39: SRL C (Z 0 0 C)
static void op_srl_c(gbc_system *gbc) {
    gbc->cpu->registers->C = shift_right_logic(gbc->cpu->registers->C, &gbc->cpu->registers->F); 
}

// 0xCB3A: SRL D (Z 0 0 C)
static void op_srl_d(gbc_system *gbc) {
    gbc->cpu->registers->D = shift_right_logic(gbc->cpu->registers->D, &gbc->cpu->registers->F); 
}

// 0xCB3B: SRL E (Z 0 0 C)
static void op_srl_e(gbc_system *gbc) {
    gbc->cpu->registers->E = shift_right_logic(gbc->cpu->registers->E, &gbc->cpu->registers->F); 
}

// 0xCB3C: SRL H (Z 0 0 C)
static void op_srl_h(gbc_system *gbc) {
    gbc->cpu->registers->H = shift_right_logic(gbc->cpu->registers->H, &gbc->cpu->registers->F); 
}

// 0xCB3D: SRL L (Z 0 0 C)
static void op_srl_l(gbc_system *gbc) {
    gbc->cpu->registers->L = shift_right_logic(gbc->cpu->registers->L, &gbc->cpu->registers->F); 
}

// 0xCB3E: SRL (HL) (Z 0 0 C)
static void op_srl_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               shift_right_logic(read_byte(gbc->ram, gbc->cpu->registers->HL), &gbc->cpu->registers->F)); 
}

// 0xCB3F: SRL A (Z 0 0 C)
static void op_srl_a(gbc_system *gbc) {
    gbc->cpu->registers->A = shift_right_logic(gbc->cpu->registers->A, &gbc->cpu->registers->F); 
}

// 0xCB40: BIT 0, B (Z 0 1 -)
static void op_bit_0_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 0, &gbc->cpu->registers->F); 
}

// 0xCB41: BIT 0, C (Z 0 1 -)
static void op_bit_0_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 0, &gbc->cpu->registers->F); 
}

// 0xCB42: BIT 0, D (Z 0 1 -)
static void op_bit_0_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 0, &gbc->cpu->registers->F); 
}

// 0xCB43: BIT 0, E (Z 0 1 -)
static void op_bit_0_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 0, &gbc->cpu->registers->F); 
}

// 0xCB44: BIT 0, H (Z 0 1 -)
static void op_bit_0_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 0, &gbc->cpu->registers->F); 
}

// 0xCB45: BIT 0, L (Z 0 1 -)
static void op_bit_0_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 0, &gbc->cpu->registers->F); 
}

// 0xCB46: BIT 0, (HL) (Z 0 1 -)
static void op_bit_0_hlp(gbc_system *gbc) {
    test_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 0, &gbc->cpu->registers->F); 
}

// 0xCB47: BIT 0, A (Z 0 1 -)
static void op_bit_0_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 0, &gbc->cpu->registers->F); 
}

// 0xCB48: BIT 1, B (Z 0 1 -)
static void op_bit_1_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 1, &gbc->cpu->registers->F); 
}

// 0xCB49: BIT 1, C (Z 0 1 -)
static void op_bit_1_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 1, &gbc->cpu->registers->F); 
}

// 0xCB4A: BIT 1, D (Z 0 1 -)
static void op_bit_1_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 1, &gbc->cpu->registers->F); 
}

// 0xCB4B: BIT 1, E (Z 0 1 -)
static void op_bit_1_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 1, &gbc->cpu->registers->F); 
}

// 0xCB4C: BIT 1, H (Z 0 1 -)
static void op_bit_1_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 1, &gbc->cpu->registers->F); 
}

// 0xCB4D: BIT 1, L (Z 0 1 -)
static void op_bit_1_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 1, &gbc->cpu->registers->F); 
}

// 0xCB4E: BIT 1, (HL) (Z 0 1 -)
static void op_bit_1_hlp(gbc_system *gbc) {
    test_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 1, &gbc->cpu->registers->F); 
}

// 0xCB4F: BIT 1, A (Z 0 1 -)
static void op_bit_1_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 1, &gbc->cpu->registers->F); 
}

// 0xCB50: BIT 2, B (Z 0 1 -)
static void op_bit_2_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 2, &gbc->cpu->registers->F); 
}

// 0xCB51: BIT 2, C (Z 0 1 -)
static void op_bit_2_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 2, &gbc->cpu->registers->F); 
}

// 0xCB52: BIT 2, D (Z 0 1 -)
static void op_bit_2_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 2, &gbc->cpu->registers->F); 
}

// 0xCB53: BIT 2, E (Z 0 1 -)
static void op_bit_2_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 2, &gbc->cpu->registers->F); 
}

// 0xCB54: BIT 2, H (Z 0 1 -)
static void op_bit_2_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 2, &gbc->cpu->registers->F); 
}

// 0xCB55: BIT 2, L (Z 0 1 -)
static void op_bit_2_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 2, &gbc->cpu->registers->F); 
}

// 0xCB56: BIT 2, (HL) (Z 0 1 -)
static void op_bit_2_hlp(gbc_system *gbc) {
    test_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 2, &gbc->cpu->registers->F); 
}

// 0xCB57: BIT 2, A (Z 0 1 -)
static void op_bit_2_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 2, &gbc->cpu->registers->F); 
}

// 0xCB58: BIT 3, B (Z 0 1 -)
static void op_bit_3_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 3, &gbc->cpu->registers->F); 
}

// 0xCB59: BIT 3, C (Z 0 1 -)
static void op_bit_3_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 3, &gbc->cpu->registers->F); 
}

// 0xCB5A: BIT 3, D (Z 0 1 -)
static void op_bit_3_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 3, &gbc->cpu->registers->F); 
}

// 0xCB5B: BIT 3, E (Z 0 1 -)
static void op_bit_3_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 3, &gbc->cpu->registers->F); 
}

// 0xCB5C: BIT 3, H (Z 0 1 -)
static void op_bit_3_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 3, &gbc->cpu->registers->F); 
}

// 0xCB5D: BIT 3, L (Z 0 1 -)
static void op_bit_3_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 3, &gbc->cpu->registers->F); 
}

// 0xCB5E: BIT 3, (HL) (Z 0 1 -)
static void op_bit_3_hlp(gbc_system *gbc) {
    test_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 3, &gbc->cpu->registers->F); 
}

// 0xCB5F: BIT 3, A (Z 0 1 -)
static void op_bit_3_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 3, &gbc->cpu->registers->F); 
}

// 0xCB60: BIT 4, B (Z 0 1 -)
static void op_bit_4_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 4, &gbc->cpu->registers->F); 
}

// 0xCB61: BIT 4, C (Z 0 1 -)
static void op_bit_4_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 4, &gbc->cpu->registers->F); 
}

// 0xCB62: BIT 4, D (Z 0 1 -)
static void op_bit_4_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 4, &gbc->cpu->registers->F); 
}

// 0xCB63: BIT 4, E (Z 0 1 -)
static void op_bit_4_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 4, &gbc->cpu->registers->F); 
}

// 0xCB64: BIT 4, H (Z 0 1 -)
static void op_bit_4_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 4, &gbc->cpu->registers->F); 
}

// 0xCB65: BIT 4, L (Z 0 1 -)
static void op_bit_4_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 4, &gbc->cpu->registers->F); 
}

// 0xCB66: BIT 4, (HL) (Z 0 1 -)
static void op_bit_4_hlp(gbc_system *gbc) {
    test_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 4, &gbc->cpu->registers->F); 
}

// 0xCB67: BIT 4, A (Z 0 1 -)
static void op_bit_4_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 4, &gbc->cpu->registers->F); 
}

// 0xCB68: BIT 5, B (Z 0 1 -)
static void op_bit_5_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 5, &gbc->cpu->registers->F); 
}

// 0xCB69: BIT 5, C (Z 0 1 -)
static void op_bit_5_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 5, &gbc->cpu->registers->F); 
}

// 0xCB6A: BIT 5, D (Z 0 1 -)
static void op_bit_5_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 5, &gbc->cpu->registers->F); 
}

// 0xCB6B: BIT 5, E (Z 0 1 -)
static void op_bit_5_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 5, &gbc->cpu->registers->F); 
}

// 0xCB6C: BIT 5, H (Z 0 1 -)
static void op_bit_5_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 5, &gbc->cpu->registers->F); 
}

// 0xCB6D: BIT 5, L (Z 0 1 -)
static void op_bit_5_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 5, &gbc->cpu->registers->F); 
}

// 0xCB6E: BIT 5, (HL) (Z 0 1 -)
static void op_bit_5_hlp(gbc_system *gbc) {
    test_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 5, &gbc->cpu->registers->F); 
}

// 0xCB6F: BIT 5, A (Z 0 1 -)
static void op_bit_5_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 5, &gbc->cpu->registers->F); 
}

// 0xCB70: BIT 6, B (Z 0 1 -)
static void op_bit_6_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 6, &gbc->cpu->registers->F); 
}

// 0xCB71: BIT 6, C (Z 0 1 -)
static void op_bit_6_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 6, &gbc->cpu->registers->F); 
}

// 0xCB72: BIT 6, D (Z 0 1 -)
static void op_bit_6_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 6, &gbc->cpu->registers->F); 
}

// 0xCB73: BIT 6, E (Z 0 1 -)
static void op_bit_6_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 6, &gbc->cpu->registers->F); 
}

// 0xCB74: BIT 6, H (Z 0 1 -)
static void op_bit_6_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 6, &gbc->cpu->registers->F); 
}

// 0xCB75: BIT 6, L (Z 0 1 -)
static void op_bit_6_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 6, &gbc->cpu->registers->F); 
}

// 0xCB76: BIT 6, (HL) (Z 0 1 -)
static void op_bit_6_hlp(gbc_system *gbc) {
    test_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 6, &gbc->cpu->registers->F); 
}

// 0xCB77: BIT 6, A (Z 0 1 -)
static void op_bit_6_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 6, &gbc->cpu->registers->F); 
}

// 0xCB78: BIT 7, B (Z 0 1 -)
static void op_bit_7_b(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->B, 7, &gbc->cpu->registers->F); 
}

// 0xCB79: BIT 7, C (Z 0 1 -)
static void op_bit_7_c(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->C, 7, &gbc->cpu->registers->F); 
}

// 0xCB7A: BIT 7, D (Z 0 1 -)
static void op_bit_7_d(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->D, 7, &gbc->cpu->registers->F); 
}

// 0xCB7B: BIT 7, E (Z 0 1 -)
static void op_bit_7_e(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->E, 7, &gbc->cpu->registers->F); 
}

// 0xCB7C: BIT 7, H (Z 0 1 -)
static void op_bit_7_h(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->H, 7, &gbc->cpu->registers->F); 
}

// 0xCB7D: BIT 7, L (Z 0 1 -)
static void op_bit_7_l(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->L, 7, &gbc->cpu->registers->F); 
}

// 0xCB7E: BIT 7, (HL) (Z 0 1 -)
static void op_bit_7_hlp(gbc_system *gbc) {
    test_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 7, &gbc->cpu->registers->F); 
}

// 0xCB7F: BIT 7, A (Z 0 1 -)
static void op_bit_7_a(gbc_system *gbc) {
    test_bit(gbc->cpu->registers->A, 7, &gbc->cpu->registers->F); 
}

// 0xCB80: RES 0, B (- - - -)
static void op_res_0_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 0, &gbc->cpu->registers->F);
}

// 0xCB81: RES 0, C (- - - -)
static void op_res_0_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 0, &gbc->cpu->registers->F);
}

// 0xCB82: RES 0, D (- - - -)
static void op_res_0_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 0, &gbc->cpu->registers->F);
}

// 0xCB83: RES 0, E (- - - -)
static void op_res_0_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 0, &gbc->cpu->registers->F);
}

// 0xCB84: RES 0, H (- - - -)
static void op_res_0_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 0, &gbc->cpu->registers->F);
}

// 0xCB85: RES 0, L (- - - -)
static void op_res_0_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 0, &gbc->cpu->registers->F);
}

// 0xCB86: RES 0, (HL) (- - - -)
static void op_res_0_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               reset_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 0, &gbc->cpu->registers->F));
}

// 0xCB87: RES 0, A (- - - -)
static void op_res_0_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 0, &gbc->cpu->registers->F);
}

// 0xCB88: RES 1, B (- - - -)
static void op_res_1_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 1, &gbc->cpu->registers->F);
}

// 0xCB89: RES 1, C (- - - -)
static void op_res_1_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 1, &gbc->cpu->registers->F);
}

// 0xCB8A: RES 1, D (- - - -)
static void op_res_1_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 1, &gbc->cpu->registers->F);
}

// 0xCB8B: RES 1, E (- - - -)
static void op_res_1_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 1, &gbc->cpu->registers->F);
}

// 0xCB8C: RES 1, H (- - - -)
static void op_res_1_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 1, &gbc->cpu->registers->F);
}

// 0xCB8D: RES 1, L (- - - -)
static void op_res_1_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 1, &gbc->cpu->registers->F);
}

// 0xCB8E: RES 1, (HL) (- - - -)
static void op_res_1_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               reset_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 1, &gbc->cpu->registers->F));
}

// 0xCB8F: RES 1, A (- - - -)
static void op_res_1_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 1, &gbc->cpu->registers->F);
}

// 0xCB90: RES 2, B (- - - -)
static void op_res_2_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 2, &gbc->cpu->registers->F);
}

// 0xCB91: RES 2, C (- - - -)
static void op_res_2_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 2, &gbc->cpu->registers->F);
}

// 0xCB92: RES 2, D (- - - -)
static void op_res_2_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 2, &gbc->cpu->registers->F);
}

// 0xCB93: RES 2, E (- - - -)
static void op_res_2_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 2, &gbc->cpu->registers->F);
}

// 0xCB94: RES 2, H (- - - -)
static void op_res_2_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 2, &gbc->cpu->registers->F);
}

// 0xCB95: RES 2, L (- - - -)
static void op_res_2_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 2, &gbc->cpu->registers->F);
}

// 0xCB96: RES 2, (HL) (- - - -)
static void op_res_2_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               reset_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 2, &gbc->cpu->registers->F));
}

// 0xCB97: RES 2, A (- - - -)
static void op_res_2_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 2, &gbc->cpu->registers->F);
}

// 0xCB98: RES 3, B (- - - -)
static void op_res_3_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 3, &gbc->cpu->registers->F);
}

// 0xCB99: RES 3, C (- - - -)
static void op_res_3_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 3, &gbc->cpu->registers->F);
}

// 0xCB9A: RES 3, D (- - - -)
static void op_res_3_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 3, &gbc->cpu->registers->F);
}

// 0xCB9B: RES 3, E (- - - -)
static void op_res_3_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 3, &gbc->cpu->registers->F);
}

// 0xCB9C: RES 3, H (- - - -)
static void op_res_3_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 3, &gbc->cpu->registers->F);
}

// 0xCB9D: RES 3, L (- - - -)
static void op_res_3_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 3, &gbc->cpu->registers->F);
}

// 0xCB9E: RES 3, (HL) (- - - -)
static void op_res_3_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               reset_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 3, &gbc->cpu->registers->F));
}

// 0xCB9F: RES 3, A (- - - -)
static void op_res_3_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 3, &gbc->cpu->registers->F);
}

// 0xCBA0: RES 4, B (- - - -)
static void op_res_4_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 4, &gbc->cpu->registers->F);
}

// 0xCBA1: RES 4, C (- - - -)
static void op_res_4_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 4, &gbc->cpu->registers->F);
}

// 0xCBA2: RES 4, D (- - - -)
static void op_res_4_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 4, &gbc->cpu->registers->F);
}

// 0xCBA3: RES 4, E (- - - -)
static void op_res_4_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 4, &gbc->cpu->registers->F);
}

// 0xCBA4: RES 4, H (- - - -)
static void op_res_4_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 4, &gbc->cpu->registers->F);
}

// 0xCBA5: RES 4, L (- - - -)
static void op_res_4_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 4, &gbc->cpu->registers->F);
}

// 0xCBA6: RES 4, (HL) (- - - -)
static void op_res_4_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               reset_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 4, &gbc->cpu->registers->F));
}

// 0xCBA7: RES 4, A (- - - -)
static void op_res_4_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 4, &gbc->cpu->registers->F);
}

// 0xCBA8: RES 5, B (- - - -)
static void op_res_5_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 5, &gbc->cpu->registers->F);
}

// 0xCBA9: RES 5, C (- - - -)
static void op_res_5_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 5, &gbc->cpu->registers->F);
}

// 0xCBAA: RES 5, D (- - - -)
static void op_res_5_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 5, &gbc->cpu->registers->F);
}

// 0xCBAB: RES 5, E (- - - -)
static void op_res_5_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 5, &gbc->cpu->registers->F);
}

// 0xCBAC: RES 5, H (- - - -)
static void op_res_5_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 5, &gbc->cpu->registers->F);
}

// 0xCBAD: RES 5, L (- - - -)
static void op_res_5_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 5, &gbc->cpu->registers->F);
}

// 0xCBAE: RES 5, (HL) (- - - -)
static void op_res_5_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               reset_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 5, &gbc->cpu->registers->F));
}

// 0xCBAF: RES 5, A (- - - -)
static void op_res_5_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 5, &gbc->cpu->registers->F);
}

// 0xCBB0: RES 6, B (- - - -)
static void op_res_6_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 6, &gbc->cpu->registers->F);
}

// 0xCBB1: RES 6, C (- - - -)
static void op_res_6_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 6, &gbc->cpu->registers->F);
}

// 0xCBB2: RES 6, D (- - - -)
static void op_res_6_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 6, &gbc->cpu->registers->F);
}

// 0xCBB3: RES 6, E (- - - -)
static void op_res_6_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 6, &gbc->cpu->registers->F);
}

// 0xCBB4: RES 6, H (- - - -)
static void op_res_6_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 6, &gbc->cpu->registers->F);
}

// 0xCBB5: RES 6, L (- - - -)
static void op_res_6_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 6, &gbc->cpu->registers->F);
}

// 0xCBB6: RES 6, (HL) (- - - -)
static void op_res_6_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               reset_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 6, &gbc->cpu->registers->F));
}

// 0xCBB7: RES 6, A (- - - -)
static void op_res_6_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 6, &gbc->cpu->registers->F);
}

// 0xCBB8: RES 7, B (- - - -)
static void op_res_7_b(gbc_system *gbc) {
    gbc->cpu->registers->B = reset_bit(gbc->cpu->registers->B, 7, &gbc->cpu->registers->F);
}

// 0xCBB9: RES 7, C (- - - -)
static void op_res_7_c(gbc_system *gbc) {
    gbc->cpu->registers->C = reset_bit(gbc->cpu->registers->C, 7, &gbc->cpu->registers->F);
}

// 0xCBBA: RES 7, D (- - - -)
static void op_res_7_d(gbc_system *gbc) {
    gbc->cpu->registers->D = reset_bit(gbc->cpu->registers->D, 7, &gbc->cpu->registers->F);
}

// 0xCBBB: RES 7, E (- - - -)
static void op_res_7_e(gbc_system *gbc) {
    gbc->cpu->registers->E = reset_bit(gbc->cpu->registers->E, 7, &gbc->cpu->registers->F);
}

// 0xCBBC: RES 7, H (- - - -)
static void op_res_7_h(gbc_system *gbc) {
    gbc->cpu->registers->H = reset_bit(gbc->cpu->registers->H, 7, &gbc->cpu->registers->F);
}

// 0xCBBD: RES 7, L (- - - -)
static void op_res_7_l(gbc_system *gbc) {
    gbc->cpu->registers->L = reset_bit(gbc->cpu->registers->L, 7, &gbc->cpu->registers->F);
}

// 0xCBBE: RES 7, (HL) (- - - -)
static void op_res_7_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               reset_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 7, &gbc->cpu->registers->F));
}

// 0xCBBF: RES 7, A (- - - -)
static void op_res_7_a(gbc_system *gbc) {
    gbc->cpu->registers->A = reset_bit(gbc->cpu->registers->A, 7, &gbc->cpu->registers->F);
}

// 0xCBC0: SET 0, B (- - - -)
static void op_set_0_b(gbc_system *gbc) {
   gbc->cpu->registers->A = set_bit(gbc->cpu->registers->B, 0, &gbc->cpu->registers->F);
}

// 0xCBC1: SET 0, C (- - - -)
static void op_set_0_c(gbc_system *gbc) {
   gbc->cpu->registers->A = set_bit(gbc->cpu->registers->C, 0, &gbc->cpu->registers->F);
}

// 0xCBC2: SET 0, D (- - - -)
static void op_set_0_d(gbc_system *gbc) {
   gbc->cpu->registers->A = set_bit(gbc->cpu->registers->D, 0, &gbc->cpu->registers->F);
}

// 0xCBC3: SET 0, E (- - - -)
static void op_set_0_e(gbc_system *gbc) {
   gbc->cpu->registers->A = set_bit(gbc->cpu->registers->E, 0, &gbc->cpu->registers->F);
}

// 0xCBC4: SET 0, H (- - - -)
static void op_set_0_h(gbc_system *gbc) {
   gbc->cpu->registers->A = set_bit(gbc->cpu->registers->H, 0, &gbc->cpu->registers->F);
}

// 0xCBC5: SET 0, L (- - - -)
static void op_set_0_l(gbc_system *gbc) {
   gbc->cpu->registers->A = set_bit(gbc->cpu->registers->L, 0, &gbc->cpu->registers->F);
}

// 0xCBC6: SET 0, (HL) (- - - -)
static void op_set_0_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               set_bit(read_byte(gbc->ram, gbc->cpu->registers->HL), 0, &gbc->cpu->registers->F));
}

// 0xCBC7: SET 0, A (- - - -)
static void op_set_0_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 0, &gbc->cpu->registers->F);
}

// 0xCBC8: SET 1, B (- - - -)
static void op_set_1_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->A, 1, &gbc->cpu->registers->F);
}

// 0xCBC9: SET 1, C (- - - -)
static void op_set_1_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 1, &gbc->cpu->registers->F);
}

// 0xCBCA: SET 1, D (- - - -)
static void op_set_1_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 1, &gbc->cpu->registers->F);
}

// 0xCBCB: SET 1, E (- - - -)
static void op_set_1_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 1, &gbc->cpu->registers->F);
}

// 0xCBCC: SET 1, H (- - - -)
static void op_set_1_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 1, &gbc->cpu->registers->F);
}

// 0xCBCD: SET 1, L (- - - -)
static void op_set_1_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 1, &gbc->cpu->registers->F);
}

// 0xCBCE: SET 1, (HL) (- - - -)
static void op_set_1_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               set_bit(gbc->cpu->registers->A, 1, &gbc->cpu->registers->F));
}

// 0xCBCF: SET 1, A (- - - -)
static void op_set_1_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 1, &gbc->cpu->registers->F);
}

// 0xCBD0: SET 2, B (- - - -)
static void op_set_2_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 2, &gbc->cpu->registers->F);
}

// 0xCBD1: SET 2, C (- - - -)
static void op_set_2_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 2, &gbc->cpu->registers->F);
}

// 0xCBD2: SET 2, D (- - - -)
static void op_set_2_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 2, &gbc->cpu->registers->F);
}

// 0xCBD3: SET 2, E (- - - -)
static void op_set_2_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 2, &gbc->cpu->registers->F);
}

// 0xCBD4: SET 2, H (- - - -)
static void op_set_2_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 2, &gbc->cpu->registers->F);
}

// 0xCBD5: SET 2, L (- - - -)
static void op_set_2_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 2, &gbc->cpu->registers->F);
}

// 0xCBD6: SET 2, (HL) (- - - -)
static void op_set_2_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               set_bit(gbc->cpu->registers->L, 2, &gbc->cpu->registers->F));
}

// 0xCBD7: SET 2, A (- - - -)
static void op_set_2_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 2, &gbc->cpu->registers->F);
}

// 0xCBD8: SET 3, B (- - - -)
static void op_set_3_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 3, &gbc->cpu->registers->F);
}

// 0xCBD9: SET 3, C (- - - -)
static void op_set_3_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 3, &gbc->cpu->registers->F);
}

// 0xCBDA: SET 3, D (- - - -)
static void op_set_3_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 3, &gbc->cpu->registers->F);
}

// 0xCBDB: SET 3, E (- - - -)
static void op_set_3_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 3, &gbc->cpu->registers->F);
}

// 0xCBDC: SET 3, H (- - - -)
static void op_set_3_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 3, &gbc->cpu->registers->F);
}

// 0xCBDD: SET 3, L (- - - -)
static void op_set_3_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 3, &gbc->cpu->registers->F);
}

// 0xCBDE: SET 3, (HL) (- - - -)
static void op_set_3_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               set_bit(gbc->cpu->registers->A, 3, &gbc->cpu->registers->F));
}

// 0xCBDF: SET 3, A (- - - -)
static void op_set_3_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 3, &gbc->cpu->registers->F);
}

// 0xCBE0: SET 4, B (- - - -)
static void op_set_4_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 4, &gbc->cpu->registers->F);
}

// 0xCBE1: SET 4, C (- - - -)
static void op_set_4_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 4, &gbc->cpu->registers->F);
}

// 0xCBE2: SET 4, D (- - - -)
static void op_set_4_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 4, &gbc->cpu->registers->F);
}

// 0xCBE3: SET 4, E (- - - -)
static void op_set_4_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 4, &gbc->cpu->registers->F);
}

// 0xCBE4: SET 4, H (- - - -)
static void op_set_4_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 4, &gbc->cpu->registers->F);
}

// 0xCBE5: SET 4, L (- - - -)
static void op_set_4_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 4, &gbc->cpu->registers->F);
}

// 0xCBE6: SET 4, (HL) (- - - -)
static void op_set_4_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               set_bit(gbc->cpu->registers->L, 4, &gbc->cpu->registers->F));
}

// 0xCBE7: SET 4, A (- - - -)
static void op_set_4_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 4, &gbc->cpu->registers->F);
}

// 0xCBE8: SET 5, B (- - - -)
static void op_set_5_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 5, &gbc->cpu->registers->F);
}

// 0xCBE9: SET 5, C (- - - -)
static void op_set_5_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 5, &gbc->cpu->registers->F);
}

// 0xCBEA: SET 5, D (- - - -)
static void op_set_5_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 5, &gbc->cpu->registers->F);
}

// 0xCBEB: SET 5, E (- - - -)
static void op_set_5_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 5, &gbc->cpu->registers->F);
}

// 0xCBEC: SET 5, H (- - - -)
static void op_set_5_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 5, &gbc->cpu->registers->F);
}

// 0xCBED: SET 5, L (- - - -)
static void op_set_5_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 5, &gbc->cpu->registers->F);
}

// 0xCBEE: SET 5, (HL) (- - - -)
static void op_set_5_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               set_bit(gbc->cpu->registers->L, 5, &gbc->cpu->registers->F));
}

// 0xCBEF: SET 5, A (- - - -)
static void op_set_5_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 5, &gbc->cpu->registers->F);
}

// 0xCBF0: SET 6, B (- - - -)
static void op_set_6_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 6, &gbc->cpu->registers->F);
}

// 0xCBF1: SET 6, C (- - - -)
static void op_set_6_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 6, &gbc->cpu->registers->F);
}

// 0xCBF2: SET 6, D (- - - -)
static void op_set_6_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 6, &gbc->cpu->registers->F);
}

// 0xCBF3: SET 6, E (- - - -)
static void op_set_6_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 6, &gbc->cpu->registers->F);
}

// 0xCBF4: SET 6, H (- - - -)
static void op_set_6_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 6, &gbc->cpu->registers->F);
}

// 0xCBF5: SET 6, L (- - - -)
static void op_set_6_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 6, &gbc->cpu->registers->F);
}

// 0xCBF6: SET 6, (HL) (- - - -)
static void op_set_6_hlp(gbc_system *gbc) {
    write_byte(gbc->ram,
               gbc->cpu->registers->HL,
               set_bit(gbc->cpu->registers->A, 6, &gbc->cpu->registers->F));
}

// 0xCBF7: SET 6, A (- - - -)
static void op_set_6_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 6, &gbc->cpu->registers->F);
}

// 0xCBF8: SET 7, B (- - - -)
static void op_set_7_b(gbc_system *gbc) {
    gbc->cpu->registers->B = set_bit(gbc->cpu->registers->B, 7, &gbc->cpu->registers->F);
}

// 0xCBF9: SET 7, C (- - - -)
static void op_set_7_c(gbc_system *gbc) {
    gbc->cpu->registers->C = set_bit(gbc->cpu->registers->C, 7, &gbc->cpu->registers->F);
}

// 0xCBFA: SET 7, D (- - - -)
static void op_set_7_d(gbc_system *gbc) {
    gbc->cpu->registers->D = set_bit(gbc->cpu->registers->D, 7, &gbc->cpu->registers->F);
}

// 0xCBFB: SET 7, E (- - - -)
static void op_set_7_e(gbc_system *gbc) {
    gbc->cpu->registers->E = set_bit(gbc->cpu->registers->E, 7, &gbc->cpu->registers->F);
}

// 0xCBFC: SET 7, H (- - - -)
static void op_set_7_h(gbc_system *gbc) {
    gbc->cpu->registers->H = set_bit(gbc->cpu->registers->H, 7, &gbc->cpu->registers->F);
}

// 0xCBFD: SET 7, L (- - - -)
static void op_set_7_l(gbc_system *gbc) {
    gbc->cpu->registers->L = set_bit(gbc->cpu->registers->L, 7, &gbc->cpu->registers->F);
}

// 0xCBFE: SET 7, (HL) (- - - -)
static void op_set_7_hlp(gbc_system *gbc) {
    write_byte(gbc->ram, 
               gbc->cpu->registers->HL,
               set_bit(gbc->cpu->registers->A, 7, &gbc->cpu->registers->F));
}

// 0xCBFF: SET 7, A (- - - -)
static void op_set_7_a(gbc_system *gbc) {
    gbc->cpu->registers->A = set_bit(gbc->cpu->registers->A, 7, &gbc->cpu->registers->F);
}

gbc_instr cb_instructions[CB_INSTRUCTION_COUNT] = {
    {"RLC B", 2, 8, 1, 0, op_rlc_b}, // 0xCB00
    {"RLC C", 2, 8, 1, 0, op_rlc_c}, // 0xCB01
    {"RLC D", 2, 8, 1, 0, op_rlc_d}, // 0xCB02
    {"RLC E", 2, 8, 1, 0, op_rlc_e}, // 0xCB03
    {"RLC H", 2, 8, 1, 0, op_rlc_h}, // 0xCB04
    {"RLC L", 2, 8, 1, 0, op_rlc_l}, // 0xCB05
    {"RLC (HL)", 2, 16, 1, 0, op_rlc_hlp}, // 0xCB06
    {"RLC A", 2, 8, 1, 0, op_rlc_a}, // 0xCB07
    {"RRC B", 2, 8, 1, 0, op_rrc_b}, // 0xCB08
    {"RRC C", 2, 8, 1, 0, op_rrc_c}, // 0xCB09
    {"RRC D", 2, 8, 1, 0, op_rrc_d}, // 0xCB0A
    {"RRC E", 2, 8, 1, 0, op_rrc_e}, // 0xCB0B
    {"RRC H", 2, 8, 1, 0, op_rrc_h}, // 0xCB0C
    {"RRC L", 2, 8, 1, 0, op_rrc_l}, // 0xCB0D
    {"RRC (HL)", 2, 16, 1, 0, op_rrc_hlp}, // 0xCB0E
    {"RRC A", 2, 8, 1, 0, op_rrc_a}, // 0xCB0F
    {"RL B", 2, 8, 1, 0, op_rl_b}, // 0xCB10
    {"RL C", 2, 8, 1, 0, op_rl_c}, // 0xCB11
    {"RL D", 2, 8, 1, 0, op_rl_d}, // 0xCB12
    {"RL E", 2, 8, 1, 0, op_rl_e}, // 0xCB13
    {"RL H", 2, 8, 1, 0, op_rl_h}, // 0xCB14
    {"RL L", 2, 8, 1, 0, op_rl_l}, // 0xCB15
    {"RL (HL)", 2, 16, 1, 0, op_rl_hlp}, // 0xCB16
    {"RL A", 2, 8, 1, 0, op_rl_a}, // 0xCB17
    {"RR B", 2, 8, 1, 0, op_rr_b}, // 0xCB18
    {"RR C", 2, 8, 1, 0, op_rr_c}, // 0xCB19
    {"RR D", 2, 8, 1, 0, op_rr_d}, // 0xCB1A
    {"RR E", 2, 8, 1, 0, op_rr_e}, // 0xCB1B
    {"RR H", 2, 8, 1, 0, op_rr_h}, // 0xCB1C
    {"RR L", 2, 8, 1, 0, op_rr_l}, // 0xCB1D
    {"RR (HL)", 2, 16, 1, 0, op_rr_hlp}, // 0xCB1E
    {"RR A", 2, 8, 1, 0, op_rr_a}, // 0xCB1F
    {"SLA B", 2, 8, 1, 0, op_sla_b}, // 0xCB20
    {"SLA C", 2, 8, 1, 0, op_sla_c}, // 0xCB21
    {"SLA D", 2, 8, 1, 0, op_sla_d}, // 0xCB22
    {"SLA E", 2, 8, 1, 0, op_sla_e}, // 0xCB23
    {"SLA H", 2, 8, 1, 0, op_sla_h}, // 0xCB24
    {"SLA L", 2, 8, 1, 0, op_sla_l}, // 0xCB25
    {"SLA (HL)", 2, 16, 1, 0, op_sla_hlp}, // 0xCB26
    {"SLA A", 2, 8, 1, 0, op_sla_a}, // 0xCB27
    {"SRA B", 2, 8, 1, 0, op_sra_b}, // 0xCB28
    {"SRA C", 2, 8, 1, 0, op_sra_c}, // 0xCB29
    {"SRA D", 2, 8, 1, 0, op_sra_d}, // 0xCB2A
    {"SRA E", 2, 8, 1, 0, op_sra_e}, // 0xCB2B
    {"SRA H", 2, 8, 1, 0, op_sra_h}, // 0xCB2C
    {"SRA L", 2, 8, 1, 0, op_sra_l}, // 0xCB2D
    {"SRA (HL)", 2, 16, 1, 0, op_sra_hlp}, // 0xCB2E
    {"SRA A", 2, 8, 1, 0, op_sra_a}, // 0xCB2F
    {"SWAP B", 2, 8, 1, 0, op_swap_b}, // 0xCB30
    {"SWAP C", 2, 8, 1, 0, op_swap_c}, // 0xCB31
    {"SWAP D", 2, 8, 1, 0, op_swap_d}, // 0xCB32
    {"SWAP E", 2, 8, 1, 0, op_swap_e}, // 0xCB33
    {"SWAP H", 2, 8, 1, 0, op_swap_h}, // 0xCB34
    {"SWAP L", 2, 8, 1, 0, op_swap_l}, // 0xCB35
    {"SWAP (HL)", 2, 16, 1, 0, op_swap_hlp}, // 0xCB36
    {"SWAP A", 2, 8, 1, 0, op_swap_a}, // 0xCB37
    {"SRL B", 2, 8, 1, 0, op_srl_b}, // 0xCB38
    {"SRL C", 2, 8, 1, 0, op_srl_c}, // 0xCB39
    {"SRL D", 2, 8, 1, 0, op_srl_d}, // 0xCB3A
    {"SRL E", 2, 8, 1, 0, op_srl_e}, // 0xCB3B
    {"SRL H", 2, 8, 1, 0, op_srl_h}, // 0xCB3C
    {"SRL L", 2, 8, 1, 0, op_srl_l}, // 0xCB3D
    {"SRL (HL)", 2, 16, 1, 0, op_srl_hlp}, // 0xCB3E
    {"SRL A", 2, 8, 1, 0, op_srl_a}, // 0xCB3F
    {"BIT 0, B", 2, 8, 1, 0, op_bit_0_b}, // 0xCB40
    {"BIT 0, C", 2, 8, 1, 0, op_bit_0_c}, // 0xCB41
    {"BIT 0, D", 2, 8, 1, 0, op_bit_0_d}, // 0xCB42
    {"BIT 0, E", 2, 8, 1, 0, op_bit_0_e}, // 0xCB43
    {"BIT 0, H", 2, 8, 1, 0, op_bit_0_h}, // 0xCB44
    {"BIT 0, L", 2, 8, 1, 0, op_bit_0_l}, // 0xCB45
    {"BIT 0, (HL)", 2, 16, 1, 0, op_bit_0_hlp}, // 0xCB46
    {"BIT 0, A", 2, 8, 1, 0, op_bit_0_a}, // 0xCB47
    {"BIT 1, B", 2, 8, 1, 0, op_bit_1_b}, // 0xCB48
    {"BIT 1, C", 2, 8, 1, 0, op_bit_1_c}, // 0xCB49
    {"BIT 1, D", 2, 8, 1, 0, op_bit_1_d}, // 0xCB4A
    {"BIT 1, E", 2, 8, 1, 0, op_bit_1_e}, // 0xCB4B
    {"BIT 1, H", 2, 8, 1, 0, op_bit_1_h}, // 0xCB4C
    {"BIT 1, L", 2, 8, 1, 0, op_bit_1_l}, // 0xCB4D
    {"BIT 1, (HL)", 2, 16, 1, 0, op_bit_1_hlp}, // 0xCB4E
    {"BIT 1, A", 2, 8, 1, 0, op_bit_1_a}, // 0xCB4F
    {"BIT 2, B", 2, 8, 1, 0, op_bit_2_b}, // 0xCB50
    {"BIT 2, C", 2, 8, 1, 0, op_bit_2_c}, // 0xCB51
    {"BIT 2, D", 2, 8, 1, 0, op_bit_2_d}, // 0xCB52
    {"BIT 2, E", 2, 8, 1, 0, op_bit_2_e}, // 0xCB53
    {"BIT 2, H", 2, 8, 1, 0, op_bit_2_h}, // 0xCB54
    {"BIT 2, L", 2, 8, 1, 0, op_bit_2_l}, // 0xCB55
    {"BIT 2, (HL)", 2, 16, 1, 0, op_bit_2_hlp}, // 0xCB56
    {"BIT 2, A", 2, 8, 1, 0, op_bit_2_a}, // 0xCB57
    {"BIT 3, B", 2, 8, 1, 0, op_bit_3_b}, // 0xCB58
    {"BIT 3, C", 2, 8, 1, 0, op_bit_3_c}, // 0xCB59
    {"BIT 3, D", 2, 8, 1, 0, op_bit_3_d}, // 0xCB5A
    {"BIT 3, E", 2, 8, 1, 0, op_bit_3_e}, // 0xCB5B
    {"BIT 3, H", 2, 8, 1, 0, op_bit_3_h}, // 0xCB5C
    {"BIT 3, L", 2, 8, 1, 0, op_bit_3_l}, // 0xCB5D
    {"BIT 3, (HL)", 2, 16, 1, 0, op_bit_3_hlp}, // 0xCB5E
    {"BIT 3, A", 2, 8, 1, 0, op_bit_3_a}, // 0xCB5F
    {"BIT 4, B", 2, 8, 1, 0, op_bit_4_b}, // 0xCB60
    {"BIT 4, C", 2, 8, 1, 0, op_bit_4_c}, // 0xCB61
    {"BIT 4, D", 2, 8, 1, 0, op_bit_4_d}, // 0xCB62
    {"BIT 4, E", 2, 8, 1, 0, op_bit_4_e}, // 0xCB63
    {"BIT 4, H", 2, 8, 1, 0, op_bit_4_h}, // 0xCB64
    {"BIT 4, L", 2, 8, 1, 0, op_bit_4_l}, // 0xCB65
    {"BIT 4, (HL)", 2, 16, 1, 0, op_bit_4_hlp}, // 0xCB66
    {"BIT 4, A", 2, 8, 1, 0, op_bit_4_a}, // 0xCB67
    {"BIT 5, B", 2, 8, 1, 0, op_bit_5_b}, // 0xCB68
    {"BIT 5, C", 2, 8, 1, 0, op_bit_5_c}, // 0xCB69
    {"BIT 5, D", 2, 8, 1, 0, op_bit_5_d}, // 0xCB6A
    {"BIT 5, E", 2, 8, 1, 0, op_bit_5_e}, // 0xCB6B
    {"BIT 5, H", 2, 8, 1, 0, op_bit_5_h}, // 0xCB6C
    {"BIT 5, L", 2, 8, 1, 0, op_bit_5_l}, // 0xCB6D
    {"BIT 5, (HL)", 2, 16, 1, 0, op_bit_5_hlp}, // 0xCB6E
    {"BIT 5, A", 2, 8, 1, 0, op_bit_5_a}, // 0xCB6F
    {"BIT 6, B", 2, 8, 1, 0, op_bit_6_b}, // 0xCB70
    {"BIT 6, C", 2, 8, 1, 0, op_bit_6_c}, // 0xCB71
    {"BIT 6, D", 2, 8, 1, 0, op_bit_6_d}, // 0xCB72
    {"BIT 6, E", 2, 8, 1, 0, op_bit_6_e}, // 0xCB73
    {"BIT 6, H", 2, 8, 1, 0, op_bit_6_h}, // 0xCB74
    {"BIT 6, L", 2, 8, 1, 0, op_bit_6_l}, // 0xCB75
    {"BIT 6, (HL)", 2, 16, 1, 0, op_bit_6_hlp}, // 0xCB76
    {"BIT 6, A", 2, 8, 1, 0, op_bit_6_a}, // 0xCB77
    {"BIT 7, B", 2, 8, 1, 0, op_bit_7_b}, // 0xCB78
    {"BIT 7, C", 2, 8, 1, 0, op_bit_7_c}, // 0xCB79
    {"BIT 7, D", 2, 8, 1, 0, op_bit_7_d}, // 0xCB7A
    {"BIT 7, E", 2, 8, 1, 0, op_bit_7_e}, // 0xCB7B
    {"BIT 7, H", 2, 8, 1, 0, op_bit_7_h}, // 0xCB7C
    {"BIT 7, L", 2, 8, 1, 0, op_bit_7_l}, // 0xCB7D
    {"BIT 7, (HL)", 2, 16, 1, 0, op_bit_7_hlp}, // 0xCB7E
    {"BIT 7, A", 2, 8, 1, 0, op_bit_7_a}, // 0xCB7F
    {"RES 0, B", 2, 8, 1, 0, op_res_0_b}, // 0xCB80
    {"RES 0, C", 2, 8, 1, 0, op_res_0_c}, // 0xCB81
    {"RES 0, D", 2, 8, 1, 0, op_res_0_d}, // 0xCB82
    {"RES 0, E", 2, 8, 1, 0, op_res_0_e}, // 0xCB83
    {"RES 0, H", 2, 8, 1, 0, op_res_0_h}, // 0xCB84
    {"RES 0, L", 2, 8, 1, 0, op_res_0_l}, // 0xCB85
    {"RES 0, (HL)", 2, 16, 1, 0, op_res_0_hlp}, // 0xCB86
    {"RES 0, A", 2, 8, 1, 0, op_res_0_a}, // 0xCB87
    {"RES 1, B", 2, 8, 1, 0, op_res_1_b}, // 0xCB88
    {"RES 1, C", 2, 8, 1, 0, op_res_1_c}, // 0xCB89
    {"RES 1, D", 2, 8, 1, 0, op_res_1_d}, // 0xCB8A
    {"RES 1, E", 2, 8, 1, 0, op_res_1_e}, // 0xCB8B
    {"RES 1, H", 2, 8, 1, 0, op_res_1_h}, // 0xCB8C
    {"RES 1, L", 2, 8, 1, 0, op_res_1_l}, // 0xCB8D
    {"RES 1, (HL)", 2, 16, 1, 0, op_res_1_hlp}, // 0xCB8E
    {"RES 1, A", 2, 8, 1, 0, op_res_1_a}, // 0xCB8F
    {"RES 2, B", 2, 8, 1, 0, op_res_2_b}, // 0xCB90
    {"RES 2, C", 2, 8, 1, 0, op_res_2_c}, // 0xCB91
    {"RES 2, D", 2, 8, 1, 0, op_res_2_d}, // 0xCB92
    {"RES 2, E", 2, 8, 1, 0, op_res_2_e}, // 0xCB93
    {"RES 2, H", 2, 8, 1, 0, op_res_2_h}, // 0xCB94
    {"RES 2, L", 2, 8, 1, 0, op_res_2_l}, // 0xCB95
    {"RES 2, (HL)", 2, 16, 1, 0, op_res_2_hlp}, // 0xCB96
    {"RES 2, A", 2, 8, 1, 0, op_res_2_a}, // 0xCB97
    {"RES 3, B", 2, 8, 1, 0, op_res_3_b}, // 0xCB98
    {"RES 3, C", 2, 8, 1, 0, op_res_3_c}, // 0xCB99
    {"RES 3, D", 2, 8, 1, 0, op_res_3_d}, // 0xCB9A
    {"RES 3, E", 2, 8, 1, 0, op_res_3_e}, // 0xCB9B
    {"RES 3, H", 2, 8, 1, 0, op_res_3_h}, // 0xCB9C
    {"RES 3, L", 2, 8, 1, 0, op_res_3_l}, // 0xCB9D
    {"RES 3, (HL)", 2, 16, 1, 0, op_res_3_hlp}, // 0xCB9E
    {"RES 3, A", 2, 8, 1, 0, op_res_3_a}, // 0xCB9F
    {"RES 4, B", 2, 8, 1, 0, op_res_4_b}, // 0xCBA0
    {"RES 4, C", 2, 8, 1, 0, op_res_4_c}, // 0xCBA1
    {"RES 4, D", 2, 8, 1, 0, op_res_4_d}, // 0xCBA2
    {"RES 4, E", 2, 8, 1, 0, op_res_4_e}, // 0xCBA3
    {"RES 4, H", 2, 8, 1, 0, op_res_4_h}, // 0xCBA4
    {"RES 4, L", 2, 8, 1, 0, op_res_4_l}, // 0xCBA5
    {"RES 4, (HL)", 2, 16, 1, 0, op_res_4_hlp}, // 0xCBA6
    {"RES 4, A", 2, 8, 1, 0, op_res_4_a}, // 0xCBA7
    {"RES 5, B", 2, 8, 1, 0, op_res_5_b}, // 0xCBA8
    {"RES 5, C", 2, 8, 1, 0, op_res_5_c}, // 0xCBA9
    {"RES 5, D", 2, 8, 1, 0, op_res_5_d}, // 0xCBAA
    {"RES 5, E", 2, 8, 1, 0, op_res_5_e}, // 0xCBAB
    {"RES 5, H", 2, 8, 1, 0, op_res_5_h}, // 0xCBAC
    {"RES 5, L", 2, 8, 1, 0, op_res_5_l}, // 0xCBAD
    {"RES 5, (HL)", 2, 16, 1, 0, op_res_5_hlp}, // 0xCBAE
    {"RES 5, A", 2, 8, 1, 0, op_res_5_a}, // 0xCBAF
    {"RES 6, B", 2, 8, 1, 0, op_res_6_b}, // 0xCBB0
    {"RES 6, C", 2, 8, 1, 0, op_res_6_c}, // 0xCBB1
    {"RES 6, D", 2, 8, 1, 0, op_res_6_d}, // 0xCBB2
    {"RES 6, E", 2, 8, 1, 0, op_res_6_e}, // 0xCBB3
    {"RES 6, H", 2, 8, 1, 0, op_res_6_h}, // 0xCBB4
    {"RES 6, L", 2, 8, 1, 0, op_res_6_l}, // 0xCBB5
    {"RES 6, (HL)", 2, 16, 1, 0, op_res_6_hlp}, // 0xCBB6
    {"RES 6, A", 2, 8, 1, 0, op_res_6_a}, // 0xCBB7
    {"RES 7, B", 2, 8, 1, 0, op_res_7_b}, // 0xCBB8
    {"RES 7, C", 2, 8, 1, 0, op_res_7_c}, // 0xCBB9
    {"RES 7, D", 2, 8, 1, 0, op_res_7_d}, // 0xCBBA
    {"RES 7, E", 2, 8, 1, 0, op_res_7_e}, // 0xCBBB
    {"RES 7, H", 2, 8, 1, 0, op_res_7_h}, // 0xCBBC
    {"RES 7, L", 2, 8, 1, 0, op_res_7_l}, // 0xCBBD
    {"RES 7, (HL)", 2, 16, 1, 0, op_res_7_hlp}, // 0xCBBE
    {"RES 7, A", 2, 8, 1, 0, op_res_7_a}, // 0xCBBF
    {"SET 0, B", 2, 8, 1, 0, op_set_0_b}, // 0xCBC0
    {"SET 0, C", 2, 8, 1, 0, op_set_0_c}, // 0xCBC1
    {"SET 0, D", 2, 8, 1, 0, op_set_0_d}, // 0xCBC2
    {"SET 0, E", 2, 8, 1, 0, op_set_0_e}, // 0xCBC3
    {"SET 0, H", 2, 8, 1, 0, op_set_0_h}, // 0xCBC4
    {"SET 0, L", 2, 8, 1, 0, op_set_0_l}, // 0xCBC5
    {"SET 0, (HL)", 2, 16, 1, 0, op_set_0_hlp}, // 0xCBC6
    {"SET 0, A", 2, 8, 1, 0, op_set_0_a}, // 0xCBC7
    {"SET 1, B", 2, 8, 1, 0, op_set_1_b}, // 0xCBC8
    {"SET 1, C", 2, 8, 1, 0, op_set_1_c}, // 0xCBC9
    {"SET 1, D", 2, 8, 1, 0, op_set_1_d}, // 0xCBCA
    {"SET 1, E", 2, 8, 1, 0, op_set_1_e}, // 0xCBCB
    {"SET 1, H", 2, 8, 1, 0, op_set_1_h}, // 0xCBCC
    {"SET 1, L", 2, 8, 1, 0, op_set_1_l}, // 0xCBCD
    {"SET 1, (HL)", 2, 16, 1, 0, op_set_1_hlp}, // 0xCBCE
    {"SET 1, A", 2, 8, 1, 0, op_set_1_a}, // 0xCBCF
    {"SET 2, B", 2, 8, 1, 0, op_set_2_b}, // 0xCBD0
    {"SET 2, C", 2, 8, 1, 0, op_set_2_c}, // 0xCBD1
    {"SET 2, D", 2, 8, 1, 0, op_set_2_d}, // 0xCBD2
    {"SET 2, E", 2, 8, 1, 0, op_set_2_e}, // 0xCBD3
    {"SET 2, H", 2, 8, 1, 0, op_set_2_h}, // 0xCBD4
    {"SET 2, L", 2, 8, 1, 0, op_set_2_l}, // 0xCBD5
    {"SET 2, (HL)", 2, 16, 1, 0, op_set_2_hlp}, // 0xCBD6
    {"SET 2, A", 2, 8, 1, 0, op_set_2_a}, // 0xCBD7
    {"SET 3, B", 2, 8, 1, 0, op_set_3_b}, // 0xCBD8
    {"SET 3, C", 2, 8, 1, 0, op_set_3_c}, // 0xCBD9
    {"SET 3, D", 2, 8, 1, 0, op_set_3_d}, // 0xCBDA
    {"SET 3, E", 2, 8, 1, 0, op_set_3_e}, // 0xCBDB
    {"SET 3, H", 2, 8, 1, 0, op_set_3_h}, // 0xCBDC
    {"SET 3, L", 2, 8, 1, 0, op_set_3_l}, // 0xCBDD
    {"SET 3, (HL)", 2, 16, 1, 0, op_set_3_hlp}, // 0xCBDE
    {"SET 3, A", 2, 8, 1, 0, op_set_3_a}, // 0xCBDF
    {"SET 4, B", 2, 8, 1, 0, op_set_4_b}, // 0xCBE0
    {"SET 4, C", 2, 8, 1, 0, op_set_4_c}, // 0xCBE1
    {"SET 4, D", 2, 8, 1, 0, op_set_4_d}, // 0xCBE2
    {"SET 4, E", 2, 8, 1, 0, op_set_4_e}, // 0xCBE3
    {"SET 4, H", 2, 8, 1, 0, op_set_4_h}, // 0xCBE4
    {"SET 4, L", 2, 8, 1, 0, op_set_4_l}, // 0xCBE5
    {"SET 4, (HL)", 2, 16, 1, 0, op_set_4_hlp}, // 0xCBE6
    {"SET 4, A", 2, 8, 1, 0, op_set_4_a}, // 0xCBE7
    {"SET 5, B", 2, 8, 1, 0, op_set_5_b}, // 0xCBE8
    {"SET 5, C", 2, 8, 1, 0, op_set_5_c}, // 0xCBE9
    {"SET 5, D", 2, 8, 1, 0, op_set_5_d}, // 0xCBEA
    {"SET 5, E", 2, 8, 1, 0, op_set_5_e}, // 0xCBEB
    {"SET 5, H", 2, 8, 1, 0, op_set_5_h}, // 0xCBEC
    {"SET 5, L", 2, 8, 1, 0, op_set_5_l}, // 0xCBED
    {"SET 5, (HL)", 2, 16, 1, 0, op_set_5_hlp}, // 0xCBEE
    {"SET 5, A", 2, 8, 1, 0, op_set_5_a}, // 0xCBEF
    {"SET 6, B", 2, 8, 1, 0, op_set_6_b}, // 0xCBF0
    {"SET 6, C", 2, 8, 1, 0, op_set_6_c}, // 0xCBF1
    {"SET 6, D", 2, 8, 1, 0, op_set_6_d}, // 0xCBF2
    {"SET 6, E", 2, 8, 1, 0, op_set_6_e}, // 0xCBF3
    {"SET 6, H", 2, 8, 1, 0, op_set_6_h}, // 0xCBF4
    {"SET 6, L", 2, 8, 1, 0, op_set_6_l}, // 0xCBF5
    {"SET 6, (HL)", 2, 16, 1, 0, op_set_6_hlp}, // 0xCBF6
    {"SET 6, A", 2, 8, 1, 0, op_set_6_a}, // 0xCBF7
    {"SET 7, B", 2, 8, 1, 0, op_set_7_b}, // 0xCBF8
    {"SET 7, C", 2, 8, 1, 0, op_set_7_c}, // 0xCBF9
    {"SET 7, D", 2, 8, 1, 0, op_set_7_d}, // 0xCBFA
    {"SET 7, E", 2, 8, 1, 0, op_set_7_e}, // 0xCBFB
    {"SET 7, H", 2, 8, 1, 0, op_set_7_h}, // 0xCBFC
    {"SET 7, L", 2, 8, 1, 0, op_set_7_l}, // 0xCBFD
    {"SET 7, (HL)", 2, 16, 1, 0, op_set_7_hlp}, // 0xCBFE
    {"SET 7, A", 2, 8, 1, 0, op_set_7_a} // 0xCBFF
};

// Returns an instruction struct of the provided opcode
gbc_instr find_instr(const unsigned char opcode, gbc_system *gbc) {

    // If the opcode has the CB prefix
    if(opcode == 0xCB) {
        unsigned char next_opcode = read_byte(gbc->ram, gbc->cpu->registers->PC + 1);
        return cb_instructions[next_opcode];
    } 
    // Standard instruction
    else {
        return instructions[opcode];
    }
}

// Executes an instruction by calling the function associated with it 
static void execute_instr(gbc_instr instruction, gbc_system *gbc) {

    // Create a pointer to the function to execute
    void (*opcode_function)();
    opcode_function = instruction.execute;

    // Execute the instruction based on the length of the operand
    char operand_len = instruction.length - 1;
    unsigned short operand;

    // If it is a CB instruction, it's shorter
    if(strcmp(instruction.disassembly, "PREFIX CB") == 0) {
        operand_len--; 
    }

    switch(operand_len) {

        case 0:
            opcode_function(gbc);
            break;

        case 1:
            operand = read_byte(gbc->ram, gbc->cpu->registers->PC + 1);
            opcode_function(gbc, (unsigned char) operand);
            break;

        case 2:
            operand = read_short(gbc->ram, gbc->cpu->registers->PC + 1);
            opcode_function(gbc, operand);
            break;
    }

    // If the instruction is not a jump, return, etc... Increment the PC
    if(instruction.increment_PC) {
        gbc->cpu->registers->PC += instruction.length;
    }
}

// Execute a cpu instruction and respect its duration in clock cycles
void cpu_do_clock(gbc_system *gbc) {
    
    // Wait until the current instruction is finished
    if(gbc->cpu->clock < gbc->cpu->run_for) {
        gbc->cpu->clock++;
    }
    // We've moved to the next instruction
    else {
   
        // Execute the instruction
        gbc_instr instruction = get_curr_instr(gbc);
        execute_instr(instruction, gbc);

        // Set the clock accordingly
        gbc->cpu->clock = 0;
        gbc->cpu->run_for = instruction.cycles;
    }
}

// Gets the current instruction at the program counter
static gbc_instr get_curr_instr(gbc_system *gbc) {
    
    // Get the opcode at the program counter
    unsigned char opcode = read_byte(gbc->ram, gbc->cpu->registers->PC);

    // Get the instruction structure
    return find_instr(opcode, gbc);
}

// Runs a cpu instruction immediately and ignores the clock
// Used in debug mode, for stepping through a program
void simulate_cpu(gbc_system *gbc) {
    execute_instr(get_curr_instr(gbc), gbc); 
}

// Returns the bit no of the specified flag in the F register
static char get_flag_offset(const char flag) {
    
    // Find the offset of the bit
    switch(flag) {
        case FLAG_ZERO: return 7;
        case FLAG_SUBTRACT: return 6;
        case FLAG_HALFCARRY: return 5;
        case FLAG_CARRY: return 4;
        default: return -1;
    }
}

// Sets a flag in the F register
static void set_flag(const char flag, const unsigned char value, unsigned char *regis) {

    char offset = get_flag_offset(flag);

    // Set or clear the bit
    if(value == 0) {
        *regis &= ~(1 << offset);
    } else if(value == 1) {
        *regis |= 1 << offset;          
    }
}

// Returns the value of a flag in the F register
static char get_flag(const char flag, const unsigned char regis) {

    char offset = get_flag_offset(flag);

    // Read the nth bit of the register
    return (regis >> offset) & 1;
}

// Pushes a byte to the stack after decrementing the stack pointer
static void stack_push(const unsigned char value, gbc_ram *ram, unsigned short *sp) {
   
    // Decrement the stack pointer
    *sp -= sizeof(char);

    // Write the value
    write_byte(ram, *sp, value);
}

// Pops a byte from the stack and increments the stack pointer
static unsigned char stack_pop(gbc_ram *ram, unsigned short *sp) {
    
    // Read the value
    unsigned char value = read_byte(ram, *sp);
    
    // Increment the stack pointer
    *sp += sizeof(char);

    return value;
}
