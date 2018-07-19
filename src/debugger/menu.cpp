extern "C" {
    #include "jgbc.h"
    #include "cpu.h"
}

#include "debugger/debugger.h"
#include "imgui/imgui.h"
#include "debugger/menu.h"
#include "imgui_dock/imgui_dock.h"
#include "debugger/modal_rom.h"


void menu_show(gbc_system *gbc, gbc_debugger *debugger) {

    if(ImGui::BeginMenuBar()) {

        if(ImGui::BeginMenu("jgbc")) {
            if(ImGui::MenuItem("Open")) {

            }

            ImGui::Separator();

            if(ImGui::MenuItem("Quit", "Alt+F4"))  { 
                gbc->is_running = false;
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Emulator")) {
            if(ImGui::MenuItem("Pause / Play"))  { 
                debugger->is_paused = !debugger->is_paused;
            }

            if(ImGui::MenuItem("Restart"))  { 
                gbc->cpu->registers->PC = PROGRAM_START;
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Window")) {
            static bool t = false;
            if(ImGui::MenuItem("Tileset", nullptr, &t, true))  { 

            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Tools")) {
            if(ImGui::MenuItem("Ram Dumper")) { 

            }

            if(ImGui::MenuItem("Rom Info")) {
                debugger->modal_rom_open = true;
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Help")) {
            if(ImGui::MenuItem("About"))  { 
                
            }

            if(ImGui::MenuItem("Help"))  { 
                
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if(debugger->modal_rom_open)
        modal_rom_show(gbc, debugger);
}