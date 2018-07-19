extern "C" {
    #include "jgbc.h"
    #include "ram.h"
    #include "cpu.h"
}

#include "debugger/debugger.h"
#include "imgui/imgui.h"
#include "debugger/window_break.h"
#include "imgui_dock/imgui_dock.h"


void window_break_show(gbc_system *gbc, gbc_debugger *debugger) {

    if(ImGui::BeginDock(WINDOW_BREAK_TITLE)) {

        static const ImVec4 addr_colour = ImVec4(0.46f, 1.0f, 0.01f, 1.0f);

        ImGui::BeginChild("Scrolling");
        ImGui::Columns(2);
        
        ImGui::Text("Address Breakpoints");
        const ImS32 s32_one = 1, s32_fifty = 50;
        static int32_t addr = 0;

        ImGui::InputScalar("ADDR", ImGuiDataType_S32, &addr, &s32_one, &s32_fifty, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
        if(ImGui::Button("Add")) {
            add_breakpoint(addr, debugger);
        }

        if(debugger->breakpoint_head == nullptr) {
            ImGui::Text("No breakpoints set");
        } else {

            gbc_breakpoint *item = debugger->breakpoint_head;
            int i = 1;

            while(item) {
                ImGui::Text("%d: ", i);
                ImGui::SameLine();
                ImGui::TextColored(addr_colour, "0x%04X", item->address);
                ImGui::SameLine();
                ImGui::Text(find_instr(read_byte(gbc, item->address, false), item->address, gbc).disassembly, 0x0, 0x0);

                item = item->next;
                i++;
            }
        }

        ImGui::NextColumn();
        ImGui::Text("Access Breakpoints");

        ImGui::EndChild();
    }

    ImGui::EndDock();
}