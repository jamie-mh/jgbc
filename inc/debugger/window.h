#pragma once

// Create local pointer named gb to use the C macros
#define INIT_GB_CTX() static auto gb = debugger().gb().get()


class Debugger;
class Window {
    public:
        explicit Window(Debugger &);
        virtual ~Window() = default;

        virtual void render() = 0;
        virtual const char *title() const = 0;

        void set_open(bool);
        bool is_open() const;

        Debugger &debugger() const;

    private:
        bool _is_open = true;
        Debugger &_debugger;
};
