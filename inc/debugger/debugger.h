#pragma once

#include <cstdio>
#include <SDL.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <memory>
#include <optional>

#define IMGUI_USER_CONFIG "debugger/imconfig.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "debugger/emulator.h"
#include "debugger/window.h"
#include "debugger/menubar.h"


class Debugger final {
    public:
        enum class WindowId {
            Breakpoints, CartInfo, Controls, Disassembly, Emulator, IO, Memory, Palettes, Registers, Stack
        };

        explicit Debugger(const char *);
        ~Debugger();

        std::shared_ptr<Emulator::GameBoy> &gb();

        const std::vector<uint16_t>& breakpoints() const;
        bool is_breakpoint(uint16_t) const;
        void add_breakpoint(uint16_t);
        void remove_breakpoint(uint16_t);

        bool is_paused() const;
        void set_paused(bool);

        std::optional<uint16_t> next_stop() const;
        void set_next_stop(const std::optional<uint16_t>);

        void run();
        void render();

        std::shared_ptr<Window> &window(WindowId);
        std::map<Debugger::WindowId, std::shared_ptr<Window>> & windows();

    private:
        static constexpr int WINDOW_WIDTH = 1500;
        static constexpr int WINDOW_HEIGHT = 900;

        std::shared_ptr<Emulator::GameBoy> _gb;

        SDL_Window *_window;
        SDL_GLContext _gl_context;

        bool _is_paused;
        std::optional<uint16_t> _next_stop;
        std::vector<uint16_t> _breakpoints;

        MenuBar _menu = MenuBar(*this);
        std::map<WindowId, std::shared_ptr<Window>> _windows;

        void init_sdl();
        void init_gl();
        void init_imgui() const;

        void handle_event(SDL_Event);
};
