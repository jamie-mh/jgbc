#include <imgui.h>
#include "debugger/debugger.h"
#include "debugger/menubar.h"


MenuBar::MenuBar(Debugger &debugger) : Window(debugger) {

}

void MenuBar::render() {
        
    if(!ImGui::BeginMenuBar()) {
        ImGui::EndMenuBar();
        return;
    }

    if(ImGui::BeginMenu("jgbc")) {
        if(ImGui::MenuItem("Quit", "Alt+F4")) 
            debugger().gb()->is_running = false;

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Emulator")) {
        if(ImGui::MenuItem("Pause / Play"))
            debugger().set_paused(!debugger().is_paused());

        if(ImGui::MenuItem("Restart"))
            Emulator::reset(debugger().gb().get());

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Tools"))
        ImGui::EndMenu();

    if(ImGui::BeginMenu("View")) {

        for(const auto &[_, window] : debugger().windows()) {

            if(ImGui::MenuItem(window->title(), nullptr, window->is_open()))
                window->set_open(!window->is_open());
        }

        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

const char *MenuBar::title() const {
    return nullptr;
}
