#pragma once
#include <glad/glad.h>
#include "window.h"

class WindowEmulator final : public Window {
    private:
        Emulator::GameBoy &_gb;
        GLuint _texture_id;

    public:
        explicit WindowEmulator(Emulator::GameBoy &gb);
        ~WindowEmulator() override;

        void render() override;
};
