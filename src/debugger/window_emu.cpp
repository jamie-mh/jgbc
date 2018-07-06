extern "C" {
    #include "lxgbc.h"
    #include "ppu.h"
}

#include "imgui.h"
#include "window_emu.h"

static void window_emu_start();


void window_emu_tick(gbc_system *gbc) {
    window_emu_start();

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetWindowPos();

    unsigned int buf_offset;
    for(int x = 0; x < SCREEN_WIDTH; x++) {
        for(int y = 0; y < SCREEN_HEIGHT; y++) {

            buf_offset = (x * 4) + (y * SCREEN_WIDTH * 4);
            
            draw_list->AddRectFilled(
                ImVec2(pos.x + x * SCREEN_SCALE, pos.y + y * SCREEN_SCALE), 
                ImVec2(pos.x + x * SCREEN_SCALE + SCREEN_SCALE, pos.y + y * SCREEN_SCALE + SCREEN_SCALE),
                IM_COL32(
                    gbc->ppu->framebuffer[buf_offset + 0],
                    gbc->ppu->framebuffer[buf_offset + 1],
                    gbc->ppu->framebuffer[buf_offset + 2],
                    gbc->ppu->framebuffer[buf_offset + 3]
                )
            );
        }
    }

    ImGui::End();
}

static void window_emu_start() {

    ImGui::SetNextWindowSize(
        ImVec2(SCREEN_WIDTH * SCREEN_SCALE, SCREEN_HEIGHT * SCREEN_SCALE), 
        ImGuiCond_Always
    );
    ImGui::SetNextWindowPos(WINDOW_EMU_POS, ImGuiCond_FirstUseEver);

    ImGui::Begin(
        WINDOW_EMU_TITLE, 
        NULL, 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
    );
}