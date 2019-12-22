#include "debugger/debugger.h"
#include "debugger/window_audio.h"
#include <imgui_memory_editor/imgui_memory_editor.h>
#include <imgui.h>
#include <iostream>
#include <fstream>

WindowAudio::WindowAudio(Emulator::GameBoy &gb): _gb(gb) {
    _editor.OptShowOptions = false;
    _editor.OptShowAscii = false;
}

void WindowAudio::render() {

    if(!_is_open || !ImGui::Begin("Audio", &_is_open)) {
        if(_is_open) ImGui::End();
        return;
    }

    const auto width = ImGui::GetContentRegionAvailWidth();

    ImGui::Text("APU Buffer");
    ImGui::PlotLines("Buffer", _gb.apu.buffer, _gb.apu.actual_spec.size / sizeof(float), 0, nullptr, 0.0f, 1.0f, ImVec2(width, 150));

    if(ImGui::Button("Dump buffer to audio.bin")) {
        std::ofstream file;
        file.open("audio.bin");

        for(size_t i = 0; i < _gb.apu.actual_spec.size; ++i)
            file << _gb.apu.buffer[i];

        file.close();
    }

    _editor.DrawContents(_gb.apu.buffer, _gb.apu.actual_spec.size / sizeof(float), sizeof(float));

    ImGui::End();
}
