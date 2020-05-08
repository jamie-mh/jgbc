#pragma once
#include "window.h"

class WindowCartInfo final : public Window {
    public:
        explicit WindowCartInfo(Debugger &);

        void render() override;
        const char *title() const override;
};
