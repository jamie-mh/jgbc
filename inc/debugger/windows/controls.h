#pragma once
#include "debugger/window.h"
#include <cstdint>

namespace Windows {
class Controls final : public Window {
public:
    explicit Controls(Debugger &);

    void render() override;
    [[nodiscard]] constexpr const char *title() const override;

private:
    void step_into();
    void step_over();
    void run_to_next() const;

    constexpr static bool is_subroutine_call(uint8_t opcode);
    constexpr static bool is_jump_call(uint8_t opcode);
    constexpr static bool is_jump_signed(uint8_t opcode);
    constexpr static bool is_return(uint8_t opcode);
};
}
