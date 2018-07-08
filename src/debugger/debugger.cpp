extern "C" {
    #include "lxgbc.h"
    #include "ram.h"
    #include "cpu.h"
    #include "mbc.h"
    #include "rom.h"
    #include "ppu.h"
    #include "sound.h"
    #include "input.h"
    #include "emu.h"
}

#include <iostream>

#include "imgui/imgui.h"
#include "imgui_impl/imgui_impl_sdl.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include "imgui_dock/imgui_dock.h"
#include "glad/glad.h"

#include "debugger/debugger.h"
#include "debugger/menu.h"
#include "debugger/window_emu.h"
#include "debugger/window_perf.h"
#include "debugger/window_disasm.h"
#include "debugger/window_reg.h"
#include "debugger/window_ctrl.h"
#include "debugger/window_mem.h"
#include "debugger/window_break.h"
#include "debugger/window_stack.h"


static void init_debugger(gbc_debugger *);
static void destroy_debugger(gbc_debugger *);

static void init_imgui(gbc_debugger *);

static void render(gbc_system *, gbc_debugger *, ImGuiIO &);
static void handle_event(SDL_Event, gbc_system *);


int main(int argc, char **argv) {
    
    if(argc < 2) {
        std::cerr << "Please specify a rom file." << std::endl;
        return EXIT_FAILURE; 
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "ERROR: Cannot load SDL" << std::endl;
        return EXIT_FAILURE;
    }

    gbc_system *gbc = new gbc_system();
    init_system(gbc, argv[1]);

    gbc_debugger *debugger = new gbc_debugger();
    init_debugger(debugger);

    // Skip the Nintendo logo
    write_byte(gbc, BOOTROM_DISABLE, 1, false);
    gbc->cpu->registers->PC = PROGRAM_START;

    set_window_title(debugger->window, gbc->rom->title, true);
    init_imgui(debugger);

    ImGuiIO &io = ImGui::GetIO(); 
    (void) io;

    // Main endless loop 
    SDL_Event event;
    unsigned int last_time = 0;

    while(gbc->is_running) {

        static const unsigned int max_clocks = CLOCK_SPEED / FRAMERATE;
        static const unsigned char millis_per_frame = (1 / FRAMERATE) * 1000.0;

        unsigned int frame_clocks = 0;
        last_time = SDL_GetTicks();

        // Run the clocks for this frame
        while(!debugger->is_paused && frame_clocks < max_clocks) {

            unsigned char clocks = 0;

            if(gbc->cpu->is_halted == false) {
                clocks = execute_instr(gbc);                
            } else {
                clocks = 4;
            }

            check_interrupts(gbc);

            update_timer(gbc, clocks);
            update_ppu(gbc, clocks);

            frame_clocks += clocks;
            debugger->clocks += clocks;

            if(find_breakpoint(gbc->cpu->registers->PC, debugger) != nullptr) {
                debugger->is_paused = true;
            }

            if(gbc->cpu->registers->PC == debugger->next_addr) {
                debugger->is_paused = true;
                debugger->next_addr = 0;
            }

            write_byte(gbc, JOYP, 0x1F, false); // Temp simulate no buttons
        }

        render(gbc, debugger, io);

        // Run at the framerate and remove the time it took to compute this frame
        int execute_time = SDL_GetTicks() - last_time;

        if(execute_time < millis_per_frame) {
            SDL_Delay(millis_per_frame - execute_time);
        }

        while(SDL_PollEvent(&event)) {
            handle_event(event, gbc);
        }
    }

    destroy_debugger(debugger);
    return 0;
}

/*
*   Setup
*/

static void init_debugger(gbc_debugger *debugger) {

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    debugger->window = SDL_CreateWindow(
        MAIN_WINDOW_TITLE, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        DEBUGGER_WIDTH, 
        DEBUGGER_HEIGHT, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    debugger->gl_context = SDL_GL_CreateContext(debugger->window);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if(!gladLoadGL()) {
        std::cerr << "Cannot load OpenGL extensions!" << std::endl;
        exit(EXIT_FAILURE);
    }

    debugger->next_addr = 0;
    debugger->is_paused = true;

    debugger->modal_rom_open = false;

    debugger->clocks = 0;
    debugger->breakpoint_head = nullptr;
}

static void destroy_debugger(gbc_debugger *debugger) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(debugger->gl_context);
    SDL_DestroyWindow(debugger->window);
    SDL_Quit();
}

static void init_imgui(gbc_debugger *debugger) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL2_InitForOpenGL(debugger->window, debugger->gl_context);
    ImGui_ImplOpenGL3_Init();

    ImGui::StyleColorsDark();
    ImGui::GetIO().Fonts->AddFontFromFileTTF("LiberationMono-Regular.ttf", 18.0f);
}

static void render(gbc_system *gbc, gbc_debugger *debugger, ImGuiIO &io) {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(debugger->window);
    ImGui::NewFrame();

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Workspace", nullptr, flags);
    menu_show(gbc, debugger);

    ImGui::BeginWorkspace();
    ImGui::SetNextDock(ImGuiDockSlot_Left);
    window_emu_show(gbc);

    ImGui::SetNextDock(ImGuiDockSlot_Bottom);
    window_break_show(gbc, debugger);

    ImGui::SetNextDock(ImGuiDockSlot_Top);
    window_perf_show(debugger);

    ImGui::SetNextDock(ImGuiDockSlot_Right);
    window_ctrl_show(gbc, debugger);

    ImGui::SetNextDockParentToRoot();
    ImGui::SetNextDock(ImGuiDockSlot_Right);
    window_disasm_show(gbc, debugger);

    ImGui::SetNextDock(ImGuiDockSlot_Right);
    window_mem_show(gbc);

    ImGui::SetNextDock(ImGuiDockSlot_Bottom);
    window_reg_show(gbc);

    ImGui::SetNextDock(ImGuiDockSlot_Bottom);
    window_stack_show(gbc);
    ImGui::EndWorkspace();

    ImGui::End();
    //ImGui::ShowDemoWindow();

    ImGui::Render();
    SDL_GL_MakeCurrent(debugger->window, debugger->gl_context);

    glClearColor(BG_COLOUR);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(debugger->window);
}

static void handle_event(SDL_Event event, gbc_system *gbc) {

    ImGui_ImplSDL2_ProcessEvent(&event);
    switch(event.type) {
        case SDL_QUIT:
            gbc->is_running = 0;
            break;

        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_CLOSE) {
                gbc->is_running = 0;
            }
            break;

        case SDL_KEYUP:
            handle_input(gbc, event.key); 
            break;
    }
}

/*
*   Breakpoint
*/

// Adds a breakpoint element to the breakpoint linked list
bool add_breakpoint(const unsigned int address, gbc_debugger *debugger) {

    const gbc_breakpoint *found = find_breakpoint(address, debugger);

    if(found == nullptr) {
        
        gbc_breakpoint *bp = new gbc_breakpoint();
        bp->address = address;

        if(debugger->breakpoint_head == nullptr) {
            bp->next = nullptr;
            debugger->breakpoint_head = bp;
        } else {
            bp->next = debugger->breakpoint_head;
            debugger->breakpoint_head = bp;
        }
        
        return true;
    }
    
    return false; 
}

// Returns a pointer to the breakpoint element with the specifed address in the breakpoint linked list provided it exists
gbc_breakpoint *find_breakpoint(const unsigned int address, gbc_debugger *debugger) {

    if(debugger->breakpoint_head != nullptr) {

        gbc_breakpoint *item = debugger->breakpoint_head;

        while(item) {
            
            if(item->address == address) {
                return item;
            }

            item = item->next;
        }
    }

    return nullptr; 
}

bool remove_breakpoint(const unsigned int address, gbc_debugger *debugger) {

    gbc_breakpoint *prev = nullptr;
    gbc_breakpoint *curr= nullptr;

    if(debugger->breakpoint_head != nullptr) {
       
        curr = debugger->breakpoint_head;

        while(curr) {
            
            if(curr->address == address) {
                
                if(curr == debugger->breakpoint_head) {

                    if(debugger->breakpoint_head->next == nullptr) {
                        debugger->breakpoint_head = nullptr;
                    } else {
                        debugger->breakpoint_head = curr->next;
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
