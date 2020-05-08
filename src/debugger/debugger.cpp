#include <sstream>
#include <iostream>
#include <algorithm>

#include "debugger/debugger.h"
#include "debugger/font.h"
#include "imgui/examples/imgui_impl_sdl.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include "glad/glad.h"

#include "debugger/windows/breakpoints.h"
#include "debugger/windows/cart_info.h"
#include "debugger/windows/controls.h"
#include "debugger/windows/disassembly.h"
#include "debugger/windows/framebuffer.h"
#include "debugger/windows/io.h"
#include "debugger/windows/memory.h"
#include "debugger/windows/palettes.h"
#include "debugger/windows/registers.h"
#include "debugger/windows/stack.h"


Debugger::Debugger(const char *rom_path) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    _is_paused = true;
    _next_stop = 0;
    _window = nullptr;
    _gl_context = nullptr;

    _gb = std::make_shared<Emulator::GameBoy>();
    Emulator::init(_gb.get());

    if(!Emulator::load_rom(_gb.get(), rom_path)) {
        std::cerr << "ERROR: Cannot load rom file" << std::endl;
        std::exit(EXIT_FAILURE);
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
}

void Debugger::init_sdl() {
    _window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
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

    if(!gladLoadGL()) {
        std::cerr << "ERROR: Cannot load OpenGL extensions!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

std::shared_ptr<Emulator::GameBoy> &Debugger::gb() {
    return _gb;
}

std::map<Debugger::WindowId, std::shared_ptr<Window>> &Debugger::windows() {
    return _windows;
}

std::shared_ptr<Window> &Debugger::window(const Debugger::WindowId id) {
    return _windows.at(id);
}

bool Debugger::is_paused() const {
    return _is_paused;
}

void Debugger::set_paused(const bool value) {
    _is_paused = value;
    SDL_PauseAudioDevice(_gb->apu.device_id, value);
}

std::optional<uint16_t> Debugger::next_stop() const {
    return _next_stop;
}

void Debugger::set_next_stop(const std::optional<uint16_t> addr) {
    _next_stop = addr;
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

    io.Fonts->AddFontFromMemoryCompressedTTF(
        LiberationMono_compressed_data, 
        LiberationMono_compressed_size, 
        15.0f
    );

    // const auto dockspace_id = ImGui::GetID("##dockspace");
    // ImGui::DockBuilderRemoveNode(dockspace_id);
    // ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    // ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

    // ImGui::DockBuilderFinish(dockspace_id); 
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

    while(_gb->is_running) {

        static const uint32_t max_ticks = CLOCK_SPEED / FRAMERATE;

        if(!SDL_GetQueuedAudioSize(_gb->apu.device_id)) {

            uint32_t frame_ticks = 0;

            while(!_is_paused && frame_ticks < max_ticks) {

                Emulator::execute_instr(_gb.get());
                Emulator::update_ppu(_gb.get());

                if(_gb->cpu.is_double_speed) {
                    Emulator::execute_instr(_gb.get());
                    Emulator::update_ppu(_gb.get());
                }

                Emulator::update_timer(_gb.get());
                Emulator::update_apu(_gb.get());
                Emulator::check_interrupts(_gb.get());

                frame_ticks += _gb->cpu.ticks;
                
//                 if(_gb.cpu.reg.PC == _next_stop) {
//                     _next_stop = 0;
//                     _is_paused = true;
//                 }

//                 if(is_breakpoint(_gb.cpu.reg.PC)) {
//                     window_disassembly().set_goto_addr(_gb.cpu.reg.PC);
//                     _is_paused = true;
//                 }
            }
        }

        render();

        while(SDL_PollEvent(&event))
            handle_event(event);
    }

    Emulator::save_ram(_gb.get());
}

void Debugger::handle_event(SDL_Event event) {

    ImGui_ImplSDL2_ProcessEvent(&event);

    switch(event.type) {
        case SDL_QUIT:
            _gb->is_running = false;
            break;

        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_CLOSE)
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

    const auto flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##workspace", nullptr, flags);
    ImGui::PopStyleVar();

    ImGui::DockSpace(ImGui::GetID("##dockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    _menu.render();

    for(const auto &[_, window] : _windows)
        if(window->is_open())
            window->render();

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
    if(is_breakpoint(addr))
        return;

    _breakpoints.push_back(addr);
}

void Debugger::remove_breakpoint(const uint16_t addr) {
    for(size_t i = 0; i < _breakpoints.size(); ++i) {
        if(_breakpoints[i] == addr) {
            _breakpoints.erase(_breakpoints.begin() + i);
            return;
        }
    }
}

const std::vector<uint16_t>& Debugger::breakpoints() const {
    return _breakpoints;
}
