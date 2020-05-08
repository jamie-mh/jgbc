#pragma once
#include "debugger/window.h"


class WindowRegisters final : public Window {
    public:
        explicit WindowRegisters(Debugger &);

        void render() override;
        const char *title() const override;
};
