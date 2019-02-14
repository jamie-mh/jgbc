#pragma once

#include <cstdio>
#include <SDL.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

#include "imgui/imgui.h"

#include "debugger/emulator.h"
#include "debugger/window.h"
#include "debugger/menubar.h"

#include "debugger/window_breakpoints.h"
#include "debugger/window_cart_info.h"
#include "debugger/window_controls.h"
#include "debugger/window_disassembly.h"
#include "debugger/window_emulator.h"
#include "debugger/window_memory.h"
#include "debugger/window_registers.h"
#include "debugger/window_io_map.h"
#include "debugger/window_stack.h"

constexpr unsigned int WINDOW_WIDTH = 1600;
constexpr unsigned int WINDOW_HEIGHT = 900;

#define BG_COLOUR 0.45f, 0.55f, 0.60f, 1.00f
const ImVec4 ADDR_COLOUR { 0.38f, 0.49f, 0.55f, 1.0f };
const ImVec4 DATA_COLOUR { 1.0f, 0.96f, 0.62f, 1.0f };
const ImVec4 INSTR_COLOUR { 0.93f, 0.93f, 0.93f, 1.0f };
const ImVec4 CURR_COLOUR { 0.46f, 1.0f, 0.01f, 1.0f };


class Debugger final {
    public:
		static constexpr unsigned int WINDOW_COUNT = 8;

        bool is_paused = true;
		uint16_t next_stop;

        Emulator::GameBoy gb;

        Debugger(const char *rom_path);
        ~Debugger();

		
		void read_mem(uint16_t addr);
		
		size_t breakpoint_count() const;
		bool is_breakpoint(uint16_t addr) const;
		void add_breakpoint(uint16_t addr);
		void remove_breakpoint(uint16_t addr);
		const std::vector<uint16_t>& get_breakpoints() const;

        void run();
        void render();

		// Windows
		inline WindowBreakpoints &get_window_breakpoints() { return *static_cast<WindowBreakpoints *>(_windows[0]); }
		inline WindowCartInfo &get_window_cart_info() { return *static_cast<WindowCartInfo *>(_windows[1]); }
		inline WindowRegisters &get_window_io_map() { return *static_cast<WindowRegisters *>(_windows[2]); }
		inline WindowControls &get_window_controls() { return *static_cast<WindowControls *>(_windows[3]); }
		inline WindowDisassembly &get_window_disassembly() { return *static_cast<WindowDisassembly *>(_windows[4]); }
		inline WindowEmulator &get_window_emulator() { return *static_cast<WindowEmulator *>(_windows[5]); }
		inline WindowMemory &get_window_memory() { return *static_cast<WindowMemory *>(_windows[6]); }
		inline WindowIOMap &get_window_registers() { return *static_cast<WindowIOMap *>(_windows[7]); }
		inline WindowStack &get_window_stack() { return *static_cast<WindowStack *>(_windows[8]); }

    private:
        SDL_Window *_window;
        SDL_GLContext _gl_context;

		std::vector<uint16_t> _breakpoints;

		MenuBar _menu = MenuBar(*this);
		std::vector<Window *> _windows;

        void init_gl();
        void init_imgui() const;

        void handle_event(SDL_Event event);
};