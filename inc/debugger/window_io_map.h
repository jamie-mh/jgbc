#pragma once

#include "debugger/window.h"
#include "debugger/emulator.h"

class WindowIOMap final : public Window {
    private:
		Emulator::GameBoy &_gb;

	public:
		WindowIOMap(Emulator::GameBoy &gb) : _gb(gb) { }
		void render() override;
};
