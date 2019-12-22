#include "debugger/debugger.h"
#include "debugger/colours.h"
#include "debugger/window_registers.h"
#include <imgui.h>

void WindowRegisters::render() {

    if(!_is_open || !ImGui::Begin("Registers", &_is_open)) {
        if(_is_open) ImGui::End();
        return;
    }

    static const auto gb = &_gb;

    ImGui::Columns(2, nullptr, true);

    ImGui::Text("CPU");
    ImGui::NextColumn();
    ImGui::Text("IO");
    ImGui::NextColumn();
    ImGui::Separator();

    const char *cpu_reg_labels[6] = { "AF", "BC", "DE", "HL", "SP", "PC" };
    uint16_t *cpu_reg_addr[6] = { &REG(AF), &REG(BC), &REG(DE), &REG(HL), &REG(SP), &REG(PC) };

    const ImU32 step = 1, step_fast = 10;
    for(auto i = 0; i < 6; ++i) {
        int value = *cpu_reg_addr[i];
        if(ImGui::InputScalar(cpu_reg_labels[i], ImGuiDataType_U32, &value, &step, &step_fast, "%04X", ImGuiInputTextFlags_CharsHexadecimal))
            *cpu_reg_addr[i] = value;
    }

    ImGui::NextColumn();
    const char *io_reg_labels[5] = { "LCDC", "STAT", "LY", "IE", "IF" };
    const uint16_t io_reg_addr[5] = { LCDC, STAT, LY, IE, IF };

    for(auto i = 0; i < 5; ++i) {
        int value = io_reg_addr[i];
        if(ImGui::InputScalar(io_reg_labels[i], ImGuiDataType_U32, &value, &step, &step_fast, "%02X", ImGuiInputTextFlags_CharsHexadecimal))
            SWRITE8(io_reg_addr[i], value);
    }

    bool ime = REG(IME);
    if(ImGui::Checkbox("IME", &ime))
        REG(IME) = ime;

    ImGui::Columns(1);
    ImGui::Separator();

    bool flag_zero = FGET(FLAG_ZERO);
    bool flag_subtract = FGET(FLAG_SUBTRACT);
    bool flag_halfcarry = FGET(FLAG_HALFCARRY);
    bool flag_carry = FGET(FLAG_CARRY);

    ImGui::Text("FLAGS");
    if(ImGui::Checkbox("Z", &flag_zero))
        FSET(FLAG_ZERO, flag_zero);

    ImGui::SameLine();
    if(ImGui::Checkbox("N", &flag_subtract))
        FSET(FLAG_SUBTRACT, flag_subtract);

    ImGui::SameLine();
    if(ImGui::Checkbox("H", &flag_halfcarry))
        FSET(FLAG_HALFCARRY, flag_halfcarry);

    ImGui::SameLine();
    if(ImGui::Checkbox("C", &flag_carry))
        FSET(FLAG_CARRY, flag_carry);

    ImGui::Separator();
    ImGui::Text("STATE");
    ImGui::Checkbox("Halted", &gb->cpu.is_halted);
    ImGui::Checkbox("Double Speed", &gb->cpu.is_double_speed);

    ImGui::End();
}
