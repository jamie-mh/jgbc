#pragma once

#include "debugger/window.h"
#include "debugger/emulator.h"

class WindowRegisters final : public Window {
    private:
		Emulator::GameBoy &_gb;

	public:
		WindowRegisters(Emulator::GameBoy &gb) : _gb(gb) { }
		void render() override;
};
