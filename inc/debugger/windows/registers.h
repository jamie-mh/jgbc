#pragma once
#include "debugger/window.h"

namespace Windows {
    class Registers final : public Window {
        public:
            explicit Registers(Debugger &);

            void render() override;
            const char *title() const override;
    };
}
