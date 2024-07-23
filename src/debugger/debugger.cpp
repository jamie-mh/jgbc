#include <algorithm>
#include <fstream>
#include <iostream>

#include "debugger/debugger.h"
#include "debugger/font.h"
#include "glad/glad.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_sdl2.h"

#include "debugger/windows/breakpoints.h"
#include "debugger/windows/cart_info.h"
#include "debugger/windows/controls.h"
#include "debugger/windows/disassembly.h"
#include "debugger/windows/framebuffer.h"
#include "debugger/windows/io.h"
#include "debugger/windows/memory.h"
#include "debugger/windows/palettes.h"
#include "debugger/windows/registers.h"
#include "debugger/windows/serial.h"
#include "debugger/windows/stack.h"

Debugger::Debugger(const char *rom_path) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    _is_paused = true;
    _window = nullptr;
    _gl_context = nullptr;

    _next_stop_fall_thru = std::nullopt;
    _next_stop_jump = std::nullopt;

    _gb = std::make_shared<Emulator::GameBoy>();
    Emulator::init(_gb.get());

    if (!Emulator::load_rom(_gb.get(), rom_path)) {
        std::cerr << "ERROR: Cannot load rom file" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (!Emulator::load_ram(_gb.get())) {
        std::cerr << "ERROR: Cannot load ram (save) file" << std::endl;
    }

    init_sdl();
    init_gl();
    init_imgui();

    std::ostringstream title;
    title << WINDOW_TITLE << " - " << _gb->cart.title << " (DEBUGGER)";
    SDL_SetWindowTitle(_window, title.str().c_str());

    _windows.emplace(WindowId::Breakpoints, std::make_shared<Windows::Breakpoints>(*this));
    _windows.emplace(WindowId::CartInfo, std::make_shared<Windows::CartInfo>(*this));
    _windows.emplace(WindowId::Controls, std::make_shared<Windows::Controls>(*this));
    _windows.emplace(WindowId::Disassembly, std::make_shared<Windows::Disassembly>(*this));
    _windows.emplace(WindowId::Framebuffer, std::make_shared<Windows::Framebuffer>(*this));
    _windows.emplace(WindowId::IO, std::make_shared<Windows::IO>(*this));
    _windows.emplace(WindowId::Memory, std::make_shared<Windows::Memory>(*this));
    _windows.emplace(WindowId::Palettes, std::make_shared<Windows::Palettes>(*this));
    _windows.emplace(WindowId::Registers, std::make_shared<Windows::Registers>(*this));
    _windows.emplace(WindowId::Stack, std::make_shared<Windows::Stack>(*this));

    auto serial_window = std::make_shared<Windows::Serial>(*this);
    _gb->mmu.serial_write_handler = serial_window->serial_write_handler;
    _windows.emplace(WindowId::Serial, serial_window);

    load_symbols(rom_path);
}

void Debugger::init_sdl() {
    _window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                               WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
}

void Debugger::init_gl() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    _gl_context = SDL_GL_CreateContext(_window);
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGL()) {
        std::cerr << "ERROR: Cannot load OpenGL extensions!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

std::shared_ptr<Emulator::GameBoy> &Debugger::gb() { return _gb; }

std::map<Debugger::WindowId, std::shared_ptr<Window>> &Debugger::windows() { return _windows; }

std::shared_ptr<Window> &Debugger::window(const Debugger::WindowId id) { return _windows.at(id); }

bool Debugger::is_paused() const { return _is_paused; }

void Debugger::set_paused(const bool value) {
    _is_paused = value;
    SDL_PauseAudioDevice(_gb->apu.device_id, value);
}

void Debugger::set_next_stop(const std::optional<uint16_t> fall_thru_addr, const std::optional<uint16_t> jump_addr) {
    _next_stop_fall_thru = fall_thru_addr;
    _next_stop_jump = jump_addr;
}

void Debugger::init_imgui() const {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL2_InitForOpenGL(_window, _gl_context);
    ImGui_ImplOpenGL3_Init();

    ImGui::StyleColorsDark();
    auto &style = ImGui::GetStyle();
    style.ScrollbarSize = 20.0f;
    style.GrabMinSize = 20.0f;
    style.ScrollbarRounding = 0.0f;
    style.WindowRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.WindowBorderSize = 0.0f;

    io.Fonts->AddFontFromMemoryCompressedTTF(LiberationMono_compressed_data, LiberationMono_compressed_size, 15.0f);
}

void Debugger::load_symbols(const char *rom_path) {

    auto symbols_path = std::string(rom_path);
    const auto extension_pos = symbols_path.find_last_of('.');

    if (extension_pos != std::string::npos) {
        symbols_path.erase(symbols_path.begin() + extension_pos, symbols_path.end());
    }

    symbols_path.append(".sym");

    std::ifstream file(symbols_path);

    if (!file.is_open())
        return;

    auto disassembly_window = std::dynamic_pointer_cast<Windows::Disassembly>(_windows.at(WindowId::Disassembly));

    std::string line;
    while (std::getline(file, line)) {

        if (line.empty() || line[0] == ';')
            continue;

        // TODO: deal with banks
        uint8_t bank;
        uint16_t addr;
        char label_buffer[101];

        sscanf(line.c_str(), "%hhd:%4hX %100s", &bank, &addr, label_buffer);

        std::string label(label_buffer);
        disassembly_window->add_label(addr, label);
    }
}

Debugger::~Debugger() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(_gl_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Debugger::run() {

    SDL_Event event;
    _gb->is_running = true;

    static auto window_disassembly =
        std::dynamic_pointer_cast<Windows::Disassembly>(_windows.at(WindowId::Disassembly));
    static auto *gb = _gb.get();

    while (_gb->is_running) {
        static const uint32_t max_ticks = CLOCK_SPEED / FRAMERATE;

        if (!SDL_GetQueuedAudioSize(_gb->apu.device_id)) {
            uint32_t frame_ticks = 0;

            while (!_is_paused && frame_ticks < max_ticks) {
                Emulator::update_cpu(_gb.get());

                if (_gb->cpu.is_double_speed) {
                    Emulator::check_interrupts(_gb.get());
                    Emulator::update_cpu(_gb.get());
                }

                Emulator::check_interrupts(_gb.get());
                Emulator::update_ppu(_gb.get());
                Emulator::update_apu(_gb.get());
                Emulator::update_dma(_gb.get());
                Emulator::update_hdma(_gb.get());

                frame_ticks += _gb->cpu.ticks;

                if (is_breakpoint(_gb->cpu.reg.PC)) {
                    window_disassembly->scroll_to_address(_gb->cpu.reg.PC);
                    _is_paused = true;
                }

                if (_next_stop_fall_thru == REG(PC) || _next_stop_jump == REG(PC)) {
                    _next_stop_fall_thru = std::nullopt;
                    _next_stop_jump = std::nullopt;

                    window_disassembly->scroll_to_address(_gb->cpu.reg.PC);
                    _is_paused = true;
                }
            }
        }

        render();

        while (SDL_PollEvent(&event)) {
            handle_event(event);
        }
    }

    Emulator::save_ram(_gb.get());
}

void Debugger::handle_event(SDL_Event event) const {
    ImGui_ImplSDL2_ProcessEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        _gb->is_running = false;
        break;

    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_CLOSE)
            _gb->is_running = false;
        break;

    case SDL_KEYDOWN:
        Emulator::set_key(_gb.get(), event.key.keysym.scancode, true);
        break;

    case SDL_KEYUP:
        Emulator::set_key(_gb.get(), event.key.keysym.scancode, false);
        break;
    }
}

void Debugger::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(_window);
    ImGui::NewFrame();

    const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking |
                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground |
                       ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
                       ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar |
                       ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##workspace", nullptr, flags);
    ImGui::PopStyleVar();

    ImGui::DockSpace(ImGui::GetID("##dockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    _menu.render();

    for (const auto &[_, window] : _windows) {
        if (window->is_open()) {
            window->render();
        }
    }

    // ImGui::ShowDemoWindow();
    ImGui::End();

    ImGui::Render();
    SDL_GL_MakeCurrent(_window, _gl_context);

    glClearColor(0x0, 0x0, 0x0, 0x0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, static_cast<int>(ImGui::GetIO().DisplaySize.x), static_cast<int>(ImGui::GetIO().DisplaySize.y));

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(_window);
}

bool Debugger::is_breakpoint(const uint16_t addr) const {
    return std::find(_breakpoints.begin(), _breakpoints.end(), addr) != _breakpoints.end();
}

void Debugger::add_breakpoint(const uint16_t addr) {
    if (is_breakpoint(addr)) {
        return;
    }

    _breakpoints.push_back(addr);
}

void Debugger::remove_breakpoint(const uint16_t addr) {
    for (size_t i = 0; i < _breakpoints.size(); ++i) {
        if (_breakpoints[i] == addr) {
            _breakpoints.erase(_breakpoints.begin() + i);
            return;
        }
    }
}

const std::vector<uint16_t> &Debugger::breakpoints() const { return _breakpoints; }
