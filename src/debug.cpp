extern "C" {
    #include "jgbc.h"
    #include "mmu.h"
    #include "cpu.h"
    #include "mbc.h"
    #include "cart.h"
    #include "ppu.h"
    #include "apu.h"
    #include "input.h"
}

#include <iostream>
#include <SDL.h>

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_sdl.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include "imgui_memory_editor/imgui_memory_editor.h"
#include "glad/glad.h"

#include "debug.h"
#include "font.h"


int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: jgbc_debugger.exe <path to rom>\n");
        return EXIT_FAILURE; 
    }

    GameBoy *gb = new GameBoy();
    init(gb);

    if(!load_rom(gb, argv[1])) {
        fprintf(stderr, "ERROR: Cannot load rom file\n");
        return EXIT_FAILURE;
    }

    Debugger *dbg = new Debugger(gb);
    dbg->run();

    delete dbg;
    delete gb;

    return EXIT_SUCCESS;
}

Debugger::Debugger(GameBoy *gb) {

    this->gb = gb;
    init_windows();

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
        WINDOW_TITLE, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        DEBUGGER_WIDTH, 
        DEBUGGER_HEIGHT, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    init_gl();
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if(!gladLoadGL()) {
        fprintf(stderr, "ERROR: Cannot load OpenGL extensions!\n");
        exit(EXIT_FAILURE);
    }

    init_imgui();

    is_paused = true;
    breakpoint_head = nullptr;

    set_window_title();
}

Debugger::~Debugger() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Debugger::run() {

    SDL_Event event;
    gb->is_running = true;

    while(gb->is_running) {

        static const uint32_t max_ticks = CLOCK_SPEED / FRAMERATE;
        uint32_t frame_ticks = 0;

        while(!is_paused && frame_ticks < max_ticks) {
            execute_instr(gb);
            update_timer(gb);

            update_ppu(gb);

            check_interrupts(gb);
            frame_ticks += gb->cpu.ticks;
        }

        frame_ticks -= max_ticks;
        render();

        if(find_breakpoint(REG(PC)) != nullptr) {
            is_paused = true;
        }

        while(SDL_PollEvent(&event)) {
            handle_event(event);
        }
    }
}

void Debugger::init_gl() {

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
}

void Debugger::init_imgui() {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init();

    ImGui::StyleColorsDark();
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        LiberationMono_compressed_data, 
        LiberationMono_compressed_size, 
        16.0f
    );
}

void Debugger::handle_event(SDL_Event event) {

    ImGui_ImplSDL2_ProcessEvent(&event);

    switch(event.type) {
        case SDL_QUIT:
            gb->is_running = 0;
            break;

        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_CLOSE) {
                gb->is_running = 0;
            }
            break;

        case SDL_KEYDOWN:
            set_key(gb, event.key.keysym.scancode, true);
            break;

        case SDL_KEYUP:
            set_key(gb, event.key.keysym.scancode, false);
            break;
    }
}

void Debugger::init_windows() {
    main_menu = MainMenu(this);
    window_break = WindowBreak(this);
    window_cart_info = WindowCartInfo(this);
    window_ctrl = WindowCTRL(this);
    window_disasm = WindowDisasm(this);
    window_emu = WindowEmu(this);
    window_mem = WindowMem(this);
    window_reg = WindowReg(this);
    window_stack = WindowStack(this);
}

void Debugger::render() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##workspace", nullptr, flags);
    ImGui::PopStyleVar();

    ImGui::DockSpace(ImGui::GetID("##dockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruDockspace);

    main_menu.show();

    window_emu.show();
    window_break.show();
    window_ctrl.show();
    window_disasm.show();
    window_mem.show();
    window_cart_info.show();
    window_reg.show();
    window_stack.show();

    // ImGui::ShowDemoWindow();
    ImGui::End();

    ImGui::Render();
    SDL_GL_MakeCurrent(window, gl_context);

    glClearColor(BG_COLOUR);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, (int) ImGui::GetIO().DisplaySize.x, (int) ImGui::GetIO().DisplaySize.y);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

void Debugger::set_window_title() {
    char buffer[50];
    snprintf(buffer, 50, "%s - %s (DEBUGGER)", WINDOW_TITLE, gb->cart.title);
    SDL_SetWindowTitle(window, buffer);
}

/*
    Breakpoints
*/

bool Debugger::create_breakpoint(const uint16_t addr) {

    const Breakpoint *found = Debugger::find_breakpoint(addr);

    if(found == nullptr) {

        Breakpoint *bp = new Breakpoint();
        bp->addr = addr;

        if(breakpoint_head == nullptr) {
            bp->next = nullptr;
            breakpoint_head = bp;
        } else {
            bp->next = breakpoint_head;
            breakpoint_head = bp;
        }
        
        return true;
    }
    
    return false; 
}

Breakpoint *Debugger::find_breakpoint(const uint16_t addr) {

    if(breakpoint_head != nullptr) {

        Breakpoint *item = breakpoint_head;

        while(item) {
            
            if(item->addr == addr) {
                return item;
            }

            item = item->next;
        }
    }

    return nullptr; 
}

bool Debugger::remove_breakpoint(const uint16_t addr) {

    Breakpoint *prev = nullptr;
    Breakpoint *curr= nullptr;

    if(breakpoint_head != nullptr) {
       
        curr = breakpoint_head;

        while(curr) {
            
            if(curr->addr == addr) {
                
                if(curr == breakpoint_head) {

                    if(breakpoint_head->next == nullptr) {
                        breakpoint_head = nullptr;
                    } else {
                        breakpoint_head = curr->next;
                    }
                } 
                else {
                    prev->next = curr->next; 
                }

                delete curr;
                return true;
            }
            
            prev = curr;
            curr = curr->next;
        }

    }

    return false;
}

/*
    Debugger Windows
*/

void MainMenu::show() {
    
    if(!is_open) {
        return;
    }

    if(ImGui::BeginMenuBar()) {

        if(ImGui::BeginMenu("jgbc")) {
            if(ImGui::MenuItem("Quit", "Alt+F4"))  { 
                dbg->gb->is_running = false;
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Emulator")) {
            if(ImGui::MenuItem("Pause / Play"))  { 
                dbg->is_paused = !dbg->is_paused;
            }

            if(ImGui::MenuItem("Restart"))  { 
                dbg->gb->cpu.reg.PC = 0x100;
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Tools")) {
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Breakpoints", nullptr, &dbg->window_break.is_open);
            ImGui::MenuItem("Cartridge Info", nullptr, &dbg->window_cart_info.is_open);
            ImGui::MenuItem("Controls", nullptr, &dbg->window_ctrl.is_open);
            ImGui::MenuItem("Disassembly", nullptr, &dbg->window_disasm.is_open);
            ImGui::MenuItem("Emulator", nullptr, &dbg->window_emu.is_open);
            ImGui::MenuItem("Registers", nullptr, &dbg->window_reg.is_open);
            ImGui::MenuItem("Stack", nullptr, &dbg->window_stack.is_open);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void WindowBreak::show() {

    static GameBoy *gb = dbg->gb;

    if(ImGui::Begin("Breakpoints", &is_open)) {

        static const ImVec4 addr_colour = ImVec4(ADDR_COLOUR);

        ImGui::BeginChild("##scroll");
        ImGui::Columns(2);
        
        ImGui::Text("Instruction Breakpoints");
        const ImS32 s32_one = 1, s32_fifty = 50;
        static int32_t addr = 0;

        ImGui::InputScalar("ADDR", ImGuiDataType_S32, &addr, &s32_one, &s32_fifty, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
        if(ImGui::Button("Add")) {
            dbg->create_breakpoint(addr);
        }

        if(dbg->breakpoint_head == nullptr) {
            ImGui::Text("No breakpoints set");
        } 
        else {

            Breakpoint *curr = dbg->breakpoint_head;
            uint16_t i = 1;

            while(curr) {
                ImGui::Text("%d: ", i);
                ImGui::SameLine();
                ImGui::TextColored(addr_colour, "0x%04X", curr->addr);
                ImGui::SameLine();
                ImGui::Text(find_instr(gb, SREAD8(curr->addr)).disassembly, 0x0, 0x0);

                curr = curr->next;
                i++;
            }
        }

        ImGui::NextColumn();
        ImGui::Text("Access Breakpoints");

        ImGui::EndChild();
    }

    ImGui::End();
}

void WindowCartInfo::show() {

    if(ImGui::Begin("Cart Info", &is_open)) {

        ImGui::Text("Title: %s", dbg->gb->cart.title);
        ImGui::Text("GBC Flag: %02X", dbg->gb->cart.gbc_flag);
        ImGui::Text("Cartridge Type: %02X", dbg->gb->cart.type);
        ImGui::Text("ROM Size: %d x %d KB", dbg->gb->cart.rom_size, ROM_BANK_SIZE);
        ImGui::Text("RAM Size: %d x %d KB", dbg->gb->cart.ram_size, EXTRAM_BANK_SIZE);

        ImGui::End();
    }
}

void WindowCTRL::show() {

    static GameBoy *gb = dbg->gb;

    if(ImGui::Begin("Controls", &is_open)) {

        ImGui::Columns(2);
        ImGui::Text("Debugger");

        if(ImGui::Button((dbg->is_paused) ? "Play" : "Pause")) {
            dbg->is_paused = !dbg->is_paused;
        }

        if(ImGui::Button("Step Over")) {
            step_over();
        }

        if(ImGui::Button("Step Into")) {
            step_into();
        }

        if(ImGui::Button("Step Out")) {
            step_out();
        }

        if(ImGui::Button("Restart")) {
            REG(PC) = 0x100;
        }

        ImGui::NextColumn();
        ImGui::Text("View");

         const ImS32 s32_one = 1, s32_fifty = 50;
         static uint32_t addr = 0;

         ImGui::InputScalar("ADDR", ImGuiDataType_S32, &addr, &s32_one, &s32_fifty, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
         if(ImGui::Button("Goto")) {
            
         }
    }
    
    ImGui::End();
}

void WindowCTRL::step_into() {
    
    static GameBoy *gb = dbg->gb;
    const uint8_t opcode = SREAD8(REG(PC));

    if(is_jump_call(opcode) || is_subroutine_call(opcode)) {
        dbg->next_stop = SREAD16(REG(PC) + 1);
    } 
    else if(is_jump_signed(opcode)) {

        // Address just before the jump destination (done like this because the PC hasn't been incremented yet)
        const uint16_t before_addr = REG(PC) + (int8_t) SREAD8(REG(PC) + 1);

        Instruction before_instr = find_instr(gb, SREAD8(before_addr));
        dbg->next_stop = before_addr + before_instr.length;
    } 
    else {
        run_to_next();
    }

    dbg->is_paused = false;
}

void WindowCTRL::step_over() {
    static GameBoy *gb = dbg->gb;
    const uint8_t opcode = SREAD8(REG(PC));

    if(is_jump_call(opcode)) {
        dbg->next_stop = SREAD16(REG(PC) + 1);
    } else {
        run_to_next();
    }

    dbg->is_paused = false;
}

void WindowCTRL::step_out() {

}

void WindowCTRL::run_to_next() {
    static GameBoy *gb = dbg->gb;
    Instruction instr = find_instr(gb, SREAD8(REG(PC)));
    dbg->next_stop = REG(PC) + instr.length;
}

bool WindowCTRL::is_subroutine_call(const uint8_t opcode) {
    switch(opcode) {
        case 0xC4:
        case 0xCC:
        case 0xCD:
        case 0xD4:
        case 0xD3:
            return true;
    }
    return false;
}

bool WindowCTRL::is_jump_call(const uint8_t opcode) {
    switch(opcode) {
        case 0xC2:
        case 0xC3:
        case 0xCA:
        case 0xD2:
        case 0xDA:
            return true;
    }
    return false;
}

bool WindowCTRL::is_jump_signed(const uint8_t opcode) {
    switch(opcode) {
        case 0x18:
        case 0x20:
        case 0x28:
        case 0x30:
            return true;
    }
    return false;
}

void WindowDisasm::show() {
        
    static const ImVec4 addr_colour = ImVec4(ADDR_COLOUR);
    static const ImVec4 data_colour = ImVec4(DATA_COLOUR);
    static const ImVec4 instr_colour = ImVec4(INSTR_COLOUR);
    static const ImVec4 curr_colour = ImVec4(CURR_COLOUR);

    static GameBoy *gb = dbg->gb;

    if(ImGui::Begin("Disassembly", &is_open)) {

        ImGui::BeginChild("##scroll");
                
        static const uint16_t instr_count = get_instr_count(WRAMNN_END);

        ImGuiListClipper clipper(instr_count);
        uint16_t addr = 0x0;

        while(clipper.Step()) {
            addr = get_nth_instr_addr(clipper.DisplayStart);

            for(uint16_t i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {

                draw_break_selectable(addr);
                ImGui::SameLine();

                const uint8_t opcode = SREAD8(addr);
                const Instruction instr = find_instr(gb, addr);
                const uint8_t length = instr.length;

                ImGui::TextColored(addr_colour, "0x%04X: ", addr);
                ImGui::SameLine();

                ImGui::TextColored(data_colour, "%02X ", opcode);
                ImGui::SameLine();

                if(length > 1 && opcode != 0xCB) {

                    uint16_t operand = 0;

                    if(length == 2) {
                        operand = SREAD8(addr + 1);

                        // If the operand is signed, get the two's complement
                        if(instr.signed_operand) {
                            operand = (~(operand - 1)) & 0x00FF;
                        }

                        ImGui::TextColored(data_colour, "%02X ", operand);
                        ImGui::SameLine();

                    } else if(length == 3) {
                        operand = SREAD16(addr + 1);
                        ImGui::TextColored(data_colour, "%02X %02X ", (operand & 0xFF00) >> 8, operand & 0xFF);
                        ImGui::SameLine();
                    }

                    ImGui::TextColored((addr == REG(PC)) ? curr_colour : instr_colour, instr.disassembly, operand);
                }
                // If there is no operand (or CB opcode which have no operand)
                else {
                    if(opcode == 0xCB) {
                        ImGui::TextColored(data_colour, "CB ");
                        ImGui::SameLine();
                    }

                    ImGui::TextColored((addr == REG(PC)) ? curr_colour : instr_colour, instr.disassembly, addr);
                }

                addr += instr.length;
            }
        }
        
        clipper.End();
        ImGui::EndChild();
    }

    ImGui::End();
}

void WindowDisasm::draw_break_selectable(uint16_t addr) {

    bool is_breakpoint = (dbg->find_breakpoint(addr) != nullptr);

    ImGui::PushID(addr);

    if(ImGui::Selectable("", is_breakpoint)) {

        if(is_breakpoint) {
            dbg->remove_breakpoint(addr);
        } else {
            dbg->create_breakpoint(addr);
        }
    }

    ImGui::PopID();
}

uint16_t WindowDisasm::get_nth_instr_addr(const uint16_t n) {

    for(uint32_t addr = 0, i = 0; addr <= 0xFFFF; i++) {

        // Don't interpret the header as instructions
        if(addr >= CART_HEADER_START && addr <= CART_HEADER_END) {
            addr++;
            continue;
        }

        const Instruction instr = find_instr(dbg->gb, addr);

        if(i >= n) {
            return addr;
        }

        addr += instr.length;
    }

    return 0;
}

uint16_t WindowDisasm::get_instr_count(const uint16_t limit_addr) {

    uint16_t count = 0;

    // Don't interpret the header as instructions
    for(uint32_t addr = 0; addr <= limit_addr;) {

        if(addr >= CART_HEADER_START && addr <= CART_HEADER_END) {
            addr++;
            continue;
        }

        const Instruction instr = find_instr(dbg->gb, addr);
        count++;

        addr += instr.length;
    }

    return count;
}

void WindowEmu::show() {

    const ImVec2 size = ImVec2(SCREEN_WIDTH * WINDOW_EMU_SCALE, SCREEN_HEIGHT * WINDOW_EMU_SCALE);
    ImGui::SetNextWindowSize(size);

    if(ImGui::Begin("Emulator", &is_open)) {

        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetWindowPos();

        uint32_t buf_offset;
        static const uint8_t *buf = dbg->gb->ppu.framebuffer;
        static const int scale = WINDOW_EMU_SCALE;

        for(uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            for(uint8_t y = 0; y < SCREEN_HEIGHT; y++) {

                buf_offset = (x * 3) + (y * SCREEN_WIDTH * 3);
                
                draw_list->AddRectFilled(
                    ImVec2(pos.x + x * scale, pos.y + y * scale), 
                    ImVec2(pos.x + x * scale + scale, pos.y + y * scale + scale),
                    IM_COL32(
                        buf[buf_offset + 0],
                        buf[buf_offset + 1],
                        buf[buf_offset + 2],
                        0xFF
                    )
                );
            }
        }
    }

    ImGui::End();
}

void WindowMem::show() {

    static GameBoy *gb = dbg->gb;

    if(ImGui::Begin("Memory", &is_open)) {

        static MemoryEditor mem;

        unsigned char *regions[WINDOW_MEM_REGION_COUNT] = { gb->mmu.rom00, gb->mmu.romNN, gb->mmu.vram, gb->mmu.extram, gb->mmu.wram00, gb->mmu.wramNN, gb->mmu.oam, gb->mmu.io, gb->mmu.hram };
        static const char* labels[WINDOW_MEM_REGION_COUNT] = { "ROM 00", "ROM NN", "VRAM", "EXTRAM", "WRAM 00", "WRAM NN", "OAM", "IO", "HRAM" };
        static const size_t sizes[WINDOW_MEM_REGION_COUNT] = { ROM_BANK_SIZE, ROM_BANK_SIZE, VRAM_BANK_SIZE, EXTRAM_BANK_SIZE, WRAM_BANK_SIZE, WRAM_BANK_SIZE, OAM_SIZE, IO_SIZE, HRAM_SIZE };
        static const size_t prefixes[WINDOW_MEM_REGION_COUNT] = { ROM00_START, ROMNN_START, VRAM_START, EXTRAM_START, WRAM00_START, WRAMNN_START, OAM_START, IO_START, HRAM_START };

        static int sel = 0;

        if(ImGui::BeginCombo("Region", labels[sel], 0)) {
            for(int i = 0; i < WINDOW_MEM_REGION_COUNT; i++) {
                bool is_selected = (i == sel);

                if(ImGui::Selectable(labels[i], is_selected)) {
                    sel = i;
                }

                if(is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        // Careful not to point to non existant extram
        if(!(prefixes[sel] == EXTRAM_START && gb->mmu.extram == nullptr)) {
            mem.DrawContents(regions[sel], sizes[sel], prefixes[sel]);
        } else {
            ImGui::Text("EXTRAM not available.");
        }
    }

    ImGui::End();
}

void WindowReg::show() {

    static GameBoy *gb = dbg->gb;

    if(ImGui::Begin("Registers")) {

        ImGui::Text("AF: %04X   LCDC: %02X", REG(AF), SREAD8(LCDC));
        ImGui::Text("BC: %04X   STAT: %02X", REG(BC), SREAD8(STAT));
        ImGui::Text("DE: %04X   LY:   %02X", REG(DE), SREAD8(LY));
        ImGui::Text("HL: %04X   IE:   %02X", REG(HL), SREAD8(IE));
        ImGui::Text("SP: %04X   IF:   %02X", REG(SP), SREAD8(IF));
        ImGui::Text("PC: %04X   IME:  %02X", REG(PC), REG(IME));
        ImGui::Text(
            "Z:%d N:%d H:%d C:%d", 
            GET_FLAG(FLAG_ZERO), 
            GET_FLAG(FLAG_SUBTRACT), 
            GET_FLAG(FLAG_HALFCARRY), 
            GET_FLAG(FLAG_CARRY)
        );

        ImGui::Separator();
        ImGui::Text("HALT: %02X", dbg->gb->cpu.is_halted);
    }

    ImGui::End();
}

void WindowStack::show() {

    static const ImVec4 addr_colour = ImVec4(ADDR_COLOUR);
    static GameBoy *gb = dbg->gb;

    if(ImGui::Begin("Stack")) {
        ImGui::BeginChild("##scroll");

        ImGuiListClipper clipper(0xFFFF - REG(SP));
        uint16_t addr;

        while(clipper.Step()) {
            for(int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {

                addr = REG(SP) + i;

                if(addr == REG(SP)) {
                    ImGui::Text("-> ");
                } else {
                    ImGui::Text("   ");
                }

                ImGui::SameLine();
                ImGui::TextColored(addr_colour, "0x%04X: ", addr);
                ImGui::SameLine();
                ImGui::Text("%02X", SREAD8(addr));
            }
        }

        clipper.End();
        ImGui::EndChild();
    }

    ImGui::End();
}
