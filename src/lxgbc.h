#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<SDL2/SDL.h>
#include<gtk/gtk.h>

// GameBoy Specification

struct gbc_system {
    int is_running;
    struct gbc_registers *registers;
    struct gbc_ram *ram;
};