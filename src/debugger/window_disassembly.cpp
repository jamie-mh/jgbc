#include "debugger/debugger.h"
#include "debugger/colours.h"
#include "debugger/window_disassembly.h"
#include <imgui.h>

void WindowDisassembly::set_follow_pc(const bool value) {
    _follow_pc = value;
}

void WindowDisassembly::set_goto_addr(const uint16_t addr) {
    _goto_addr = addr;
    _goto_lock = true;
}

void WindowDisassembly::render() {

    if(!is_open || !ImGui::Begin("Disassembly", &is_open)) {
        if(is_open) ImGui::End();
        return;
    }

    static const auto gb = &_dbg.gb;
    const ImU32 step = 1, step_fast = 50;
    ImGui::InputScalar("ADDR", ImGuiDataType_U32, &_goto_addr, &step, &step_fast, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();

    if(ImGui::Button("GOTO"))
    {
        _follow_pc = false;
        _goto_lock = true;
    }

    ImGui::Checkbox("Follow PC", &_follow_pc);

    ImGui::BeginChild("##scroll");
            
    ImGuiListClipper clipper(get_line_count(0xFFFF));

    while(clipper.Step()) {

        if(_follow_pc)
            ImGui::SetScrollFromPosY(
                ImGui::GetCursorStartPos().y + ImGui::GetTextLineHeightWithSpacing() * get_line_count(REG(PC)));

        if(_goto_lock)
            ImGui::SetScrollFromPosY(
                ImGui::GetCursorStartPos().y + ImGui::GetTextLineHeightWithSpacing() * get_line_count(_goto_addr));

        auto addr = get_nth_line_addr(clipper.DisplayStart);

        for(auto i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {

            if(is_executable(addr))
                line_instr(addr);
            else
                line_data(addr);

            if(_goto_lock && addr == _goto_addr) {
                ImGui::SetScrollHereY();
                _goto_lock = false;
            }
        }
    }
    
    clipper.End();
    ImGui::EndChild();
    ImGui::End();
}

void WindowDisassembly::line_instr(uint16_t &addr) const {

    static const auto gb = &_dbg.gb;
    const auto draw_list = ImGui::GetWindowDrawList();

    bool is_breakpoint = _dbg.is_breakpoint(addr);
    ImGui::PushID(addr);

    if(ImGui::Selectable("", is_breakpoint)) {

        if(is_breakpoint)
            _dbg.remove_breakpoint(addr);
        else
            _dbg.add_breakpoint(addr);
    }

    ImGui::PopID();
    ImGui::SameLine();

    const auto opcode = SREAD8(addr);
    const auto instr = Emulator::find_instr(gb, addr);

    const auto line_coords = ImGui::GetCursorScreenPos();
    const auto line_height = ImGui::GetTextLineHeight();
    const auto line_center = line_coords.y + line_height / 2;

    if(addr == REG(PC)) {
        draw_list->AddCircle(ImVec2(line_coords.x, line_center), 6.0f, IM_COL32(0, 255, 0, 255));
        if(_follow_pc) ImGui::SetScrollHereY();
    }

    if(is_breakpoint)
        draw_list->AddCircleFilled(ImVec2(line_coords.x + 16.0f, line_center), 6.0f, IM_COL32(255, 0, 0, 255));

    prefix_region(addr);
    ImGui::TextColored(Colours::instruction, "%02X", opcode);
    ImGui::SameLine();

    if(instr.length > 1 && opcode != 0xCB) {

        uint16_t operand = 0;

        if(instr.length == 2) {
            operand = SREAD8(addr + 1);

            // If the operand is signed, get the two's complement
            if(instr.signed_operand) {
                operand = (~(operand - 1)) & 0x00FF;
            }

            ImGui::TextColored(Colours::data, "%02X", operand);

        } else if(instr.length == 3) {
            operand = SREAD16(addr + 1);
            ImGui::TextColored(Colours::data, "%02X %02X", operand & 0xFF, (operand & 0xFF00) >> 8);
        }

        ImGui::SameLine(300);
        ImGui::TextColored(Colours::disassembly, instr.disassembly, operand);
    }
    // If there is no operand (or CB opcode which have no operand)
    else {
        if(opcode == 0xCB) {
            ImGui::TextColored(Colours::instruction, "%02X ", SREAD8(addr + 1));
            ImGui::SameLine();
        }

        ImGui::SameLine(300);
        ImGui::TextColored(Colours::disassembly, instr.disassembly, addr);
    }

    ImGui::SameLine(440);
    ImGui::TextColored(Colours::comment, "; %d", instr.length);
    addr += instr.length;
}

void WindowDisassembly::line_data(uint16_t &addr) const {
    
    static const auto gb = &_dbg.gb;
    ImGui::Text("");
    prefix_region(addr);

    const auto data = SREAD8(addr);
    ImGui::TextColored(Colours::data, "%02X ", data);
    ImGui::SameLine();
    ImGui::TextColored(Colours::disassembly, "%c", (int8_t) data);

    ImGui::SameLine(440);

    switch(addr) {

        case 0x104:
            ImGui::TextColored(Colours::comment, "; Header Start");
            break;
        
        case CART_HEADER_TITLE:
            ImGui::TextColored(Colours::comment, "; Title: %s", gb->cart.title);
            break;

        case CART_HEADER_GBC_FLAG:
            ImGui::TextColored(Colours::comment, "; GBC Flag");
            break;

        case CART_HEADER_TYPE:
            ImGui::TextColored(Colours::comment, "; MBC Type");
            break;

        case CART_HEADER_ROM_SIZE:
            ImGui::TextColored(Colours::comment, "; ROM %d x %d KB", gb->cart.rom_size, ROM_BANK_SIZE);
            break;

        case CART_HEADER_RAM_SIZE:
            ImGui::TextColored(Colours::comment, "; RAM %d x %d KB", gb->cart.ram_size, EXTRAM_BANK_SIZE);
            break;

        case CART_HEADER_END:
            ImGui::TextColored(Colours::comment, "; Header End");
            break;

        default:
            ImGui::TextColored(Colours::comment, ";");
            break;
    }

    addr++;
}

inline void WindowDisassembly::prefix_region(const uint16_t addr) const {
    ImGui::SameLine(40);
    ImGui::TextColored(Colours::region, "%s: ", get_region_label(addr));

    ImGui::SameLine(120);
    ImGui::TextColored(Colours::address, "0x%04X", addr);
    ImGui::SameLine(200);
}

bool WindowDisassembly::is_executable(const uint16_t addr) const {
    
    // Executable instructions can be found here
    return 
        ((addr >= ROM00_START && addr <= 0x103) ||
        (addr > CART_HEADER_END && addr <= ROMNN_END) ||
        (addr >= EXTRAM_START && addr <= OAM_START) ||
        (addr >= HRAM_START && addr <= HRAM_END));
}

char *WindowDisassembly::get_region_label(const uint16_t addr) const {

    if(addr <= ROM00_END)
        return "ROM00";
    else if(addr >= ROMNN_START && addr <= ROMNN_END)
        return "ROMNN";
    else if(addr >= VRAM_START && addr <= VRAM_END)
        return "VRAM";
    else if(addr >= EXTRAM_START && addr <= EXTRAM_END)
        return "EXTRAM";
    else if(addr >= WRAM00_START && addr <= WRAM00_END)
        return "WRAM00";
    else if(addr >= WRAMNN_START && addr <= WRAMNN_END)
        return "WRAMNN";
    else if(addr >= WRAM00_MIRROR_START && addr <= WRAM00_MIRROR_END)
        return "ECHO00";
    else if(addr >= WRAMNN_MIRROR_START && addr <= WRAMNN_MIRROR_END)
        return "ECHONN";
    else if(addr >= OAM_START && addr <= OAM_END)
        return "OAM";
    else if(addr >= IO_START && addr <= IO_END)
        return "IO";
    else if(addr >= HRAM_START && addr <= HRAM_END)
        return "HRAM";
    else if(addr == IE_START_END)
        return "IE";

    return "";
}

uint16_t WindowDisassembly::get_nth_line_addr(const uint16_t n) const {

    for(uint32_t addr = 0, count = 1; addr <= 0xFFFF; count++) {

        if(is_executable(addr))
            addr += Emulator::find_instr(&_dbg.gb, addr).length;
        else
            addr++;

        if(count == n)
            return addr;
    }

    return 0x0;
}

uint16_t WindowDisassembly::get_line_count(const uint16_t limit_addr) const {

    uint16_t count = 0;

    for(uint32_t addr = 0; addr <= limit_addr;) {

        if(is_executable(addr))
            addr += Emulator::find_instr(&_dbg.gb, addr).length;
        else
            addr++;

        count++;
    }

    return count;
}
