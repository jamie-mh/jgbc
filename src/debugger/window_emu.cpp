extern "C" {
    #include "lxgbc.h"
    #include "ppu.h"
}

#include "imgui/imgui.h"
#include "debugger/debugger.h"
#include "debugger/window_emu.h"
#include "imgui_dock/imgui_dock.h"


void window_emu_show(gbc_system *gbc) {

    if(ImGui::BeginDock(WINDOW_EMU_TITLE)) {

        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetWindowPos();

        unsigned int buf_offset;
        for(int x = 0; x < SCREEN_WIDTH; x++) {
            for(int y = 0; y < SCREEN_HEIGHT; y++) {

                buf_offset = (x * 3) + (y * SCREEN_WIDTH * 3);
                
                draw_list->AddRectFilled(
                    ImVec2(pos.x + x * DEBUGGER_SCALE, pos.y + y * DEBUGGER_SCALE), 
                    ImVec2(pos.x + x * DEBUGGER_SCALE + DEBUGGER_SCALE, pos.y + y * DEBUGGER_SCALE + DEBUGGER_SCALE),
                    IM_COL32(
                        gbc->ppu->framebuffer[buf_offset + 0],
                        gbc->ppu->framebuffer[buf_offset + 1],
                        gbc->ppu->framebuffer[buf_offset + 2],
                        0xFF
                    )
                );
            }
        }
    }

    ImGui::EndDock();
}
