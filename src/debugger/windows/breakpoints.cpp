#include "debugger/windows/breakpoints.h"
#include "debugger/colours.h"
#include "debugger/debugger.h"
#include <imgui.h>

using namespace Windows;

Breakpoints::Breakpoints(Debugger &debugger) : Window(debugger) {}

void Breakpoints::render() {

    if (!ImGui::Begin(title())) {
        ImGui::End();
        return;
    }

    INIT_GB_CTX();

    ImGui::BeginChild("##scroll");
    const ImU32 step = 1, step_fast = 50;
    static uint32_t addr = 0;
    ImGui::InputScalar("ADDR", ImGuiDataType_U32, &addr, &step, &step_fast, "%04X",
                       ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();

    if (ImGui::Button("Add"))
        debugger().add_breakpoint(addr);

    ImGui::Text("Presets");

    ImGui::Columns(2);
    const auto button_size = ImVec2(ImGui::GetColumnWidth(), 20);

    if (ImGui::Button("V-Blank", button_size))
        debugger().add_breakpoint(INT_VBLANK);

    if (ImGui::Button("LCD", button_size))
        debugger().add_breakpoint(INT_LCD_STAT);

    if (ImGui::Button("Timer", button_size))
        debugger().add_breakpoint(INT_TIMER);

    ImGui::NextColumn();
    if (ImGui::Button("Serial", button_size))
        debugger().add_breakpoint(INT_SERIAL);

    if (ImGui::Button("Joypad", button_size))
        debugger().add_breakpoint(INT_JOYPAD);

    ImGui::Columns();
    ImGui::Separator();

    if (debugger().breakpoints().empty())
        ImGui::Text("No breakpoints set");
    else {

        for (size_t i = 0; i < debugger().breakpoints().size(); ++i) {
            const auto bp = debugger().breakpoints()[i];

            ImGui::PushID(bp);
            if (ImGui::Button("X"))
                debugger().remove_breakpoint(bp);

            ImGui::SameLine();
            ImGui::Text("%zu: ", i);
            ImGui::SameLine();
            ImGui::TextColored(Colours::address, "0x%04X", bp);
            ImGui::SameLine();

            const auto instr = Emulator::find_instr(gb, bp);
            const auto opcode = SREAD8(bp);

            if (instr.length > 1 && opcode != 0xCB) {

                uint16_t operand = 0;

                if (instr.length == 2) {
                    operand = SREAD8(bp + 1);

                    // If the operand is signed, get the two's complement
                    if (instr.signed_operand)
                        operand = (~(operand - 1)) & 0x00FF;

                } else if (instr.length == 3)
                    operand = SREAD16(bp + 1);

                ImGui::Text(instr.disassembly, operand);
            }
            // If there is no operand (or CB opcode which have no operand)
            else
                ImGui::Text("%s", instr.disassembly);

            ImGui::PopID();
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

const char *Breakpoints::title() const { return "Breakpoints"; }
