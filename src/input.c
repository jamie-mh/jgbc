#include "lxgbc.h"
#include "input.h"
#include "ram.h"
#include "cpu.h"


void handle_input(gbc_system *gbc, SDL_KeyboardEvent key) {
    write_register(gbc, IF, IEF_JOYPAD, 1);    
    write_register(gbc, 0xFF00, 3, 0);
    write_register(gbc, 0xFF00, 4, 0);
}
