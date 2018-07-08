extern "C" {
    #include "lxgbc.h"
    #include "ram.h"
    #include "rom.h"
}

#include "debugger/debugger.h"
#include "imgui/imgui.h"
#include "debugger/modal_rom.h"
#include "imgui_memory_editor/imgui_memory_editor.h"


void modal_rom_show(gbc_system *gbc, gbc_debugger *debugger) {
    ImGui::OpenPopup(MODAL_ROM_TITLE);

    if(ImGui::BeginPopupModal(MODAL_ROM_TITLE, &debugger->modal_rom_open, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::Text("Title: %s", gbc->rom->title);
        ImGui::Text("CGB Flag: %02X", gbc->rom->cgb_flag);
        ImGui::Text("Cartridge Type: %02X", gbc->rom->cart_type);
        ImGui::Text("ROM Size: %d x %d KB", gbc->rom->rom_size, ROM_BANK_SIZE);
        ImGui::Text("RAM Size: %d x %d KB", gbc->rom->ram_size, EXTRAM_BANK_SIZE);
        ImGui::Text("Destination Code: %02X", gbc->rom->dest_code);
        ImGui::Text("Version Number: %02X", gbc->rom->ver_no);

        if(ImGui::Button("OK", ImVec2(120,0))) { 
            debugger->modal_rom_open = false;
        }
        ImGui::EndPopup();
    }
}