extern "C" {
    #include "jgbc.h"
    #include "ram.h"
}

#include "debugger/debugger.h"
#include "imgui/imgui.h"
#include "debugger/window_mem.h"
#include "imgui_dock/imgui_dock.h"
#include "imgui_memory_editor/imgui_memory_editor.h"


void window_mem_show(gbc_system *gbc) {

    if(ImGui::BeginDock(WINDOW_MEM_TITLE)) {

        static MemoryEditor mem;

        uint8_t *regions[WINDOW_MEM_REGION_COUNT] = { gbc->ram->rom00, gbc->ram->romNN, gbc->ram->vram, gbc->ram->extram, gbc->ram->wram00, gbc->ram->wramNN, gbc->ram->oam, gbc->ram->io, gbc->ram->hram };
        static const char* labels[WINDOW_MEM_REGION_COUNT] = { "ROM 00", "ROM NN", "VRAM", "EXTRAM", "WRAM 00", "WRAM NN", "OAM", "IO", "HRAM" };
        static const size_t sizes[WINDOW_MEM_REGION_COUNT] = { ROM_BANK_SIZE, ROM_BANK_SIZE, VRAM_SIZE, EXTRAM_BANK_SIZE, WRAM_BANK_SIZE, WRAM_BANK_SIZE, OAM_SIZE, IO_SIZE, HRAM_SIZE };
        static const size_t prefixes[WINDOW_MEM_REGION_COUNT] = { ROM00_START, ROMNN_START, VRAM_START, EXTRAM_START, WRAM00_START, WRAMNN_START, OAM_START, IO_START, HRAM_START };

        static int sel = 0;

        if(ImGui::BeginCombo("Region", labels[sel], 0)) {
            for(int i = 0; i < WINDOW_MEM_REGION_COUNT; i++) {
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

        // Careful not to point to non existant extram
        if(!(prefixes[sel] == EXTRAM_START && gbc->ram->extram == nullptr)) {
            mem.DrawContents(regions[sel], sizes[sel], prefixes[sel]);
        } else {
            ImGui::Text("EXTRAM not supported by cartridge");
        }
    }

    ImGui::EndDock();
}