#pragma once
#include "window.h"

constexpr unsigned int EMULATOR_SCALE = 2;

class WindowEmulator final : public Window {
    private:
        Emulator::GameBoy &_gb;

    public:
        WindowEmulator(Emulator::GameBoy &gb) : _gb(gb) { }
        void render() override;
};
