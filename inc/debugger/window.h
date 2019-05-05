#pragma once

class Debugger;
class Window {
    public:
        Window() = default;
        virtual ~Window() = default;

        bool is_open = true;
        virtual void render() = 0;
};
