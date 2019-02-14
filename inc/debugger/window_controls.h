#pragma once
#include "window.h"

class Debugger;
class WindowControls final : public Window {
	public:
		WindowControls(Debugger &dbg) : _dbg(dbg) { }
		void render() override;

    private:
		Debugger &_dbg;

        void step_into();
        void step_over();
        void step_out();
        void run_to_next();

        bool is_subroutine_call(uint8_t opcode) const;
        bool is_jump_call(uint8_t opcode) const;
        bool is_jump_signed(uint8_t opcode) const;
        bool is_return(uint8_t opcode) const;
};
