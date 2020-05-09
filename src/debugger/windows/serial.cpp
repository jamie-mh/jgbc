#include <imgui.h>
#include <iostream>
#include "debugger/windows/serial.h"

using namespace Windows;

std::stringstream Serial::_buffer;

Serial::Serial(Debugger &debugger) : Window(debugger) {

}

void Serial::render() {

    if(!ImGui::Begin(title())) {
        ImGui::End();
        return;
    }

    if(ImGui::Button("Clear"))
        _buffer.str("");

    ImGui::BeginChild("##scroll");
    ImGui::Text("%s", _buffer.str().c_str());
    ImGui::EndChild();

    ImGui::End();
}

const char *Serial::title() const {
    return "Serial Output";
}

void Serial::serial_write_handler(const uint8_t data) {
    _buffer << static_cast<char>(data);
}
