#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jgbc.h"
#include "main.h"

#include "ppu.h"
#include "cpu.h"
#include "apu.h"
#include "cart.h"
#include "mmu.h"
#include "input.h"


static void handle_event(GameBoy *, SDL_Event);
static void set_window_title(GameBoy *);
static void run(GameBoy *);
static void print_help();
static void serial_write_handler(uint8_t);
static CliArgs parse_cli_args(int, const char **);


int main(const int argc, const char **argv) {

    const CliArgs args = parse_cli_args(argc, argv);

    if(args.should_show_help) {
        print_help();
        return EXIT_SUCCESS;
    }

    if(args.rom_path == NULL) {
        fprintf(stderr, "Missing path to rom file.\n\n");
        print_help();
        return EXIT_FAILURE; 
    }

    if(args.invalid_option_index > -1) {
        fprintf(stderr, "Unknown option %s\n\n", argv[args.invalid_option_index]);
        print_help();
        return EXIT_FAILURE;
    }

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    GameBoy *gb = malloc(sizeof(GameBoy));
    init(gb);

    if(!load_rom(gb, args.rom_path)) {
        fprintf(stderr, "ERROR: Cannot load rom file\n");
        return EXIT_FAILURE;
    }

    if(!args.is_headless) {
        init_window(gb);
        set_window_title(gb);
    }

    if(args.should_print_serial)
        gb->mmu.serial_write_handler = serial_write_handler;

    if(args.should_print_info)
        print_cart_info(gb);

    SDL_PauseAudioDevice(gb->apu.device_id, 0);
    run(gb);

    SDL_Quit();
    return EXIT_SUCCESS;
}

static void run(GameBoy *gb) {

    SDL_Event event;
    gb->is_running = true;

    while(gb->is_running) {

        static const uint32_t max_ticks = CLOCK_SPEED / FRAMERATE;
        uint32_t frame_ticks = 0;

        while(frame_ticks < max_ticks) {
            execute_instr(gb);
            update_timer(gb);

            if(gb->cpu.is_double_speed) {
                execute_instr(gb);
                update_timer(gb);
            }

            update_ppu(gb);
            check_interrupts(gb);
            update_apu(gb);
            update_hdma(gb);

            frame_ticks += gb->cpu.ticks;
        }

        while(SDL_GetQueuedAudioSize(gb->apu.device_id))
            SDL_Delay(1);

        while(SDL_PollEvent(&event))
            handle_event(gb, event);
    }

    save_ram(gb);
}

static void handle_event(GameBoy *gb, const SDL_Event event) {

    switch(event.type) {
        case SDL_QUIT:
            gb->is_running = false;
            break;

        case SDL_KEYDOWN:
            set_key(gb, event.key.keysym.scancode, true);
            break;

        case SDL_KEYUP:
            set_key(gb, event.key.keysym.scancode, false);
            break;
    }
}

static void print_help() {
    printf( "Usage: jgbc <path to rom> options?\n");
    printf("Options:\n");
    printf("--serial: Output serial to terminal.\n");
    printf("--headless: Don't open a window.\n");
    printf("--info: Print cartridge info.\n");
    printf("--help: Show this help.\n");
}

static void set_window_title(GameBoy *gb) {
    char buffer[30];
    snprintf(buffer, 30, "%s - %s", WINDOW_TITLE, gb->cart.title);
    SDL_SetWindowTitle(gb->ppu.window, buffer);
}

CliArgs parse_cli_args(const int argc, const char **argv) {

    CliArgs result;
    result.invalid_option_index = -1;
    result.rom_path = NULL;
    result.is_headless = false;
    result.should_print_serial = false;
    result.should_show_help = false;
    result.should_print_info = false;

    if(argc < 1)
        return result;

    for(int i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if(strlen(arg) > 2 && arg[0] == '-' && arg[1] == '-') {
            const char *option = arg + 2 * sizeof(char);

            if(strcmp(option, "serial") == 0)
                result.should_print_serial = true;
            else if(strcmp(option, "headless") == 0)
                result.is_headless = true;
            else if(strcmp(option, "info") == 0)
                result.should_print_info = true;
            else if(strcmp(option, "help") == 0)
                result.should_show_help = true;
            else
                result.invalid_option_index = i;

            continue;
        }

        // Duplicate rom path, ambiguous
        if(result.rom_path != NULL) {
            result.rom_path = NULL;
            return result;
        }

        result.rom_path = arg;
    }

    return result;
}

static void serial_write_handler(const uint8_t data) {
    printf("%c", data);
}
