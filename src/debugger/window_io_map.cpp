#include "debugger/debugger.h"
#include "debugger/colours.h"
#include "debugger/window_io_map.h"
#include <imgui.h>

void WindowIOMap::render() {

	if(!is_open || !ImGui::Begin("IO Map", &is_open)) {
		if(is_open) ImGui::End();
		return;
	}

    static const auto gb = &_gb;

	ImGui::Columns(3, nullptr, true);

	ImGui::Text("PPU");
	ImGui::NextColumn();
	ImGui::Text("LCDC");
	ImGui::SameLine();
	ImGui::TextColored(Colours::data, "(= %02X)", SREAD8(LCDC));
	ImGui::NextColumn();
	ImGui::Text("STAT");
	ImGui::SameLine();
	ImGui::TextColored(Colours::data, "(= %02X)", SREAD8(STAT));
	ImGui::NextColumn();
	ImGui::Separator();

	const ImU32 step = 1, step_fast = 10;
	const char *ppu_labels[8] = { "LCDC", "STAT", "SCX", "SCY", "LY", "LYC", "WX", "WY" };
	const uint16_t ppu_addrs[8] = { LCDC, STAT, SCX, SCY, LY, LYC, WX, WY };

	for(auto i = 0; i < 8; ++i) {
		ImGui::TextColored(Colours::address, "%04X", ppu_addrs[i]);
		ImGui::SameLine();

		int value = SREAD8(ppu_addrs[i]);
		if(ImGui::InputScalar(ppu_labels[i], ImGuiDataType_U32, &value, &step, &step_fast, "%02X", ImGuiInputTextFlags_CharsHexadecimal))
			SWRITE8(ppu_addrs[i], value);
	}

	ImGui::NextColumn();
	const char *lcdc_labels[8] = { "LCD Enable", "Window Tile Map", "Window Display", "BG Tile Data", "BG Tile Map", "OBJ Size", "OBJ Display", "BG Display" };
	const uint8_t lcdc_regis[8] = { LCDC_LCD_ENABLE, LCDC_WINDOW_TILE_MAP, LCDC_WINDOW_DISPLAY, LCDC_BG_WINDOW_TILE_DATA, LCDC_BG_TILE_MAP, LCDC_OBJ_SIZE, LCDC_OBJ_DISPLAY, LCDC_BG_DISPLAY };

	for(auto i = 0; i < 8; ++i) {
		bool checked = Emulator::read_register(gb, LCDC, lcdc_regis[i]);
		if(ImGui::Checkbox(lcdc_labels[i], &checked))
			Emulator::write_register(gb, LCDC, lcdc_regis[i], checked);
	}

	ImGui::NextColumn();
	const char *stat_labels[7] = { "Coincidence Interrupt", "OAM Interrupt", "V-Blank Interrupt", "H-Blank Interrupt", "Coincidence Flag", "Mode (bit 1)", "Mode (bit 0)" };
	const uint8_t stat_regis[7] = { STAT_COINCID_INT, STAT_OAM_INT, STAT_VBLANK_INT, STAT_HBLANK_INT, STAT_COINCID_FLAG, STAT_MODE1, STAT_MODE0 };

	for(auto i = 0; i < 7; ++i) {
		bool checked = Emulator::read_register(gb, STAT, stat_regis[i]);
		if(ImGui::Checkbox(stat_labels[i], &checked))
			Emulator::write_register(gb, STAT, stat_regis[i], checked);
	}

	ImGui::NextColumn();
	ImGui::Separator();

    ImGui::End();
}
