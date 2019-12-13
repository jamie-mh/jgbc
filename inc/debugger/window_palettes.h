#pragma once
#include "window.h"


class WindowPalettes final : public Window {
    private:
        Emulator::GameBoy &_gb;

    public:
        WindowPalettes(Emulator::GameBoy &gb);
        void render() override;
};