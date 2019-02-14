#include "debugger/debugger.h"
#include "debugger/window_controls.h"
#include <imgui.h>

void WindowControls::render() {

	if(!is_open || !ImGui::Begin("Controls", &is_open)) {
		if(is_open) ImGui::End();
		return;
	}

    static auto gb = &_dbg.gb;

	ImGui::Text("Debugger");

	if(ImGui::Button(_dbg.is_paused ? "Play" : "Pause"))
		_dbg.is_paused = !_dbg.is_paused;

	if(ImGui::Button("Step Over"))
		step_over();

	if(ImGui::Button("Step Into"))
		step_into();

	if(ImGui::Button("Restart"))
		REG(PC) = 0x100;

    ImGui::End();
}

void WindowControls::step_into() {
    
    static const auto gb = &_dbg.gb;
    const auto opcode = SREAD8(REG(PC));

    if(is_jump_call(opcode) || is_subroutine_call(opcode)) {
        _dbg.next_stop = SREAD16(REG(PC) + 1);
    } 
    else if(is_jump_signed(opcode)) {

        // Address just before the jump destination (done like this because the PC hasn't been incremented yet)
        const uint16_t before_addr = REG(PC) + (int8_t) SREAD8(REG(PC) + 1);

        const auto before_instr = find_instr(gb, SREAD8(before_addr));
        _dbg.next_stop = before_addr + before_instr.length;
    } 
	else if(is_return(opcode)){
		_dbg.next_stop = PEEK16();
	}
    else {
		run_to_next();
    }

    _dbg.is_paused = false;
}

void WindowControls::step_over() {
    static const auto gb = &_dbg.gb;
	const auto opcode = SREAD8(REG(PC));

    if(is_subroutine_call(opcode)) {
		run_to_next();
    } else {
		step_into();
    }

    _dbg.is_paused = false;
}

void WindowControls::run_to_next() {
	static const auto gb = &_dbg.gb;
	const auto instr = Emulator::find_instr(gb, REG(PC));
	_dbg.next_stop = REG(PC) + instr.length;
}

bool WindowControls::is_subroutine_call(const uint8_t opcode) const {
    switch(opcode) {
        case 0xC4:
        case 0xCC:
        case 0xCD:
        case 0xD4:
        case 0xD3:
            return true;

        default: return false;
    }
}

bool WindowControls::is_jump_call(const uint8_t opcode) const {
    switch(opcode) {
        case 0xC2:
        case 0xC3:
        case 0xCA:
        case 0xD2:
        case 0xDA:
            return true;

        default: return false;
    }
}

bool WindowControls::is_jump_signed(const uint8_t opcode) const {
    switch(opcode) {
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
            return true;

		default: return false;
    }
}

bool WindowControls::is_return(const uint8_t opcode) const {
    switch(opcode) {
		case 0xC0:
		case 0xC8:
		case 0xC9:
		case 0xD0:
		case 0xD8:
		case 0xD9:
            return true;

		default: return false;
    }
}
