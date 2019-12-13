#include "debugger/debugger.h"
#include "debugger/window_palettes.h"
#include <imgui.h>


WindowPalettes::WindowPalettes(Emulator::GameBoy &gb) : _gb(gb) {

}

void WindowPalettes::render() {

    if(!is_open || !ImGui::Begin("Palettes", &is_open)) {
        if(is_open) ImGui::End();
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
                    ? _gb.ppu.bg_palette[palette * 4 + i]
                    : _gb.ppu.obj_palette[palette * 4 + i];

                ImGui::ColorButton(
                    "##button",
                    ImVec4(colour.r / 31.0f, colour.g / 31.0f, colour.b / 31.0f, 0),
                    ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker,
                    ImVec2(30, 30)
                );
            }

            ImGui::NextColumn();
        }
    }
    
    ImGui::End();
}
