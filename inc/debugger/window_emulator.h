#pragma once
#include <glad/glad.h>
#include "window.h"


class WindowEmulator final : public Window {
    private:
        Emulator::GameBoy &_gb;
        GLuint _textureId;

    public:
        WindowEmulator(Emulator::GameBoy &gb);
        ~WindowEmulator();

        void render() override;
};
