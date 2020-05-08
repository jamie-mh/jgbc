#pragma once
#include "window.h"

class WindowBreakpoints final : public Window {
    public:
        explicit WindowBreakpoints(Debugger &);

        void render() override;
        const char *title() const override;
};
