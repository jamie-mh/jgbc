extern "C" {
    #include "jgbc.h"
    #include "cpu.h"
    #include "ram.h"
    #include "ppu.h"
}

#include "imgui/imgui.h"
#include "debugger/window_reg.h"
#include "imgui_dock/imgui_dock.h"

void window_reg_show(gbc_system *gbc) {

    if(ImGui::BeginDock(WINDOW_REG_TITLE)) {

        ImGui::Text("AF: %04X   LCDC: %02X", gbc->cpu->registers->AF, read_byte(gbc, LCDC, false));
        ImGui::Text("BC: %04X   STAT: %02X", gbc->cpu->registers->BC, read_byte(gbc, STAT, false));
        ImGui::Text("DE: %04X   LY:   %02X", gbc->cpu->registers->DE, read_byte(gbc, LY, false));
        ImGui::Text("HL: %04X   IE:   %02X", gbc->cpu->registers->HL, read_byte(gbc, IE, false));
        ImGui::Text("SP: %04X   IF:   %02X", gbc->cpu->registers->SP, read_byte(gbc, IF, false));
        ImGui::Text("PC: %04X   IME:  %02X", gbc->cpu->registers->PC, gbc->cpu->registers->IME);

        ImGui::Separator();

        ImGui::Text("HALT: %02X", gbc->cpu->is_halted);
    }

    ImGui::EndDock();
}