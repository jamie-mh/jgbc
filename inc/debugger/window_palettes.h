#pragma once
#include "window.h"


class WindowPalettes final : public Window {
    public:
        explicit WindowPalettes(Debugger &);

        void render() override;
        const char *title() const override;
};