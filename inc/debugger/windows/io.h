#pragma once
#include "debugger/window.h"
#include <cstdint>

namespace Windows {
class IO final : public Window {
public:
    explicit IO(Debugger &);

    void render() override;
    [[nodiscard]] constexpr const char *title() const override;

private:
    void draw_values(const char * const *, const uint16_t *, int) const;
    void draw_registers(const char * const *, uint16_t, const uint8_t *, int) const;
};
}
