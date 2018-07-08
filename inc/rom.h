#pragma once

#define TITLE_LENGTH 16

#define ROM_HEADER_START 0x100
#define ROM_HEADER_END 0x14F


void load_bootstrap(gbc_ram *);
bool load_rom(gbc_system *, const char *);