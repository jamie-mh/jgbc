#pragma once
#include "debugger/window.h"

namespace Windows {
    class Breakpoints final : public Window {
        public:
            explicit Breakpoints(Debugger &);

            void render() override;

            [[nodiscard]] const char *title() const override;
    };
}
