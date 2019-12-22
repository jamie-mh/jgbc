#pragma once
#include "window.h"


class WindowPalettes final : public Window {
    private:
        Emulator::GameBoy &_gb;

    public:
        explicit WindowPalettes(Emulator::GameBoy &gb): _gb(gb) { };
        void render() override;
};