#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<SDL2/SDL.h>

#ifndef max
	#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
	#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

// GameBoy Specification

struct gbc_system {
    char is_running;
    char interrupts_enabled;
    struct gbc_registers *registers;
    struct gbc_ram *ram;
};
