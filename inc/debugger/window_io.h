#pragma once

#include "debugger/window.h"
#include "debugger/emulator.h"

class WindowIO final : public Window {
    private:
        Emulator::GameBoy &_gb;
        void draw_values(const char **labels, const uint16_t *addrs, const int count) const;
        void draw_registers(const char **labels, const uint16_t regis, const uint8_t *bits, const int count) const;

    public:
        WindowIO(Emulator::GameBoy &gb) : _gb(gb) { }
        void render() override;
};
