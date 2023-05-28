#pragma once
#include "debugger/window.h"
#include <glad/glad.h>

namespace Windows {
class Framebuffer final : public Window {
public:
    explicit Framebuffer(Debugger &);
    ~Framebuffer() override;

    void render() override;
    [[nodiscard]] constexpr const char *title() const override;

private:
    GLuint _texture_id;
};
}
