#pragma once
#include "debugger/window.h"


class WindowIO final : public Window {
    public:
        explicit WindowIO(Debugger &);

        void render() override;
        const char *title() const override;

    private:
        void draw_values(const char **, const uint16_t *, int);
        void draw_registers(const char **, uint16_t, const uint8_t *, int);
};
