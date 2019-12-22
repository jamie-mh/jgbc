#include "debugger/debugger.h"
#include "debugger/window_memory.h"
#include <imgui.h>

WindowMemory::WindowMemory(Emulator::GameBoy &gb): _gb(gb) {
    _selected_idx = 0;
};

void WindowMemory::render() {

    if(!_is_open || !ImGui::Begin("Memory", &_is_open)) {
        if(_is_open) ImGui::End();
        return;
    }

    if(ImGui::BeginCombo("Region", _labels[_selected_idx], 0)) {
        for(size_t i = 0; i < REGION_COUNT; i++) {
            bool is_selected = i == _selected_idx;

            if(ImGui::Selectable(_labels[i], is_selected))
                _selected_idx = i;

            if(is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    // Careful not to point to non existent extram
    if(!(_offsets[_selected_idx] == EXTRAM_START && _gb.mmu.extram == nullptr))
        _editor.DrawContents((void *) _regions[_selected_idx], _sizes[_selected_idx], _offsets[_selected_idx]);
    else
        ImGui::Text("EXTRAM not available.");

    ImGui::End();
}

