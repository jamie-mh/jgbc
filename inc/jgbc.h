#pragma once


typedef struct {
    int invalid_option_index;

    const char *rom_path;

    bool should_print_serial;
    bool should_show_help;
    bool is_headless;
    bool should_print_info;
}
CliArgs;
