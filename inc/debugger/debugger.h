#pragma once

#include <cstdio>
#include <SDL.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

#define IMGUI_USER_CONFIG "debugger/imconfig.h"
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
#include "debugger/window_io.h"
#include "debugger/window_stack.h"
#include "debugger/window_audio.h"
#include "debugger/window_palettes.h"

class Debugger final {
    public:
        explicit Debugger(const char *rom_path);
        ~Debugger();

        Emulator::GameBoy &gb();
        Emulator::GameBoy *gb_p();

        const std::vector<uint16_t>& breakpoints() const;
        bool is_breakpoint(uint16_t) const;
        void add_breakpoint(uint16_t);
        void remove_breakpoint(uint16_t);

        bool is_paused() const;
        void set_paused(bool);

        uint16_t next_stop() const;
        void set_next_stop(uint16_t);

        void run();
        void render();

        WindowBreakpoints &window_breakpoints() { return *dynamic_cast<WindowBreakpoints *>(_windows[0]); }
        WindowCartInfo &window_cart_info() { return *dynamic_cast<WindowCartInfo *>(_windows[1]); }
        WindowIO &window_io() { return *dynamic_cast<WindowIO *>(_windows[2]); }
        WindowAudio &window_audio() { return *dynamic_cast<WindowAudio *>(_windows[3]); }
        WindowControls &window_controls() { return *dynamic_cast<WindowControls *>(_windows[4]); }
        WindowPalettes &window_palettes() { return *dynamic_cast<WindowPalettes *>(_windows[5]); }
        WindowDisassembly &window_disassembly() { return *dynamic_cast<WindowDisassembly *>(_windows[6]); }
        WindowEmulator &window_emulator() { return *dynamic_cast<WindowEmulator *>(_windows[7]); }
        WindowMemory &window_memory() { return *dynamic_cast<WindowMemory *>(_windows[8]); }
        WindowRegisters &window_registers() { return *dynamic_cast<WindowRegisters *>(_windows[9]); }
        WindowStack &window_stack() { return *dynamic_cast<WindowStack *>(_windows[10]); }

    private:
        const int WINDOW_WIDTH = 1500;
        const int WINDOW_HEIGHT = 900;

        Emulator::GameBoy _gb;

        SDL_Window *_window;
        SDL_GLContext _gl_context;

        bool _is_paused;
        uint16_t _next_stop;
        std::vector<uint16_t> _breakpoints;

        MenuBar _menu = MenuBar(*this);
        std::vector<Window *> _windows;

        void init_gl();
        void init_imgui() const;

        void handle_event(SDL_Event);
};
