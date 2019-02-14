#include "debugger/debugger.h"
#include "debugger/window_emulator.h"
#include <imgui.h>

void WindowEmulator::render() {

	if(!is_open || !ImGui::Begin("Emulator", &is_open)) {
		if(is_open) ImGui::End();
		return;
	}

	const auto window_size = ImGui::GetContentRegionAvail();
	const auto scale = std::min(window_size.x / SCREEN_WIDTH, window_size.y / SCREEN_HEIGHT);

	ImDrawList *draw_list = ImGui::GetWindowDrawList();
	ImVec2 pos = ImGui::GetCursorScreenPos();

	uint32_t buf_offset;
	static const uint8_t *buf = _gb.ppu.framebuffer;

	for(auto x = 0; x < SCREEN_WIDTH; x++) {
		for(auto y = 0; y < SCREEN_HEIGHT; y++) {

			buf_offset = (x * 3) + (y * SCREEN_WIDTH * 3);
			
			draw_list->AddRectFilled(
				ImVec2(pos.x + x * scale, pos.y + y * scale), 
				ImVec2(pos.x + x * scale + scale, pos.y + y * scale + scale),
				IM_COL32(
					buf[buf_offset + 0],
					buf[buf_offset + 1],
					buf[buf_offset + 2],
					0xFF
				)
			);
		}
	}

    ImGui::End();
}
