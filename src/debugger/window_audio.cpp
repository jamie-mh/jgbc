#include "debugger/debugger.h"
#include "debugger/window_audio.h"
#include <imgui_memory_editor/imgui_memory_editor.h>
#include <imgui.h>
#include <iostream>
#include <fstream>

void WindowAudio::render() {

    if(!is_open || !ImGui::Begin("Audio", &is_open)) {
        if(is_open) ImGui::End();
        return;
    }

    const auto width = ImGui::GetContentRegionAvailWidth();
    static const auto gb = &_gb;

    ImGui::Text("APU Buffer");
    ImGui::PlotLines("Buffer", gb->apu.buffer, gb->apu.actual_spec.size / sizeof(float), 0, nullptr, 0.0f, 1.0f, ImVec2(width, 150));

    static MemoryEditor mem;
    mem.DrawContents(gb->apu.buffer, gb->apu.actual_spec.size / sizeof(float), sizeof(float));

    if(ImGui::Button("Dump")) {
        std::ofstream file;
        file.open("audio.bin");

        for(auto i = 0; i < gb->apu.actual_spec.size; ++i)
            file << gb->apu.buffer[i];

        file.close();
    }

    ImGui::End();
}
