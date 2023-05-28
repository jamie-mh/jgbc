#pragma once
#include "debugger/window.h"

namespace Windows {
class Stack final : public Window {
public:
    explicit Stack(Debugger &);

    void render() override;
    [[nodiscard]] constexpr const char *title() const override;
};
}
