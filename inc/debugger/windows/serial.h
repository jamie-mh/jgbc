#pragma once
#include <sstream>
#include <cstdint>
#include "debugger/window.h"

namespace Windows {
    class Serial final : public Window {
        public:
            explicit Serial(Debugger &);

            void render() override;
            const char *title() const override;

            static void serial_write_handler(uint8_t);

        private:
            static std::stringstream _buffer;
    };
}
