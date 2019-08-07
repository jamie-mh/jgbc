#include "debugger/debugger.h"
#include "debugger/window_memory.h"
#include <imgui.h>
#include <imgui_memory_editor/imgui_memory_editor.h>

void WindowMemory::render() {

    if(!is_open || !ImGui::Begin("Memory", &is_open)) {
        if(is_open) ImGui::End();
        return;
    }

    static const auto gb = &_gb;
    static MemoryEditor mem;

    unsigned char *regions[MEMORY_REGION_COUNT] = { gb->mmu.rom00, gb->mmu.romNN, gb->mmu.vram, gb->mmu.extram, gb->mmu.wram00, gb->mmu.wramNN, gb->mmu.oam, gb->mmu.io, gb->mmu.hram };
    static const char* labels[MEMORY_REGION_COUNT] = { "ROM 00", "ROM NN", "VRAM", "EXTRAM", "WRAM 00", "WRAM NN", "OAM", "IO", "HRAM" };
    static const size_t sizes[MEMORY_REGION_COUNT] = { ROM_BANK_SIZE, ROM_BANK_SIZE, VRAM_BANK_SIZE, EXTRAM_BANK_SIZE, WRAM_BANK_SIZE, WRAM_BANK_SIZE, OAM_SIZE, IO_SIZE, HRAM_SIZE };
    static const size_t prefixes[MEMORY_REGION_COUNT] = { ROM00_START, ROMNN_START, VRAM_START, EXTRAM_START, WRAM00_START, WRAMNN_START, OAM_START, IO_START, HRAM_START };

    static int sel = 0;

    if(ImGui::BeginCombo("Region", labels[sel], 0)) {
        for(int i = 0; i < MEMORY_REGION_COUNT; i++) {
            bool is_selected = (i == sel);

            if(ImGui::Selectable(labels[i], is_selected)) {
                sel = i;
            }

            if(is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    // Careful not to point to non existent extram
    if(!(prefixes[sel] == EXTRAM_START && gb->mmu.extram == nullptr)) {
        mem.DrawContents(regions[sel], sizes[sel], prefixes[sel]);
    } else {
        ImGui::Text("EXTRAM not available.");
    }

    ImGui::End();
}
