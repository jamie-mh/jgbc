#pragma once

class Debugger;
class Window {
    public:
        Window() = default;
        virtual ~Window() = default;
        virtual void render() = 0;

        bool is_open() const {
            return _is_open;
        }

        void set_open(const bool value) {
            _is_open = value;
        }

    protected:
        bool _is_open = true;
};
