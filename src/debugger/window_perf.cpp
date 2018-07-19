extern "C" {
    #include "jgbc.h"
    #include "cpu.h"
}

#include "debugger/debugger.h"
#include "imgui/imgui.h"
#include "debugger/window_perf.h"
#include "imgui_dock/imgui_dock.h"
#include "imgui_plot/imgui_plot.h"

void window_perf_show(gbc_debugger *debugger) {
    
    if(ImGui::BeginDock(WINDOW_PERF_TITLE)) {

        static const ImVec4 colour_bad = ImVec4(0.9f, 0.29f, 0.1f, 1.0f);
        static const ImVec4 colour_okay = ImVec4(1.0f, 0.92f, 0.23f, 1.0f);
        static const ImVec4 colour_good = ImVec4(0.40f, 0.73f, 0.42f, 1.0f);

        const float framerate = ImGui::GetIO().Framerate;

        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
        ImGui::PlotVar("Frame Times", 1000.0f / framerate);
        ImGui::PlotVar("Framerate", framerate);

        if(framerate >= 0.0f && framerate < 20.0f) {
            ImGui::TextColored(colour_bad, "BAD FRAMERATE");
        } else if(framerate > 20.0f && framerate < 40.0f) {
            ImGui::TextColored(colour_okay, "OKAY FRAMERATE");
        } else if(framerate >= 40.0f) {
            ImGui::TextColored(colour_good, "GOOD FRAMERATE");
        }

        ImGui::Separator();
        ImGui::Text("CLOCKS: %llu", debugger->clocks);
    }

    ImGui::EndDock();
}