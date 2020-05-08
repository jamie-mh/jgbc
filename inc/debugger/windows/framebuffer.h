#pragma once
#include <glad/glad.h>
#include "debugger/window.h"

namespace Windows {
    class Framebuffer final : public Window {
        public:
            explicit Framebuffer(Debugger &);
            ~Framebuffer();

            void render() override;
            const char *title() const override;

        private:
            GLuint _texture_id;
    };
}
