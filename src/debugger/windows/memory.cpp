#include "debugger/windows/memory.h"
#include "debugger/debugger.h"
#include <imgui.h>

using namespace Windows;

Memory::Memory(Debugger &debugger) : Window(debugger) { _selected_idx = 0; }

void Memory::render() {
    if (!ImGui::Begin(title())) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginCombo("Region", _labels[_selected_idx], 0)) {
        for (size_t i = 0; i < REGION_COUNT; i++) {
            const auto is_selected = i == _selected_idx;

            if (ImGui::Selectable(_labels[i], is_selected)) {
                _selected_idx = i;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    // Careful not to point to non existent extram
    if (!(_offsets[_selected_idx] == EXTRAM_START && debugger().gb()->mmu.extram == nullptr)) {
        _editor.DrawContents((void *) _regions[_selected_idx], _sizes[_selected_idx], _offsets[_selected_idx]);
    } else {
        ImGui::Text("EXTRAM not available.");
    }

    ImGui::End();
}

constexpr const char *Memory::title() const { return "Memory"; }
