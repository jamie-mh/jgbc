#include <imgui.h>
#include "debugger/debugger.h"
#include "debugger/window_controls.h"


WindowControls::WindowControls(Debugger &debugger) : Window(debugger) {

}

void WindowControls::render() {

    if(!ImGui::Begin(title())) {
        ImGui::End();
        return;
    }

    const auto window_size = ImGui::GetContentRegionAvail();
    const auto size = ImVec2(window_size.x, 30);

    ImGui::Text("Debugger");

    if(ImGui::Button(debugger().is_paused() ? "Play" : "Pause", size))
        debugger().set_paused(!debugger().is_paused());

    if(ImGui::Button("Step Over", size))
        step_over();

    if(ImGui::Button("Step Into", size))
        step_into();

    if(ImGui::Button("Reset", size))
        Emulator::reset(debugger().gb().get());

    ImGui::End();
}

const char *WindowControls::title() const {
    return "Controls";
}

void WindowControls::step_into() {

    INIT_GB_CTX();
    const auto opcode = SREAD8(REG(PC));

    if(is_jump_call(opcode) || is_subroutine_call(opcode))
        debugger().set_next_stop(SREAD16(REG(PC) + 1));
    else if(is_jump_signed(opcode)) {

        // Address just before the jump destination (done like this because the PC hasn't been incremented yet)
        const uint16_t before_addr = REG(PC) + (int8_t) SREAD8(REG(PC) + 1);

        const auto before_instr = find_instr(gb, SREAD8(before_addr));
        debugger().set_next_stop(before_addr + before_instr.length);
    }
    else if(is_return(opcode))
        debugger().set_next_stop(PEEK16());
    else
        run_to_next();

    debugger().set_paused(false);
}

void WindowControls::step_over() {
    INIT_GB_CTX();
    const auto opcode = SREAD8(REG(PC));

    if(is_subroutine_call(opcode))
        run_to_next();
    else
        step_into();

    debugger().set_paused(false);
}

void WindowControls::run_to_next() {
    INIT_GB_CTX();
    const auto instr = Emulator::find_instr(gb, REG(PC));
    debugger().set_next_stop(REG(PC) + instr.length);
}

bool WindowControls::is_subroutine_call(const uint8_t opcode) {
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

bool WindowControls::is_jump_call(const uint8_t opcode) {
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

bool WindowControls::is_jump_signed(const uint8_t opcode) {
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

bool WindowControls::is_return(const uint8_t opcode) {
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
