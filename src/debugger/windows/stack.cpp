#include <imgui.h>
#include "debugger/debugger.h"
#include "debugger/colours.h"
#include "debugger/windows/stack.h"

using namespace Windows;

Stack::Stack(Debugger &debugger) : Window(debugger) {

}

void Stack::render() {

    if(!ImGui::Begin(title())) {
        ImGui::End();
        return;
    }

    INIT_GB_CTX();
    auto *const draw_list = ImGui::GetWindowDrawList();

    ImGui::BeginChild("##scroll");

    ImGuiListClipper clipper(0xFFFF - REG(SP));

    while(clipper.Step()) {
        for(auto i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {

            const uint16_t addr = REG(SP) + i;

            if(addr == REG(SP)) {
                const auto line_coords = ImGui::GetCursorScreenPos();
                const auto line_height = ImGui::GetTextLineHeight();
                const auto line_center = line_coords.y + line_height / 2;
                draw_list->AddCircle(ImVec2(line_coords.x + 12.0f, line_center), 6.0f, IM_COL32(0, 255, 0, 255));
            }

            ImGui::NewLine();
            ImGui::SameLine(30);
            ImGui::TextColored(Colours::address, "0x%04X: ", addr);
            ImGui::SameLine();
            ImGui::Text("%02X", SREAD8(addr));
        }
    }

    clipper.End();
    ImGui::EndChild();
    ImGui::End();
}

const char *Stack::title() const {
    return "Stack";
}
