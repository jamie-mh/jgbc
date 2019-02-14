#include "debugger/debugger.h"
#include "debugger/colours.h"
#include "debugger/window_io.h"
#include <imgui.h>

void WindowIO::render() {

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

	const char *ppu_labels[8] = { "LCDC", "STAT", "SCX", "SCY", "LY", "LYC", "WX", "WY" };
	const uint16_t ppu_addrs[8] = { LCDC, STAT, SCX, SCY, LY, LYC, WX, WY };
	draw_values(ppu_labels, ppu_addrs, 8);

	ImGui::NextColumn();
	const char *lcdc_labels[8] = 
		{ "LCD Enable", "Window Tile Map", "Window Display", "BG Tile Data", "BG Tile Map", "OBJ Size", "OBJ Display", "BG Display" };
	const uint8_t lcdc_bits[8] = 
		{ LCDC_LCD_ENABLE, LCDC_WINDOW_TILE_MAP, LCDC_WINDOW_DISPLAY, LCDC_BG_WINDOW_TILE_DATA, LCDC_BG_TILE_MAP, LCDC_OBJ_SIZE, LCDC_OBJ_DISPLAY, LCDC_BG_DISPLAY };
	draw_registers(lcdc_labels, LCDC, lcdc_bits, 8);

	ImGui::NextColumn();
	const char *stat_labels[7] = 
		{ "Coincidence Interrupt", "OAM Interrupt", "V-Blank Interrupt", "H-Blank Interrupt", "Coincidence Flag", "Mode (bit 1)", "Mode (bit 0)" };
	const uint8_t stat_bits[7] = 
		{ STAT_COINCID_INT, STAT_OAM_INT, STAT_VBLANK_INT, STAT_HBLANK_INT, STAT_COINCID_FLAG, STAT_MODE1, STAT_MODE0 };
	draw_registers(stat_labels, STAT, stat_bits, 7);

	ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("Interrupt Enable");
	ImGui::SameLine();
	ImGui::TextColored(Colours::data, "(= %02X)", SREAD8(IE));
	ImGui::NextColumn();
	ImGui::Text("Interrupt Flag");
	ImGui::SameLine();
	ImGui::TextColored(Colours::data, "(= %02X)", SREAD8(IF));
	ImGui::NextColumn();
	ImGui::Text("Timer");
	ImGui::NextColumn();
	ImGui::Separator();

	const char *ief_labels[5] = { "V-Blank", "LCD Stat", "Timer", "Serial", "Joypad" };
	const uint8_t ief_bits[5] = { IEF_VBLANK, IEF_LCD_STAT, IEF_TIMER, IEF_SERIAL, IEF_JOYPAD };

	draw_registers(ief_labels, IE, ief_bits, 5);
	ImGui::NextColumn();
	draw_registers(ief_labels, IF, ief_bits, 5);

	ImGui::NextColumn();
	const char *timer_labels[8] = { "DIV", "TIMA", "TMA", "TAC" };
	const uint16_t timer_addrs[8] = { DIV, TIMA, TMA, TAC };
	draw_values(timer_labels, timer_addrs, 4);

	const char *tac_labels[3] = { "Input (bit 1)", "Input (bit 0)", "Stop (Checked = running)" };
	const uint8_t tac_bits[3] = { TAC_INPUT1, TAC_INPUT0, TAC_STOP };
	draw_registers(tac_labels, TAC, tac_bits, 3);

	ImGui::NextColumn();
	ImGui::Separator();

    ImGui::End();
}

inline void WindowIO::draw_values(const char **labels, const uint16_t *addrs, const int count) const {
    static const auto gb = &_gb;
	static const ImU32 step = 1, step_fast = 10;

	for(auto i = 0; i < count; ++i) {
		ImGui::TextColored(Colours::address, "%04X", addrs[i]);
		ImGui::SameLine();

		int value = SREAD8(addrs[i]);
		if(ImGui::InputScalar(labels[i], ImGuiDataType_U32, &value, &step, &step_fast, "%02X", ImGuiInputTextFlags_CharsHexadecimal))
			SWRITE8(addrs[i], value);
	}
}

inline void WindowIO::draw_registers(const char **labels, const uint16_t regis, const uint8_t *bits, const int count) const {
    static const auto gb = &_gb;

	for(auto i = 0; i < count; ++i) {
		bool checked = Emulator::read_register(gb, regis, bits[i]);
		if(ImGui::Checkbox(labels[i], &checked))
			Emulator::write_register(gb, regis, bits[i], checked);
	}
}
