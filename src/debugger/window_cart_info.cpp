#include <imgui.h>
#include "debugger/debugger.h"
#include "debugger/colours.h"
#include "debugger/window_cart_info.h"


WindowCartInfo::WindowCartInfo(Debugger &debugger) : Window(debugger) {

}

void WindowCartInfo::render() {

    if(!ImGui::Begin(title(), nullptr)) {
        ImGui::End();
        return;
    }

    ImGui::TextColored(Colours::address, "Title:");
    ImGui::SameLine();
    ImGui::Text("%s", debugger().gb()->cart.title);

    ImGui::TextColored(Colours::address, "Colour:");
    ImGui::SameLine();
    ImGui::Text("%s", debugger().gb()->cart.is_colour ? "yes" : "no");

    ImGui::TextColored(Colours::address, "Cartridge Type:");
    ImGui::SameLine();
    ImGui::Text("%02X", debugger().gb()->cart.type);

    ImGui::TextColored(Colours::address, "ROM Size:");
    ImGui::SameLine();
    ImGui::Text("%d x %d KB", debugger().gb()->cart.rom_size, ROM_BANK_SIZE);

    ImGui::TextColored(Colours::address, "RAM Size:");
    ImGui::SameLine();
    ImGui::Text("%d x %d KB", debugger().gb()->cart.ram_size, EXTRAM_BANK_SIZE);

    ImGui::End();
}

const char *WindowCartInfo::title() const {
    return "Cartridge Info";
}
