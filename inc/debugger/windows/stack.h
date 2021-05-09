#pragma once
#include "debugger/window.h"

namespace Windows {
    class Stack final : public Window {
        public:
            explicit Stack(Debugger &);

            void render() override;
            [[nodiscard]] const char *title() const override;
    };
}
