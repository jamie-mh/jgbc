#pragma once

#define DEBUGGER_WIDTH 1400
#define DEBUGGER_HEIGHT 900 
#define WINDOW_EMU_SCALE 3

#define BG_COLOUR 0.45f, 0.55f, 0.60f, 1.00f
#define ADDR_COLOUR 0.38f, 0.49f, 0.55f, 1.0f
#define DATA_COLOUR 1.0f, 0.96f, 0.62f, 1.0f
#define INSTR_COLOUR 0.93f, 0.93f, 0.93f, 1.0f
#define CURR_COLOUR 0.46f, 1.0f, 0.01f, 1.0f

#define WINDOW_MEM_REGION_COUNT 9

class Debugger;
class Window {
    protected:
        Debugger *dbg;

    public:
        Window(Debugger *dbg) {
            this->dbg = dbg;
            this->is_open = true;
        };
        bool is_open;
        virtual void show() = 0;
};

typedef struct Breakpoint {
    uint16_t addr;
    Breakpoint *next;
}
Breakpoint;

class MainMenu : public Window { 
    using Window::Window;
    public:
        virtual void show() override;
};

class WindowBreak : public Window {
    using Window::Window;
    public:
        virtual void show() override;
};

class WindowCartInfo : public Window {
    using Window::Window;
    public:
        virtual void show() override;
};

class WindowCTRL : public Window {
    using Window::Window;
    public:
        virtual void show() override;

    private:
        void step_into();
        void step_over();
        void step_out();
        void run_to_next();

        bool is_subroutine_call(const uint8_t opcode);
        bool is_jump_call(const uint8_t opcode);
        bool is_jump_signed(const uint8_t opcode);
};

class WindowDisasm : public Window {
    using Window::Window;
    public:
        virtual void show() override;

    private:
        void draw_break_selectable(const uint16_t addr);
        uint16_t get_nth_instr_addr(const uint16_t n);
        uint16_t get_instr_count(const uint16_t limit_addr);
};

class WindowEmu : public Window {
    using Window::Window;
    public:
        virtual void show() override;
};

class WindowMem : public Window {
    using Window::Window;
    public:
        virtual void show() override;
};

class WindowPerf : public Window {
    using Window::Window;
    public:
        virtual void show() override;
};

class WindowReg: public Window {
    using Window::Window;
    public:
        virtual void show() override;
};

class WindowStack: public Window {
    using Window::Window;
    public:
        virtual void show() override;
};

class Debugger {
    public:
        bool is_paused;
        uint16_t next_stop;
        GameBoy *gb;

        MainMenu main_menu = 0;
        WindowBreak window_break = 0;
        WindowCartInfo window_cart_info = 0;
        WindowCTRL window_ctrl = 0;
        WindowDisasm window_disasm = 0;
        WindowEmu window_emu = 0;
        WindowMem window_mem = 0;
        WindowPerf window_perf = 0;
        WindowReg window_reg = 0;
        WindowStack window_stack = 0;

        Debugger(GameBoy *gb);
        ~Debugger();

        Breakpoint *breakpoint_head;
        bool create_breakpoint(const uint16_t addr);
        bool remove_breakpoint(const uint16_t addr);
        Breakpoint *find_breakpoint(const uint16_t addr);

        void run();
        void render();

    private:
        SDL_Window *window;
        SDL_GLContext gl_context;

        void init_gl();
        void init_imgui();
        void init_windows();

        void set_window_title();
        void handle_event(SDL_Event event);
};