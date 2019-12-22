#pragma once
#include "debugger/window.h"
#include "debugger/emulator.h"

class WindowAudio final : public Window {
    private:
        Emulator::GameBoy &_gb;
        MemoryEditor _editor;

    public:
        explicit WindowAudio(Emulator::GameBoy &);
        void render() override;
};
