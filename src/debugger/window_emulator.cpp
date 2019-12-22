#include "debugger/debugger.h"
#include "debugger/window_emulator.h"
#include <imgui.h>
#include <glad/glad.h>

WindowEmulator::WindowEmulator(Emulator::GameBoy &gb) : _gb(gb) {

    _texture_id = 0;
    glad_glGenTextures(1, &_texture_id);
    glad_glBindTexture(GL_TEXTURE_2D, _texture_id);

    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, _gb.ppu.framebuffer);
}

WindowEmulator::~WindowEmulator() {
    glDeleteTextures(1, &_texture_id);
}

void WindowEmulator::render() {

    if(!_is_open || !ImGui::Begin("Emulator", &_is_open)) {
        if(_is_open) ImGui::End();
        return;
    }

    const auto window_size = ImGui::GetContentRegionAvail();
    const auto scale = std::min(window_size.x / SCREEN_WIDTH, window_size.y / SCREEN_HEIGHT);

    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, _gb.ppu.framebuffer);

    ImGui::Image((ImTextureID) (uintptr_t) _texture_id, ImVec2(SCREEN_WIDTH * scale, SCREEN_HEIGHT * scale));
    ImGui::End();
}
