#pragma once
#include "window.h"

class Debugger;
class WindowCartInfo final : public Window {
    private:
        Emulator::GameBoy &_gb;

    public:
        WindowCartInfo(Emulator::GameBoy &gb) : _gb(gb) { }
        void render() override;
};
