#pragma once

#define MAX(a, b) ((a) > (b) ? (a) : (b))

namespace ImGui {

    // Plot value over time
    // Pass FLT_MAX value to draw without adding a new value
    IMGUI_API void	PlotVar(const char* label, float value, float scale_min = FLT_MAX, float scale_max = FLT_MAX, size_t buffer_size = 120);

    // Call this periodically to discard old/unused data
    IMGUI_API void	PlotVarFlushOldEntries();

}