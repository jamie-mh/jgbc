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
            _dbg.gb().is_running = false;

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Emulator")) {
        if(ImGui::MenuItem("Pause / Play"))
            _dbg.set_paused(!_dbg.is_paused());

        if(ImGui::MenuItem("Restart"))
            Emulator::reset(_dbg.gb_p());

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Tools"))
        ImGui::EndMenu();

    if(ImGui::BeginMenu("View")) {
        if(ImGui::MenuItem("Breakpoints", nullptr, _dbg.window_breakpoints().is_open()))
            _dbg.window_breakpoints().set_open(!_dbg.window_breakpoints().is_open());

        if(ImGui::MenuItem("Cartridge Info", nullptr, _dbg.window_cart_info().is_open()))
            _dbg.window_cart_info().set_open(!_dbg.window_cart_info().is_open());

        if(ImGui::MenuItem("IO", nullptr, _dbg.window_io().is_open()))
            _dbg.window_io().set_open(!_dbg.window_io().is_open());

        if(ImGui::MenuItem("Controls", nullptr, _dbg.window_controls().is_open()))
            _dbg.window_controls().set_open(!_dbg.window_controls().is_open());

        if(ImGui::MenuItem("Disassembly", nullptr, _dbg.window_disassembly().is_open()))
            _dbg.window_disassembly().set_open(!_dbg.window_disassembly().is_open());

        if(ImGui::MenuItem("Emulator", nullptr, _dbg.window_emulator().is_open()))
            _dbg.window_emulator().set_open(!_dbg.window_emulator().is_open());

        if(ImGui::MenuItem("Memory", nullptr, _dbg.window_memory().is_open()))
            _dbg.window_memory().set_open(!_dbg.window_memory().is_open());

        if(ImGui::MenuItem("Registers", nullptr, _dbg.window_registers().is_open()))
            _dbg.window_registers().set_open(!_dbg.window_registers().is_open());

        if(ImGui::MenuItem("Stack", nullptr, _dbg.window_stack().is_open()))
            _dbg.window_stack().set_open(!_dbg.window_stack().is_open());

        if(ImGui::MenuItem("Audio", nullptr, _dbg.window_audio().is_open()))
            _dbg.window_audio().set_open(!_dbg.window_audio().is_open());

        if(ImGui::MenuItem("Palettes", nullptr, _dbg.window_palettes().is_open()))
            _dbg.window_palettes().set_open(!_dbg.window_palettes().is_open());

        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}
