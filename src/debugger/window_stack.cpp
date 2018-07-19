extern "C" {
    #include "jgbc.h"
    #include "ram.h"
}

#include "debugger/debugger.h"
#include "imgui/imgui.h"
#include "debugger/window_stack.h"
#include "imgui_dock/imgui_dock.h"
#include "imgui_memory_editor/imgui_memory_editor.h"


void window_stack_show(gbc_system *gbc) {

    if(ImGui::BeginDock(WINDOW_STACK_TITLE)) {
        ImGui::BeginChild("Scrolling");

        static const ImVec4 addr_colour = ImVec4(0.38f, 0.49f, 0.55f, 1.0f);
        const uint16_t sp = gbc->cpu->registers->SP;

        ImGuiListClipper clipper(0xFFFF - sp);
        uint16_t addr;

        while(clipper.Step()) {
            for(int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {

                addr = sp + i;

                if(addr == sp) {
                    ImGui::Text("-> ");
                } else {
                    ImGui::Text("   ");
                }

                ImGui::SameLine();
                ImGui::TextColored(addr_colour, "0x%04X: ", addr);
                ImGui::SameLine();
                ImGui::Text("%02X", read_byte(gbc, addr, false));
            }
        }

        clipper.End();
        ImGui::EndChild();
    }

    ImGui::EndDock();
}