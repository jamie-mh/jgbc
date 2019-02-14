#include "debugger/debugger.h"
#include "debugger/window_breakpoints.h"
#include <imgui.h>

void WindowBreakpoints::render() {

	if(!is_open || !ImGui::Begin("Breakpoints", &is_open)) {
		if(is_open) ImGui::End();
		return;
	}

	static auto gb = &_dbg.gb; // required for the c macros

	ImGui::BeginChild("##scroll");
	const ImU32 step = 1, step_fast = 50;
	static uint32_t addr = 0;
	ImGui::InputScalar("ADDR", ImGuiDataType_U32, &addr, &step, &step_fast, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::SameLine();

	if(ImGui::Button("Add"))
		_dbg.add_breakpoint(addr);

	if(_dbg.breakpoint_count() == 0)
		ImGui::Text("No breakpoints set");
	else {

		auto i = 1;
		for(const auto bp : _dbg.get_breakpoints()) {

			ImGui::PushID(bp);
			if(ImGui::Button("X"))
				_dbg.remove_breakpoint(bp);

			ImGui::SameLine();
			ImGui::Text("%d: ", i);
			ImGui::SameLine();
			ImGui::TextColored(ADDR_COLOUR, "0x%04X", bp);
			ImGui::SameLine();

			const auto instr = Emulator::find_instr(gb, bp);
			const auto opcode = SREAD8(bp);

			if(instr.length > 1 && opcode != 0xCB) {

				uint16_t operand = 0;

				if(instr.length == 2) {
					operand = SREAD8(bp + 1);

					// If the operand is signed, get the two's complement
					if(instr.signed_operand) {
						operand = (~(operand - 1)) & 0x00FF;
					}

				} else if(instr.length == 3) {
					operand = SREAD16(bp + 1);
				}

				ImGui::Text(instr.disassembly, operand);
			}
			// If there is no operand (or CB opcode which have no operand)
			else {
				ImGui::Text(instr.disassembly);
			}

			ImGui::PopID();
			i++;
		}
	}

	ImGui::EndChild();
    ImGui::End();
}
