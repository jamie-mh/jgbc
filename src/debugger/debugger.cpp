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

#include "debugger.h"
#include "window_emu.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "glad/glad.h"

static void handle_event(SDL_Event, gbc_system *);

static void init_debugger(gbc_debugger *);
static void init_imgui(gbc_debugger *);
static void render(gbc_system *, gbc_debugger *, ImGuiIO &);


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

    // Skip the Nintendo logo
    write_byte(gbc, BOOTROM_DISABLE, 1, false);
    gbc->cpu->registers->PC = PROGRAM_START;

    gbc_debugger *debugger = new gbc_debugger();

    init_debugger(debugger);
    set_window_title(debugger->window, gbc->rom->title);
    init_imgui(debugger);

    ImGuiIO &io = ImGui::GetIO(); 
    (void) io;
    io.Fonts->AddFontFromFileTTF("LiberationSans-Regular.ttf", 18.0f);

    // Main endless loop 
    SDL_Event event;
    unsigned int last_time = 0;

    while(gbc->is_running) {

        static const unsigned int max_clocks = CLOCK_SPEED / FRAMERATE;
        static const unsigned char millis_per_frame = (1 / FRAMERATE) * 1000.0;

        unsigned int frame_clocks = 0;
        last_time = SDL_GetTicks();

        // Run the clocks for this frame
        while(frame_clocks < max_clocks) {

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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(debugger->gl_context);
    SDL_DestroyWindow(debugger->window);
    SDL_Quit();

    return 0;
}

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
        1280, 
        720, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    debugger->gl_context = SDL_GL_CreateContext(debugger->window);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if(!gladLoadGL()) {
        std::cerr << "Cannot load OpenGL extensions!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

static void init_imgui(gbc_debugger *debugger) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL2_InitForOpenGL(debugger->window, debugger->gl_context);
    ImGui_ImplOpenGL3_Init();

    ImGui::StyleColorsDark();
}

static void render(gbc_system *gbc, gbc_debugger *debugger, ImGuiIO &io) {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(debugger->window);
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    window_emu_tick(gbc);

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
    }
}