#pragma once
#include "debugger/window.h"

namespace Windows {
class Palettes final : public Window {
public:
    explicit Palettes(Debugger &);

    void render() override;
    [[nodiscard]] constexpr const char *title() const override;
};
}
