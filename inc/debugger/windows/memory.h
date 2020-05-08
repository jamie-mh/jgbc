#pragma once
#include <imgui_club/imgui_memory_editor/imgui_memory_editor.h>
#include "debugger/window.h"

namespace Windows {
    class Memory final : public Window {
        public:
            explicit Memory(Debugger &);

            void render() override;
            const char *title() const override;

        private:
            static constexpr size_t REGION_COUNT = 9;
            MemoryEditor _editor;

            size_t _selected_idx;

            const uint8_t *_regions[REGION_COUNT] = { debugger().gb()->mmu.rom00, debugger().gb()->mmu.romNN, debugger().gb()->mmu.vram, debugger().gb()->mmu.extram, debugger().gb()->mmu.wram00, debugger().gb()->mmu.wramNN, debugger().gb()->mmu.oam, debugger().gb()->mmu.io, debugger().gb()->mmu.hram };
            const char *_labels[REGION_COUNT] = { "ROM 00", "ROM NN", "VRAM", "EXTRAM", "WRAM 00", "WRAM NN", "OAM", "IO", "HRAM" };
            const size_t _sizes[REGION_COUNT] = { ROM_BANK_SIZE, ROM_BANK_SIZE, VRAM_BANK_SIZE, EXTRAM_BANK_SIZE, WRAM_BANK_SIZE, WRAM_BANK_SIZE, OAM_SIZE, IO_SIZE, HRAM_SIZE };
            const size_t _offsets[REGION_COUNT] = { ROM00_START, ROMNN_START, VRAM_START, EXTRAM_START, WRAM00_START, WRAMNN_START, OAM_START, IO_START, HRAM_START };
    };
}
