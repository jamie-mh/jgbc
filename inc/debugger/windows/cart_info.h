#pragma once
#include "debugger/window.h"

namespace Windows {
class CartInfo final : public Window {
public:
    explicit CartInfo(Debugger &);

    void render() override;
    [[nodiscard]] constexpr const char *title() const override;
};
}
