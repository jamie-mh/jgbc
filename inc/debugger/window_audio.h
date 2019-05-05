#pragma once

#include "debugger/window.h"
#include "debugger/emulator.h"

class WindowAudio final : public Window {
    private:
        Emulator::GameBoy &_gb;

    public:
        WindowAudio(Emulator::GameBoy &gb) : _gb(gb) { }
        void render() override;
};
