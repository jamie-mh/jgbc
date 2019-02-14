#include "debugger/debugger.h"
#include "debugger/colours.h"
#include "debugger/window_registers.h"
#include <imgui.h>

void WindowRegisters::render() {

	if(!is_open || !ImGui::Begin("Registers", &is_open)) {
		if(is_open) ImGui::End();
		return;
	}

    static const auto gb = &_gb;

	ImGui::Columns(2, nullptr, true);

	ImGui::Text("CPU");
	ImGui::NextColumn();
	ImGui::Text("IO");
	ImGui::NextColumn();
	ImGui::Separator();

	const char *cpu_reg_labels[6] = { "AF", "BC", "DE", "HL", "SP", "PC" };
	uint16_t *cpu_reg_addr[6] = { &REG(AF), &REG(BC), &REG(DE), &REG(HL), &REG(SP), &REG(PC) };

	const ImU32 step = 1, step_fast = 10;
	for(auto i = 0; i < 6; ++i) {
		int value = *cpu_reg_addr[i];
		if(ImGui::InputScalar(cpu_reg_labels[i], ImGuiDataType_U32, &value, &step, &step_fast, "%04X", ImGuiInputTextFlags_CharsHexadecimal))
			*cpu_reg_addr[i] = value;
	}

	ImGui::NextColumn();
	const char *io_reg_labels[5] = { "LCDC", "STAT", "LY", "IE", "IF" };
	const uint16_t io_reg_addr[5] = { LCDC, STAT, LY, IE, IF };

	for(auto i = 0; i < 5; ++i) {
		int value = io_reg_addr[i];
		if(ImGui::InputScalar(io_reg_labels[i], ImGuiDataType_U32, &value, &step, &step_fast, "%02X", ImGuiInputTextFlags_CharsHexadecimal))
			SWRITE8(io_reg_addr[i], value);
	}

	bool ime = REG(IME);
	if(ImGui::Checkbox("IME", &ime))
		REG(IME) = ime;

	ImGui::Columns(1);
	ImGui::Separator();

	bool flag_zero = Emulator::get_flag(FLAG_ZERO, REG(F));
	bool flag_subtract = Emulator::get_flag(FLAG_SUBTRACT, REG(F));
	bool flag_halfcarry = Emulator::get_flag(FLAG_HALFCARRY, REG(F));
	bool flag_carry = Emulator::get_flag(FLAG_CARRY, REG(F));

	ImGui::Text("FLAGS");
	if(ImGui::Checkbox("Z", &flag_zero))
		Emulator::set_flag(FLAG_ZERO, flag_zero, &REG(F));

	ImGui::SameLine();
	if(ImGui::Checkbox("N", &flag_subtract))
		Emulator::set_flag(FLAG_SUBTRACT, flag_subtract, &REG(F));

	ImGui::SameLine();
	if(ImGui::Checkbox("H", &flag_halfcarry))
		Emulator::set_flag(FLAG_HALFCARRY, flag_halfcarry, &REG(F));

	ImGui::SameLine();
	if(ImGui::Checkbox("C", &flag_carry))
		Emulator::set_flag(FLAG_CARRY, flag_carry, &REG(F));

	ImGui::Separator();
	ImGui::Text("Halted? %s", _gb.cpu.is_halted ? "yes" : "no");

    ImGui::End();
}
