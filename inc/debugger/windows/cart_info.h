#pragma once
#include "debugger/window.h"

namespace Windows {
    class CartInfo final : public Window {
        public:
            explicit CartInfo(Debugger &);

            void render() override;
            [[nodiscard]] const char *title() const override;
    };
}
