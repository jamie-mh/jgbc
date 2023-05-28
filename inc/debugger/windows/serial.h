#pragma once
#include "debugger/window.h"
#include <cstdint>
#include <sstream>

namespace Windows {
class Serial final : public Window {
public:
    explicit Serial(Debugger &);

    void render() override;
    [[nodiscard]] constexpr const char *title() const override;

    static void serial_write_handler(uint8_t);

private:
    static std::stringstream _buffer;
};
}
