#include "debugger/debugger.h"
#include "debugger/window_emulator.h"
#include <imgui.h>
#include <glad/glad.h>

void WindowEmulator::render() {

	if(!is_open || !ImGui::Begin("Emulator", &is_open)) {
		if(is_open) ImGui::End();
		return;
	}

	const auto window_size = ImGui::GetContentRegionAvail();
	const auto scale = std::min(window_size.x / SCREEN_WIDTH, window_size.y / SCREEN_HEIGHT);

	GLuint texture;
	glad_glGenTextures(1, &texture);
	glad_glBindTexture(GL_TEXTURE_2D, texture);

	glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, _gb.ppu.framebuffer);
	glad_glGenerateMipmap(GL_TEXTURE_2D);

	ImGui::Image((ImTextureID)texture, ImVec2(SCREEN_WIDTH * scale, SCREEN_HEIGHT * scale));
    ImGui::End();
}
