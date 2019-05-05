#pragma once
#include "window.h"

constexpr unsigned int MEMORY_REGION_COUNT = 9;

class WindowMemory final : public Window {
    private:
        Emulator::GameBoy &_gb;

    public:
        WindowMemory(Emulator::GameBoy &gb) : _gb(gb) { }
        void render() override;
};
