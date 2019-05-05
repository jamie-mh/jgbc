#include "debugger/debugger.h"
#include "debugger/window_audio.h"
#include <imgui.h>

void WindowAudio::render() {

    if(!is_open || !ImGui::Begin("Audio", &is_open)) {
        if(is_open) ImGui::End();
        return;
    }

    const auto width = ImGui::GetContentRegionAvailWidth();
    static const auto gb = &_gb;

    ImGui::Text("APU Buffer");
    ImGui::PlotLines("", gb->apu.buffer.data, gb->apu.buffer.size / sizeof(float), 0, "", -1.0f, 1.0f, ImVec2(width, 150));

    ImGui::End();
}
