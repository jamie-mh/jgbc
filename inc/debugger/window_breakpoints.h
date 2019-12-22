#pragma once
#include "window.h"

class Debugger;
class WindowBreakpoints final : public Window {
    private:
        Debugger &_dbg;

    public:
        explicit WindowBreakpoints(Debugger &dbg) : _dbg(dbg) { }
        void render() override;
};
