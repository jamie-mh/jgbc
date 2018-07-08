extern "C" {
    #include "lxgbc.h"
    #include "cpu.h"
    #include "ram.h"
}

#include "imgui/imgui.h"
#include "debugger/debugger.h"
#include "debugger/window_ctrl.h"
#include "imgui_dock/imgui_dock.h"

static void step_into(gbc_system *, gbc_debugger *);
static void step_over(gbc_system *, gbc_debugger *);

static bool is_subroutine_call(const unsigned char);
static bool is_jump_call(const unsigned char);
static bool is_jump_signed(const unsigned char);
static void run_to_next(gbc_system *, gbc_debugger *);


void window_ctrl_show(gbc_system *gbc, gbc_debugger *debugger) {

    if(ImGui::BeginDock(WINDOW_CTRL_TITLE)) {

        ImGui::Columns(2);
        ImGui::Text("Debugger");

        if(ImGui::Button((debugger->is_paused) ? "Play" : "Pause")) {
            debugger->is_paused = !debugger->is_paused;
        }

        if(ImGui::Button("Step Over")) {
            step_over(gbc, debugger);
        }

        if(ImGui::Button("Step Into")) {
            step_into(gbc, debugger);
        }

        if(ImGui::Button("Restart")) {
            gbc->cpu->registers->PC = PROGRAM_START;
        }

        ImGui::NextColumn();
        ImGui::Text("View");

        // const ImS32 s32_one = 1, s32_fifty = 50;
        // static int addr = 0;

        // ImGui::InputScalar("ADDR", ImGuiDataType_S32, &addr, &s32_one, &s32_fifty, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
        // if(ImGui::Button("Goto")) {
            
        // }
    }
    
    ImGui::EndDock();
}

static void step_into(gbc_system *gbc, gbc_debugger *debugger) {
    unsigned char opcode = read_byte(gbc->ram, gbc->cpu->registers->PC);

    if(is_jump_call(opcode) || is_subroutine_call(opcode)) {
        debugger->next_addr = read_short(gbc->ram, gbc->cpu->registers->PC + 1);
    } else if(is_jump_signed(opcode)) {

        // Address just before the jump destination (done like this because the PC hasn't been incremented yet)
        const unsigned short before_addr = gbc->cpu->registers->PC + (signed char) read_byte(gbc->ram, gbc->cpu->registers->PC + 1);

        gbc_instruction before_instr = find_instr(
            read_byte(gbc->ram, before_addr),
            before_addr,
            gbc
        );

        debugger->next_addr = before_addr + before_instr.length;
    } else {
        run_to_next(gbc, debugger);
    }

    debugger->is_paused = false;
}

static void step_over(gbc_system *gbc, gbc_debugger *debugger) {
    const unsigned char opcode = read_byte(gbc->ram, gbc->cpu->registers->PC);

    if(is_jump_call(opcode)) {
        debugger->next_addr = read_short(gbc->ram, gbc->cpu->registers->PC + 1);
    } else {
        run_to_next(gbc, debugger);
    }

    debugger->is_paused = false;
}

static bool is_subroutine_call(const unsigned char op) {
    switch(op) {
        case 0xC4:
        case 0xCC:
        case 0xCD:
        case 0xD4:
        case 0xD3:
            return true;
    }
    return false;
}

static bool is_jump_call(const unsigned char op) {
    switch(op) {
        case 0xC2:
        case 0xC3:
        case 0xCA:
        case 0xD2:
        case 0xDA:
            return true;
    }
    return false;
}

static bool is_jump_signed(const unsigned char op) {
    switch(op) {
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
            return true;
    }
    return false;
}

static void run_to_next(gbc_system *gbc, gbc_debugger *debugger) {
    gbc_instruction instr = find_instr(
        read_byte(gbc->ram, gbc->cpu->registers->PC),
        gbc->cpu->registers->PC,
        gbc
    );
    debugger->next_addr = gbc->cpu->registers->PC + instr.length;
}