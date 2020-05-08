#pragma once
#include "debugger/window.h"


class MenuBar final : Window {
    public:
        explicit MenuBar(Debugger &debugger);

        void render() override;
        const char *title() const override;
};
