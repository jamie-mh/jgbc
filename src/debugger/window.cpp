#include "debugger/window.h"

Window::Window(Debugger &debugger) : _debugger(debugger) {}

void Window::set_open(const bool is_open) { _is_open = is_open; }

bool Window::is_open() const { return _is_open; }

Debugger &Window::debugger() const { return _debugger; }
