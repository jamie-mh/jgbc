#pragma once

#include "debugger/window.h"
#include "debugger/emulator.h"

class WindowStack final : public Window {
    private:
		Emulator::GameBoy &_gb;

	public:
		WindowStack(Emulator::GameBoy &gb) : _gb(gb) { }
		void render() override;
};
