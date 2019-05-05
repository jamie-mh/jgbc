#include "debugger/debugger.h"
#include "debugger/colours.h"
#include "debugger/window_cart_info.h"
#include <imgui.h>

void WindowCartInfo::render() {

    if(!is_open || !ImGui::Begin("Cart Info", &is_open)) {
        if(is_open) ImGui::End();
        return;
    }

    const auto &style = ImGui::GetStyle();

    ImGui::TextColored(Colours::address, "Title");
    ImGui::SameLine();
    ImGui::Text("%s", _gb.cart.title);

    ImGui::TextColored(Colours::address, "GBC Flag");
    ImGui::SameLine();
    ImGui::Text("%02X", _gb.cart.gbc_flag);

    ImGui::TextColored(Colours::address, "Cartridge Type");
    ImGui::SameLine();
    ImGui::Text("%02X", _gb.cart.type);

    ImGui::TextColored(Colours::address, "ROM Size");
    ImGui::SameLine();
    ImGui::Text("%d x %d KB", _gb.cart.rom_size, ROM_BANK_SIZE);

    ImGui::TextColored(Colours::address, "RAM Size");
    ImGui::SameLine();
    ImGui::Text("%d x %d KB", _gb.cart.ram_size, EXTRAM_BANK_SIZE);

    ImGui::End();
}
