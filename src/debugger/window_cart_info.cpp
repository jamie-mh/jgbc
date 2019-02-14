#include "debugger/debugger.h"
#include "debugger/window_cart_info.h"
#include <imgui.h>

void WindowCartInfo::render() {

	if(!is_open || !ImGui::Begin("Cart Info", &is_open)) {
		if(is_open) ImGui::End();
		return;
	}

	const auto &style = ImGui::GetStyle();
	const auto name_colour = style.Colors[ImGuiCol_ButtonActive];

	ImGui::TextColored(name_colour, "Title ");
	ImGui::SameLine();
	ImGui::Text("%s", _gb.cart.title);

	ImGui::TextColored(name_colour, "GBC Flag");
	ImGui::SameLine();
	ImGui::Text("%02X", _gb.cart.gbc_flag);

	ImGui::TextColored(name_colour, "Cartridge Type");
	ImGui::SameLine();
	ImGui::Text("%02X", _gb.cart.type);

	ImGui::TextColored(name_colour, "ROM Size");
	ImGui::SameLine();
	ImGui::Text("%d x %d KB", _gb.cart.rom_size, ROM_BANK_SIZE);

	ImGui::TextColored(name_colour, "RAM Size");
	ImGui::SameLine();
	ImGui::Text("%d x %d KB", _gb.cart.ram_size, EXTRAM_BANK_SIZE);

	ImGui::End();
}
