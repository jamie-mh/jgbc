#pragma once
#include "debugger/window.h"


namespace Windows {
    class IO final : public Window {
        public:
            explicit IO(Debugger &);

            void render() override;
            const char *title() const override;

        private:
            void draw_values(const char **, const uint16_t *, int) const;
            void draw_registers(const char **, uint16_t, const uint8_t *, int) const;
    };
}
