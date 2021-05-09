#pragma once
#include "debugger/window.h"

namespace Windows {
    class Controls final : public Window {
        public:
            explicit Controls(Debugger &);

            void render() override;
            [[nodiscard]] const char *title() const override;

        private:
            void step_into();
            void step_over();
            void run_to_next() const;

            static bool is_subroutine_call(uint8_t opcode) ;
            static bool is_jump_call(uint8_t opcode) ;
            static bool is_jump_signed(uint8_t opcode) ;
            static bool is_return(uint8_t opcode) ;
    };
}
