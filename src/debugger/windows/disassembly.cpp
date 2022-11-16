#include "debugger/windows/disassembly.h"
#include "debugger/colours.h"
#include "debugger/debugger.h"
#include <imgui.h>

using namespace Windows;

// TODO: improve performance

Disassembly::Disassembly(Debugger &debugger) : Window(debugger) {
    _address_to_scroll_to = PROGRAM_START;

    _labels.emplace(PROGRAM_START, "Program Start");
    _labels.emplace(CART_HEADER_START + 4, "Cartridge Header");
    _labels.emplace(CART_HEADER_TITLE, "Title");
    _labels.emplace(CART_HEADER_GBC_FLAG, "GBC Flag");
    _labels.emplace(CART_HEADER_TYPE, "Type");
    _labels.emplace(CART_HEADER_ROM_SIZE, "ROM Size");
    _labels.emplace(CART_HEADER_RAM_SIZE, "RAM Size");
}

void Disassembly::render() {

    if (!ImGui::Begin(title())) {
        ImGui::End();
        return;
    }

    INIT_GB_CTX();

    if (ImGui::Button("Goto PC"))
        _address_to_scroll_to = REG(PC);

    ImGui::SameLine();

    static uint16_t selected_label_addr = _labels.begin()->first;
    if (ImGui::BeginCombo("Label", _labels.at(selected_label_addr).c_str())) {

        for (const auto &[addr, name] : _labels) {
            ImGui::PushID(reinterpret_cast<void *>(addr));

            if (ImGui::Selectable(name.c_str(), selected_label_addr == addr))
                selected_label_addr = addr;

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    ImGui::SameLine();

    if (ImGui::Button("Goto"))
        _address_to_scroll_to = selected_label_addr;

    ImGui::BeginChild("##scroll");
    ImGuiListClipper clipper(line_count(0, UINT16_MAX));

    // TODO: fix bug with display when label is placed at 0x0000

    while (clipper.Step()) {

        if (_address_to_scroll_to.has_value()) {
            const auto offset = ImGui::GetTextLineHeightWithSpacing() *
                                static_cast<float>(line_count(0, _address_to_scroll_to.value()) + _labels.size());
            ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + offset);
        }

        auto addr = address_of_nth_line(0, clipper.DisplayStart);

        for (auto i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {

            for (const auto &[a, label] : _labels) {
                if (addr == a) {
                    ImGui::Text("%s:", label.c_str());
                    break;
                }
            }

            if (is_executable(addr)) {
                const auto instr = Emulator::find_instr(gb, addr);
                draw_instr_line(addr, instr);
                addr += instr.length;
            } else {
                draw_data_line(addr);
                addr++;
            }
        }
    }

    clipper.End();
    ImGui::EndChild();
    ImGui::End();

    _address_to_scroll_to = std::nullopt;
}

const char *Disassembly::title() const { return "Disassembly"; }

void Disassembly::draw_instr_line(uint16_t addr, const Emulator::Instruction &instr) {

    INIT_GB_CTX();

    const auto line_coords = ImGui::GetCursorScreenPos();
    const auto line_height = ImGui::GetTextLineHeight();
    const auto line_center = line_coords.y + line_height / 2;

    if (addr == REG(PC)) {
        ImGui::Selectable("", true, ImGuiSelectableFlags_NoHoldingActiveID);
        ImGui::SameLine();
    } else
        ImGui::NewLine();

    if (debugger().is_breakpoint(addr)) {
        auto *const draw_list = ImGui::GetWindowDrawList();
        draw_list->AddCircleFilled(ImVec2(line_coords.x + 16.0f, line_center), 6.0f, IM_COL32(255, 0, 0, 255));
    }

    draw_region_prefix(addr);
    const auto opcode = SREAD8(addr);
    ImGui::TextColored(Colours::instruction, "%02X", opcode);
    ImGui::SameLine();

    if (instr.length > 1 && opcode != 0xCB) {

        uint16_t operand = 0;
        uint16_t label_addr = 0;

        if (instr.length == 2) {
            operand = SREAD8(addr + 1);
            ImGui::TextColored(Colours::data, "%02X", operand);

            if (instr.signed_operand) {
                const auto jump_addr = addr + static_cast<int8_t>(operand);
                label_addr = jump_addr + Emulator::find_instr(gb, jump_addr).length;
            }

        } else if (instr.length == 3) {
            operand = SREAD16(addr + 1);
            ImGui::TextColored(Colours::data, "%02X %02X", operand & 0xFF, (operand & 0xFF00) >> 8);
            label_addr = operand;
        }

        ImGui::SameLine(300);
        ImGui::TextColored(Colours::disassembly, instr.disassembly, operand);

        if (label_addr > 0 && _labels.find(label_addr) != _labels.end()) {
            ImGui::SameLine();
            ImGui::Text("[%s]", _labels.at(label_addr).c_str());
        }
    }
    // If there is no operand (or CB opcode which have no operand)
    else {
        if (opcode == 0xCB) {
            ImGui::TextColored(Colours::instruction, "%02X ", SREAD8(addr + 1));
            ImGui::SameLine();
        }

        ImGui::SameLine(300);
        ImGui::TextColored(Colours::disassembly, instr.disassembly, addr);
    }
}

void Disassembly::draw_data_line(uint16_t addr) const {

    INIT_GB_CTX();
    ImGui::NewLine();
    draw_region_prefix(addr);

    const auto data = SREAD8(addr);
    ImGui::TextColored(Colours::data, "%02X ", data);
    ImGui::SameLine();
    ImGui::TextColored(Colours::disassembly, "%c", data);
}

void Disassembly::draw_region_prefix(const uint16_t addr) {
    ImGui::SameLine(40);
    const auto *const prefix = get_region_label(addr);

    if (prefix != nullptr)
        ImGui::TextColored(Colours::region, "%s: ", prefix);

    ImGui::SameLine(120);
    ImGui::TextColored(Colours::address, "0x%04X", addr);
    ImGui::SameLine(200);
}

bool Disassembly::is_executable(const uint16_t addr) {

    return (addr <= 0x103 || (addr > CART_HEADER_END && addr <= ROMNN_END) ||
            (addr >= EXTRAM_START && addr <= OAM_START) || (addr >= HRAM_START && addr <= HRAM_END));
}

const char *Disassembly::get_region_label(const uint16_t addr) {

    if (addr <= ROM00_END)
        return "ROM00";
    if (addr >= ROMNN_START && addr <= ROMNN_END)
        return "ROMNN";
    if (addr >= VRAM_START && addr <= VRAM_END)
        return "VRAM";
    if (addr >= EXTRAM_START && addr <= EXTRAM_END)
        return "EXTRAM";
    if (addr >= WRAM00_START && addr <= WRAM00_END)
        return "WRAM00";
    if (addr >= WRAMNN_START && addr <= WRAMNN_END)
        return "WRAMNN";
    if (addr >= WRAM00_MIRROR_START && addr <= WRAM00_MIRROR_END)
        return "ECHO00";
    if (addr >= WRAMNN_MIRROR_START && addr <= WRAMNN_MIRROR_END)
        return "ECHONN";
    if (addr >= OAM_START && addr <= OAM_END)
        return "OAM";
    if (addr >= IO_START && addr <= IO_END)
        return "IO";
    if (addr >= HRAM_START && addr <= HRAM_END)
        return "HRAM";
    if (addr == IE_START_END)
        return "IE";

    return nullptr;
}

uint16_t Disassembly::address_of_nth_line(const uint16_t start_addr, const size_t n) const {
    size_t count = 1;

    for (uint32_t addr = start_addr; addr <= 0xFFFF; count++) {

        if (is_executable(addr))
            addr += Emulator::find_instr(debugger().gb().get(), addr).length;
        else
            addr++;

        if (count == n)
            return addr;
    }

    return 0;
}

size_t Disassembly::line_count(const uint16_t from_addr, const uint16_t to_addr) const {
    size_t count = 0;

    for (uint32_t addr = from_addr; addr <= to_addr;) {

        if (is_executable(addr))
            addr += Emulator::find_instr(debugger().gb().get(), addr).length;
        else
            addr++;

        count++;
    }

    return count;
}

void Disassembly::scroll_to_address(const uint16_t address) { _address_to_scroll_to = address; }

void Disassembly::add_label(const uint16_t address, const std::string &label) { _labels.emplace(address, label); }
