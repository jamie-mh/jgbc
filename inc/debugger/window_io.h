#pragma once

#include "debugger/window.h"
#include "debugger/emulator.h"

class WindowIO final : public Window {
    private:
        Emulator::GameBoy &_gb;
        void draw_values(const char **, const uint16_t *, int) const;
        void draw_registers(const char **, uint16_t, const uint8_t *, int) const;

    public:
        explicit WindowIO(Emulator::GameBoy &gb) : _gb(gb) { }
        void render() override;
};
