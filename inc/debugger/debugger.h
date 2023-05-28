#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#define IMGUI_USER_CONFIG "debugger/imconfig.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "debugger/emulator.h"
#include "debugger/menubar.h"
#include "debugger/window.h"

class Debugger final {
public:
    enum class WindowId {
        Breakpoints,
        CartInfo,
        Controls,
        Disassembly,
        Framebuffer,
        IO,
        Memory,
        Palettes,
        Registers,
        Serial,
        Stack
    };

    explicit Debugger(const char *);
    ~Debugger();

    std::shared_ptr<Emulator::GameBoy> &gb();

    [[nodiscard]] const std::vector<uint16_t> &breakpoints() const;
    [[nodiscard]] bool is_breakpoint(uint16_t) const;
    void add_breakpoint(uint16_t);
    void remove_breakpoint(uint16_t);

    [[nodiscard]] bool is_paused() const;
    void set_paused(bool);

    void set_next_stop(std::optional<uint16_t>, std::optional<uint16_t>);

    void run();
    void render();

    std::shared_ptr<Window> &window(WindowId);
    std::map<Debugger::WindowId, std::shared_ptr<Window>> &windows();

private:
    static constexpr int WINDOW_WIDTH = 1500;
    static constexpr int WINDOW_HEIGHT = 900;

    std::shared_ptr<Emulator::GameBoy> _gb;

    SDL_Window *_window;
    SDL_GLContext _gl_context;

    bool _is_paused;

    // A conditional jump has two possible branches
    std::optional<uint16_t> _next_stop_fall_thru;
    std::optional<uint16_t> _next_stop_jump;

    std::vector<uint16_t> _breakpoints;

    MenuBar _menu = MenuBar(*this);
    std::map<WindowId, std::shared_ptr<Window>> _windows;

    void init_sdl();
    void init_gl();
    void init_imgui() const;
    void load_symbols(const char *);

    void handle_event(SDL_Event) const;
};
