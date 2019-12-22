#pragma once
#include "window.h"

class Debugger;
class WindowDisassembly final : public Window {
    public:
        explicit WindowDisassembly(Debugger &dbg) : _dbg(dbg) { }
        void render() override;

        void set_follow_pc(bool);
        void set_goto_addr(uint16_t);

    private:
        Debugger &_dbg;

        bool _follow_pc = true;
        bool _goto_lock = false;
        uint32_t _goto_addr = 0x0;

        inline void prefix_region(uint16_t) const;
        void line_instr(uint16_t &addr) const;
        void line_data(uint16_t &addr) const;

        bool is_executable(uint16_t) const;
        std::string get_region_label(uint16_t) const;
        uint16_t get_nth_line_addr(uint16_t) const;
        uint16_t get_line_count(uint16_t) const;
};
