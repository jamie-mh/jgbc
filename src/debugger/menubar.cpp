#include "debugger/debugger.h"
#include "debugger/menubar.h"
#include <imgui.h>

void MenuBar::render() const {
        
    if(!ImGui::BeginMenuBar()) {
        ImGui::EndMenuBar();
        return;
    }

    if(ImGui::BeginMenu("jgbc")) {
        if(ImGui::MenuItem("Quit", "Alt+F4")) 
            _dbg.gb.is_running = false;

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Emulator")) {
        if(ImGui::MenuItem("Pause / Play")) {
            _dbg.is_paused = !_dbg.is_paused;
            SDL_PauseAudioDevice(_dbg.gb.apu.device_id, _dbg.is_paused);
        }

        if(ImGui::MenuItem("Restart"))
            _dbg.gb.cpu.reg.PC = 0x100;

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Tools")) {
        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("View")) {
        ImGui::MenuItem("Breakpoints", nullptr, &(_dbg.get_window_breakpoints().is_open));
        ImGui::MenuItem("Cartridge Info", nullptr, &(_dbg.get_window_cart_info().is_open));
        ImGui::MenuItem("IO", nullptr, &(_dbg.get_window_io().is_open));
        ImGui::MenuItem("Controls", nullptr, &(_dbg.get_window_controls().is_open));
        ImGui::MenuItem("Disassembly", nullptr, &(_dbg.get_window_disassembly().is_open));
        ImGui::MenuItem("Emulator", nullptr, &(_dbg.get_window_emulator().is_open));
        ImGui::MenuItem("Memory", nullptr, &(_dbg.get_window_memory().is_open));
        ImGui::MenuItem("Registers", nullptr, &(_dbg.get_window_registers().is_open));
        ImGui::MenuItem("Stack", nullptr, &(_dbg.get_window_stack().is_open));
        ImGui::MenuItem("Audio", nullptr, &(_dbg.get_window_audio().is_open));

        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}
