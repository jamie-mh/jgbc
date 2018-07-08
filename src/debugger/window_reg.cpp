extern "C" {
    #include "lxgbc.h"
    #include "cpu.h"
    #include "ram.h"
    #include "ppu.h"
}

#include "imgui/imgui.h"
#include "debugger/window_reg.h"
#include "imgui_dock/imgui_dock.h"

void window_reg_show(gbc_system *gbc) {

    if(ImGui::BeginDock(WINDOW_REG_TITLE)) {

        ImGui::Text("AF: %04X   LCDC: %02X", gbc->cpu->registers->AF, read_byte(gbc->ram, LCDC));
        ImGui::Text("BC: %04X   STAT: %02X", gbc->cpu->registers->BC, read_byte(gbc->ram, STAT));
        ImGui::Text("DE: %04X   LY:   %02X", gbc->cpu->registers->DE, read_byte(gbc->ram, LY));
        ImGui::Text("HL: %04X   IE:   %02X", gbc->cpu->registers->HL, read_byte(gbc->ram, IE));
        ImGui::Text("SP: %04X   IF:   %02X", gbc->cpu->registers->SP, read_byte(gbc->ram, IF));
        ImGui::Text("PC: %04X   IME:  %02X", gbc->cpu->registers->PC, gbc->cpu->registers->IME);

        ImGui::Separator();

        ImGui::Text("HALT: %02X", gbc->cpu->is_halted);
        ImGui::Text("INT: %02X", gbc->cpu->is_interrupted);
    }

    ImGui::EndDock();
}