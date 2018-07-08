extern "C" {
    #include "lxgbc.h"
    #include "ram.h"
    #include "cpu.h"
    #include "rom.h"
}

#include "debugger/debugger.h"
#include "imgui/imgui.h"
#include "debugger/window_disasm.h"
#include "imgui_dock/imgui_dock.h"

static void draw_break_selector(gbc_debugger *, unsigned short);
static unsigned short get_nth_instr_addr(gbc_system *, const unsigned short);
static unsigned short get_instr_count(gbc_system *, const unsigned short);


void window_disasm_show(gbc_system *gbc, gbc_debugger *debugger) {

    if(ImGui::BeginDock(WINDOW_DISASM_TITLE)) {

        ImGui::BeginChild("Scrolling");
        
        static const ImVec4 addr_colour = ImVec4(0.38f, 0.49f, 0.55f, 1.0f);
        static const ImVec4 data_colour = ImVec4(1.0f, 0.96f, 0.62f, 1.0f);
        static const ImVec4 instr_colour = ImVec4(0.93f, 0.93f, 0.93f, 1.0f);
        static const ImVec4 curr_colour = ImVec4(0.46f, 1.0f, 0.01f, 1.0f);
                
        static const unsigned short instr_count = get_instr_count(gbc, WRAMNN_END);

        ImGuiListClipper clipper(instr_count);
        unsigned short pointer = 0x0;

        while(clipper.Step()) {
            pointer = get_nth_instr_addr(gbc, clipper.DisplayStart);

            for(int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {

                draw_break_selector(debugger, pointer);
                ImGui::SameLine();

                unsigned char opcode = read_byte(gbc->ram, pointer);

                gbc_instruction instr = find_instr(opcode, pointer, gbc);
                unsigned char length = instr.length;

                ImGui::TextColored(addr_colour, "0x%04X: ", pointer);
                ImGui::SameLine();

                ImGui::TextColored(data_colour, "%02X ", opcode);
                ImGui::SameLine();

                if(length > 1 && opcode != 0xCB) {

                    // Read the operand based on the length and increment the pointer
                    unsigned short operand = 0;
                    if(length == 2) {
                        operand = read_byte(gbc->ram, pointer + 1);

                        // If the operand is signed, get the two's complement
                        if(instr.signed_operand) {
                            operand = (~(operand - 1)) & 0x00FF;
                        }

                        ImGui::TextColored(data_colour, "%02X ", operand);
                        ImGui::SameLine();

                    } else if(length == 3) {
                        operand = read_short(gbc->ram, pointer + 1);
                        ImGui::TextColored(data_colour, "%02X %02X ", (operand & 0xFF00) >> 8, operand & 0xFF);
                        ImGui::SameLine();
                    }

                    ImGui::TextColored((pointer == gbc->cpu->registers->PC) ? curr_colour : instr_colour, instr.disassembly, operand);
                }
                // If there is no operand (or CB opcode which have no operand)
                else {
                    if(opcode == 0xCB) {
                        ImGui::TextColored(data_colour, "CB ");
                        ImGui::SameLine();
                    }

                    ImGui::TextColored((pointer == gbc->cpu->registers->PC) ? curr_colour : instr_colour, instr.disassembly, pointer);
                }

                pointer += instr.length;
            }
        }
        
        clipper.End();
        ImGui::EndChild();
    }

    ImGui::EndDock();
}

// Draws the breakpoint selectable
static void draw_break_selector(gbc_debugger *debugger, unsigned short pointer) {

    bool is_breakpoint = (find_breakpoint(pointer, debugger) != nullptr);

    ImGui::PushID(pointer);
    if(ImGui::Selectable("", is_breakpoint)) {
        if(is_breakpoint) {
            remove_breakpoint(pointer, debugger);
        } else {
            add_breakpoint(pointer, debugger);
        }
    }
    ImGui::PopID();
}

// Gets the address in memory of the nth instruction
static unsigned short get_nth_instr_addr(gbc_system *gbc, const unsigned short n) {

    for(unsigned int pointer = 0, i = 0; pointer <= 0xFFFF; i++) {

        // Don't interpret the header as instructions
        if(pointer >= ROM_HEADER_START && pointer <= ROM_HEADER_END) {
            pointer++;
            continue;
        }

        const unsigned char opcode = read_byte(gbc->ram, pointer);
        const gbc_instruction instr = find_instr(opcode, pointer, gbc);

        if(i >= n) {
            return pointer;
        }

        pointer += instr.length;
    }

    return 0;
}

// Gets the number of instructions from the start of memory to the limit
static unsigned short get_instr_count(gbc_system *gbc, const unsigned short limit_addr) {

    unsigned short count;

    // Don't interpret the header as instructions
    for(unsigned int pointer = 0; pointer <= limit_addr;) {

        if(pointer >= ROM_HEADER_START && pointer <= ROM_HEADER_END) {
            pointer++;
            continue;
        }

        const unsigned char opcode = read_byte(gbc->ram, pointer);
        const gbc_instruction instr = find_instr(opcode, pointer, gbc);
        count++;

        pointer += instr.length;
    }

    return count;
}