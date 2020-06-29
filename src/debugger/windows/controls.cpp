#include <imgui.h>
#include "debugger/debugger.h"
#include "debugger/windows/controls.h"

using namespace Windows;

Controls::Controls(Debugger &debugger) : Window(debugger) {

}

void Controls::render() {

    if(!ImGui::Begin(title())) {
        ImGui::End();
        return;
    }

    if(ImGui::Button(debugger().is_paused() ? "Play" : "Pause")) {
        debugger().set_paused(!debugger().is_paused());
        debugger().set_next_stop(std::nullopt, std::nullopt);
    }

    ImGui::SameLine();

    if(ImGui::Button("Step Over"))
        step_over();

    ImGui::SameLine();

    if(ImGui::Button("Step Into"))
        step_into();

    ImGui::SameLine();

    if(ImGui::Button("Reset"))
        Emulator::reset(debugger().gb().get());

    ImGui::End();
}

const char *Controls::title() const {
    return "Controls";
}

void Controls::step_into() {

    INIT_GB_CTX();
    const auto opcode = SREAD8(REG(PC));

    if(is_jump_call(opcode) || is_subroutine_call(opcode))
        debugger().set_next_stop(std::nullopt, SREAD16(REG(PC) + 1));
    else if(is_jump_signed(opcode)) {

        const auto fall_thru_addr = REG(PC) + Emulator::find_instr(gb, REG(PC)).length;

        // The PC hasn't been incremented yet, add the length of the current instruction
        const auto operand = static_cast<int8_t>(SREAD8(REG(PC) + 1));
        const auto jump_addr = REG(PC) + operand + Emulator::find_instr(gb, REG(PC)).length;

        debugger().set_next_stop(fall_thru_addr, jump_addr);
    }
    else if(is_return(opcode))
        debugger().set_next_stop(PEEK16(), std::nullopt);
    else
        run_to_next();

    debugger().set_paused(false);
}

void Controls::step_over() {
    INIT_GB_CTX();
    const auto opcode = SREAD8(REG(PC));

    if(is_subroutine_call(opcode) || is_jump_call(opcode) || is_jump_signed(opcode))
        run_to_next();
    else
        step_into();

    debugger().set_paused(false);
}

void Controls::run_to_next() const {
    INIT_GB_CTX();
    const auto instr = Emulator::find_instr(gb, REG(PC));
    debugger().set_next_stop(REG(PC) + instr.length, std::nullopt);
}

bool Controls::is_subroutine_call(const uint8_t opcode) {
    switch(opcode) {
        case 0xC4:
        case 0xCC:
        case 0xCD:
        case 0xD4:
        case 0xD3:
            return true;

        default:
            return false;
    }
}

bool Controls::is_jump_call(const uint8_t opcode) {
    switch(opcode) {
        case 0xC2:
        case 0xC3:
        case 0xCA:
        case 0xD2:
        case 0xDA:
            return true;

        default:
            return false;
    }
}

bool Controls::is_jump_signed(const uint8_t opcode) {
    switch(opcode) {
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
            return true;

        default:
            return false;
    }
}

bool Controls::is_return(const uint8_t opcode) {
    switch(opcode) {
        case 0xC0:
        case 0xC8:
        case 0xC9:
        case 0xD0:
        case 0xD8:
        case 0xD9:
            return true;

        default:
            return false;
    }
}
