#include <imgui.h>
#include "debugger/debugger.h"
#include "debugger/windows/palettes.h"

using namespace Windows;

Palettes::Palettes(Debugger &debugger) : Window(debugger) {

}

void Palettes::render() {

    if(!ImGui::Begin(title())) {
        ImGui::End();
        return;
    }

    ImGui::Text("Colour Palettes");
    ImGui::Separator();
    ImGui::Columns(2, nullptr, true);

    for(auto palette = 0; palette < 8; ++palette) {

        for(auto type = 0; type < 2; ++type)
        {
            if(type == 0)
                ImGui::Text("BG %d: ", palette);
            else
                ImGui::Text("OBJ %d: ", palette);

            for(auto i = 0; i < 4; ++i)
            {
                ImGui::SameLine();

                const auto colour = type == 0
                    ? debugger().gb()->ppu.bg_palette[palette * 4 + i]
                    : debugger().gb()->ppu.obj_palette[palette * 4 + i];

                ImVec4 colour_vec;
                colour_vec.x = (colour & 0x1F) / 31.0f;
                colour_vec.y = ((colour & 0x3E0) >> 5) / 31.0f;
                colour_vec.z = ((colour & 0x7C00) >> 10) / 31.0f;

                ImGui::ColorButton(
                    "##button",
                    colour_vec,
                    ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker,
                    ImVec2(30, 30)
                );
            }

            ImGui::NextColumn();
        }
    }
    
    ImGui::End();
}

const char *Palettes::title() const {
    return "Palettes";
}
