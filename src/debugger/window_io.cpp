#include <imgui.h>
#include "debugger/debugger.h"
#include "debugger/colours.h"
#include "debugger/window_io.h"


WindowIO::WindowIO(Debugger &debugger) : Window(debugger) {

}

void WindowIO::render() {

    if(!ImGui::Begin(title())) {
        ImGui::End();
        return;
    }

    INIT_GB_CTX();

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
    const char *timer_labels[4] = { "DIV", "TIMA", "TMA", "TAC" };
    const uint16_t timer_addrs[4] = { DIV, TIMA, TMA, TAC };
    draw_values(timer_labels, timer_addrs, 4);

    const char *tac_labels[3] = { "Input (bit 1)", "Input (bit 0)", "Stop (Checked = running)" };
    const uint8_t tac_bits[3] = { TAC_INPUT1, TAC_INPUT0, TAC_STOP };
    draw_registers(tac_labels, TAC, tac_bits, 3);

    ImGui::NextColumn();
    ImGui::Separator();
    ImGui::Text("Square NR1X");
    ImGui::NextColumn();
    ImGui::Text("Square NR2X");
    ImGui::NextColumn();
    ImGui::Text("Wave NR3X");
    ImGui::Separator();
    ImGui::NextColumn();

    ImGui::Checkbox("Enabled", &gb->apu.square_waves[0].enabled);
    const char *square_1_labels[5] = { "NR10", "NR11", "NR12", "NR13", "NR14" };
    const uint16_t square_1_addrs[5] = { NR10, NR11, NR12, NR13, NR14 };
    draw_values(square_1_labels, square_1_addrs, 5);

    ImGui::NextColumn();
    ImGui::Checkbox("Enabled", &gb->apu.square_waves[1].enabled);
    const char *square_2_labels[4] = { "NR21", "NR22", "NR23", "NR24" };
    const uint16_t square_2_addrs[4] = { NR21, NR22, NR23, NR24 };
    draw_values(square_2_labels, square_2_addrs, 4);

    ImGui::NextColumn();
    ImGui::Checkbox("Enabled", &gb->apu.wave.enabled);
    const char *wave_labels[4] = { "NR31", "NR32", "NR33", "NR34" };
    const uint16_t wave_addrs[4] = { NR31, NR32, NR33, NR34 };
    draw_values(wave_labels, wave_addrs, 4);

    ImGui::NextColumn();
    ImGui::Separator();
    ImGui::Text("Noise NR4X");
    ImGui::Separator();

    ImGui::Checkbox("Enabled", &gb->apu.noise.enabled);
    const char *noise_labels[4] = { "NR41", "NR42", "NR43", "NR44" };
    const uint16_t noise_addrs[4] = { NR41, NR42, NR43, NR44 };
    draw_values(noise_labels, noise_addrs, 4);

    ImGui::Separator();
    ImGui::End();
}

const char *WindowIO::title() const {
    return "IO Map";
}

void WindowIO::draw_values(const char **labels, const uint16_t *addrs, const int count) {
    INIT_GB_CTX();
    static const ImU32 step = 1, step_fast = 10;

    for(auto i = 0; i < count; ++i) {
        ImGui::TextColored(Colours::address, "%04X", addrs[i]);
        ImGui::SameLine();

        int value = SREAD8(addrs[i]);
        if(ImGui::InputScalar(labels[i], ImGuiDataType_U32, &value, &step, &step_fast, "%02X", ImGuiInputTextFlags_CharsHexadecimal))
            WRITE8(addrs[i], value);
    }
}

void WindowIO::draw_registers(const char **labels, const uint16_t regis, const uint8_t *bits, const int count) {
    INIT_GB_CTX();

    for(auto i = 0; i < count; ++i) {
        bool checked = Emulator::read_register(gb, regis, bits[i]);
        if(ImGui::Checkbox(labels[i], &checked))
            Emulator::write_register(gb, regis, bits[i], checked);
    }
}
