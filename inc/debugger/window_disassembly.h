#pragma once
#include "window.h"


class WindowDisassembly final : public Window {
    public:
        explicit WindowDisassembly(Debugger &);

        void render() override;
        const char *title() const override;

    private:
        bool _follow_pc;
        bool _goto_lock;
        uint32_t _goto_addr;

        void prefix_region(uint16_t) const;
        void line_instr(uint16_t &addr);
        void line_data(uint16_t &addr);

        static bool is_executable(uint16_t) ;
        static std::string get_region_label(uint16_t) ;
        uint16_t get_nth_line_addr(uint16_t);
        uint16_t get_line_count(uint16_t);
};
