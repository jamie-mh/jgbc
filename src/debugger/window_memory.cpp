#include <imgui.h>
#include "debugger/debugger.h"
#include "debugger/window_memory.h"


WindowMemory::WindowMemory(Debugger &debugger) : Window(debugger) {
    _selected_idx = 0;
}

void WindowMemory::render() {

    if(!ImGui::Begin(title(), nullptr)) {
        ImGui::End();
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
    if(!(_offsets[_selected_idx] == EXTRAM_START && debugger().gb()->mmu.extram == nullptr))
        _editor.DrawContents((void *) _regions[_selected_idx], _sizes[_selected_idx], _offsets[_selected_idx]);
    else
        ImGui::Text("EXTRAM not available.");

    ImGui::End();
}

const char *WindowMemory::title() const {
    return "Memory";
}
