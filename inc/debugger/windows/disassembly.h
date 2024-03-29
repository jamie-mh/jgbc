#pragma once
#include "debugger/debugger.h"
#include "debugger/window.h"
#include <cstdint>
#include <map>
#include <optional>
#include <string>

namespace Windows {
class Disassembly final : public Window {
public:
    explicit Disassembly(Debugger &);

    void render() override;
    [[nodiscard]] constexpr const char *title() const override;

    void scroll_to_address(uint16_t);
    void add_label(uint16_t, const std::string &);

private:
    std::optional<uint16_t> _address_to_scroll_to;
    std::map<uint16_t, const std::string> _labels;

    static void draw_region_prefix(uint16_t addr);
    void draw_instr_line(uint16_t, const Emulator::Instruction &);
    void draw_data_line(uint16_t) const;

    constexpr static bool is_executable(uint16_t);
    constexpr static const char *get_region_label(uint16_t);
    [[nodiscard]] uint16_t address_of_nth_line(uint16_t, size_t) const;
    [[nodiscard]] size_t line_count(uint16_t, uint16_t) const;
};
}
