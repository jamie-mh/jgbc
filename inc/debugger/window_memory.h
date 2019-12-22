#pragma once
#include <imgui_club/imgui_memory_editor/imgui_memory_editor.h>
#include "window.h"

class WindowMemory final : public Window {
    private:
        static constexpr size_t REGION_COUNT = 9;

        Emulator::GameBoy &_gb;
        MemoryEditor _editor;

        size_t _selected_idx;

        const uint8_t *_regions[REGION_COUNT] = { _gb.mmu.rom00, _gb.mmu.romNN, _gb.mmu.vram, _gb.mmu.extram, _gb.mmu.wram00, _gb.mmu.wramNN, _gb.mmu.oam, _gb.mmu.io, _gb.mmu.hram };
        const char *_labels[REGION_COUNT] = { "ROM 00", "ROM NN", "VRAM", "EXTRAM", "WRAM 00", "WRAM NN", "OAM", "IO", "HRAM" };
        const size_t _sizes[REGION_COUNT] = { ROM_BANK_SIZE, ROM_BANK_SIZE, VRAM_BANK_SIZE, EXTRAM_BANK_SIZE, WRAM_BANK_SIZE, WRAM_BANK_SIZE, OAM_SIZE, IO_SIZE, HRAM_SIZE };
        const size_t _offsets[REGION_COUNT] = { ROM00_START, ROMNN_START, VRAM_START, EXTRAM_START, WRAM00_START, WRAMNN_START, OAM_START, IO_START, HRAM_START };

    public:
        explicit WindowMemory(Emulator::GameBoy &);
        void render() override;
};
