#pragma once
#include "debugger/window.h"


class WindowStack final : public Window {
    public:
        explicit WindowStack(Debugger &);

        void render() override;
        const char *title() const override;
};
