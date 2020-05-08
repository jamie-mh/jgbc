#pragma once
#include <glad/glad.h>
#include "window.h"


class WindowEmulator final : public Window {
    public:
        explicit WindowEmulator(Debugger &);
        ~WindowEmulator();

        void render() override;
        const char *title() const override;

    private:
        GLuint _texture_id;
};
