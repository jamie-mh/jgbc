#include "ppu.h"
#include "cpu.h"
#include "macro.h"
#include "mmu.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void update_render_mode(GameBoy *, uint8_t, bool);

static bool get_bg_tile_data_start(GameBoy *, uint16_t *);
static uint16_t get_tile_map_offset(Position);
static uint16_t get_tile_data_offset(GameBoy *gb, uint16_t, bool);
static void get_tile_row_data(GameBoy *gb, uint8_t, uint8_t, uint16_t, uint8_t *);
static uint16_t get_tile_pixel(Position, const uint8_t *, bool, const uint16_t *);
static TileAttributes get_tile_attributes(GameBoy *, uint16_t);
static void plot_tile_pixel(uint16_t *, Position, uint16_t);

static void render_bg_scan(GameBoy *, uint8_t);
static void render_window_scan(GameBoy *, uint8_t);
static void render_sprite_scan(GameBoy *, uint8_t);

static int sprite_cmp(const void *, const void *);
static uint16_t get_shade(uint8_t);
static void fill_shade_table(uint8_t, uint16_t *);

void init_ppu(GameBoy *gb) {
    gb->ppu.framebuffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t));

    gb->ppu.window = NULL;
    gb->ppu.renderer = NULL;
    gb->ppu.texture = NULL;
}

void reset_ppu(GameBoy *gb) {
    gb->ppu.scan_clock = 0;
    gb->ppu.frame_clock = 0;

    memset(gb->ppu.framebuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(int16_t));
    memset(gb->ppu.sprite_buffer, 0, 40 * sizeof(Sprite));
    memset(gb->ppu.bg_palette, 0, 32 * sizeof(uint16_t));
    memset(gb->ppu.obj_palette, 0, 32 * sizeof(uint16_t));
}

void init_window(GameBoy *gb) {
    gb->ppu.window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      SCREEN_WIDTH * SCREEN_INITIAL_SCALE, SCREEN_HEIGHT * SCREEN_INITIAL_SCALE,
                                      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    gb->ppu.renderer = SDL_CreateRenderer(gb->ppu.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderSetLogicalSize(gb->ppu.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowMinimumSize(gb->ppu.window, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_DisplayMode mode;
    mode.refresh_rate = FRAMERATE;
    SDL_SetWindowDisplayMode(gb->ppu.window, &mode);

    gb->ppu.texture = SDL_CreateTexture(gb->ppu.renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_STREAMING,
                                        SCREEN_WIDTH, SCREEN_HEIGHT);
}

void render_framebuffer(GameBoy *gb) {
    SDL_SetRenderDrawColor(gb->ppu.renderer, 0, 0, 0, 255);
    SDL_RenderClear(gb->ppu.renderer);

    SDL_UpdateTexture(gb->ppu.texture, NULL, gb->ppu.framebuffer, SCREEN_WIDTH * sizeof(uint16_t));

    SDL_RenderCopy(gb->ppu.renderer, gb->ppu.texture, NULL, NULL);
    SDL_RenderPresent(gb->ppu.renderer);
}

void update_ppu(GameBoy *gb) {
    const bool lcd_on = RREG(LCDC, LCDC_LCD_ENABLE);
    uint8_t ly = SREAD8(LY);

    update_render_mode(gb, ly, lcd_on);

    if (!lcd_on) {
        SWRITE8(LY, 0);
        gb->ppu.scan_clock = 0;
        return;
    }

    gb->ppu.scan_clock += gb->cpu.ticks;

    if (gb->ppu.scan_clock >= CLOCKS_PER_SCANLINE) {
        gb->ppu.scan_clock = 0;

        if (ly == 0) {
            fill_sprite_buffer(gb);
        }

        // Render scanlines (144 pixel tall screen)
        if (ly < 144) {
            render_bg_scan(gb, ly);
            render_window_scan(gb, ly);
            render_sprite_scan(gb, ly);
        }
        // End of frame, request vblank interrupt
        else if (ly == 144) {
            WREG(IF, IEF_VBLANK, 1);

            if (gb->ppu.window != NULL) {
                render_framebuffer(gb);
            }
        }

        ly = (ly == 153) ? 0 : ly + 1;
        SWRITE8(LY, ly);

        // Check if LY == LYC
        // And request an interrupt
        if (SREAD8(LYC) == ly) {
            WREG(STAT, STAT_COINCID_FLAG, 1);

            // If the LY == LYC interrupt is enabled, request it
            if (RREG(STAT, STAT_COINCID_INT)) {
                WREG(IF, IEF_LCD_STAT, 1);
            }
        } else {
            WREG(STAT, STAT_COINCID_FLAG, 0);
        }
    }
}

// Updates the mode in the STAT register based on the ly and scan clock
static void update_render_mode(GameBoy *gb, const uint8_t ly, const bool lcd_on) {

    const uint8_t curr_mode = SREAD8(STAT) & 0x3;
    PPUMode new_mode = -1;
    bool request_int = false;

    // V-Blank (10 lines)
    if (ly >= 144 || !lcd_on) {
        new_mode = VBlank;
        request_int = RREG(STAT, STAT_VBLANK_INT);
    }
    // Screen rendering (144 lines aka height of screen in px)
    else {

        // OAM Transfer
        if (gb->ppu.scan_clock < 80) {
            new_mode = OamTransfer;
            request_int = RREG(STAT, STAT_OAM_INT);
        }
        // Pixel Transfer
        else if (gb->ppu.scan_clock >= 80 && gb->ppu.scan_clock <= 252) {
            new_mode = PixelTransfer;
        }
        // H-Blank
        else if (gb->ppu.scan_clock > 252) {
            new_mode = HBlank;
            request_int = RREG(STAT, STAT_HBLANK_INT);
        }
    }

    if (new_mode != curr_mode) {
        // We've changed mode and the interrupt for this mode is active
        // So request a LCD STAT interrupt
        if (request_int) {
            WREG(IF, IEF_LCD_STAT, 1);
        }

        WREG(STAT, STAT_MODE1, (new_mode >> 1) & 1);
        WREG(STAT, STAT_MODE0, (new_mode >> 0) & 1);
    }
}

// Get the start of the tile data for background and window tiles
// Returns true if the tile number is a signed integer
static bool get_bg_tile_data_start(GameBoy *gb, uint16_t *start) {

    // When this bit is enabled, the tile data is stored starting at 0x8000
    // The tile numbers are unsigned (0 to 255)
    // When this bit is disabled, the tile data is stored starting at 0x8800 (pattern 0 at 0x9000)
    // The tile numbers are signed (-128 to 127)
    if (RREG(LCDC, LCDC_BG_WINDOW_TILE_DATA)) {
        *start = 0x8000;
        return false;
    } else {
        *start = 0x8800;
        return true;
    }
}

// Get the position of the nearest tile in the map that matches any given screen position
static uint16_t get_tile_map_offset(const Position screen) { return (screen.x / 8) + ((screen.y / 8) * 32); }

// Get the offset of the tile data that matches a map address
static uint16_t get_tile_data_offset(GameBoy *gb, const uint16_t map_addr, const bool signed_tile_num) {

    if (signed_tile_num) {
        const int8_t tile_number = (int8_t)SREAD8(map_addr);
        return ((tile_number + 128) * 16);
    } else {
        const uint8_t tile_number = SREAD8(map_addr);
        return (tile_number * 16);
    }
}

// Fetches two bytes in tile data memory for any given line in a tile
static void get_tile_row_data(GameBoy *gb, const uint8_t line, const uint8_t vram_bank, const uint16_t data_addr,
                              uint8_t *data) {

    assert(vram_bank <= 1);
    const uint8_t tile_y = line * 2;
    const uint8_t *vram = gb->mmu.vram_banks[vram_bank];

    data[0] = vram[data_addr + tile_y + 0 - VRAM_START];
    data[1] = vram[data_addr + tile_y + 1 - VRAM_START];
}

// Get the colour of a pixel in a tile
static uint16_t get_tile_pixel(Position screen_pos, const uint8_t *data, const bool is_flipped_x,
                               const uint16_t *palette) {

    const uint8_t bit = is_flipped_x ? screen_pos.x % 8 : 7 - screen_pos.x % 8;

    const uint8_t colour_num = GET_BIT(data[0], bit) | (GET_BIT(data[1], bit) << 1);
    return palette[colour_num];
}

// Gets the tile attributes for a given tile (CGB only)
static TileAttributes get_tile_attributes(GameBoy *gb, const uint16_t map_addr) {
    const uint8_t data = gb->mmu.vram_banks[1][map_addr - VRAM_START];

    const TileAttributes result = {(data & TILE_ATTR_PALETTE), (data & TILE_ATTR_BANK) >> 3,
                                   (data & TILE_ATTR_FLIP_X) >> 5, (data & TILE_ATTR_FLIP_Y) >> 6,
                                   (data & TILE_ATTR_BG_PRIORITY) >> 7};

    return result;
}

// Plots a single grayscale pixel on the screen at the given coordinate
static void plot_tile_pixel(uint16_t *framebuffer, const Position display_pos, const uint16_t colour) {
    const uint16_t buf_offset = display_pos.x + display_pos.y * SCREEN_WIDTH;
    framebuffer[buf_offset] = colour;
}

static void render_bg_scan(GameBoy *gb, const uint8_t ly) {
    if (!RREG(LCDC, LCDC_BG_DISPLAY)) {
        return;
    }

    const uint16_t map_start = (RREG(LCDC, LCDC_BG_TILE_MAP) ? 0x9C00 : 0x9800);

    uint16_t data_start = 0;
    const bool signed_tile_num = get_bg_tile_data_start(gb, &data_start);

    const uint8_t scroll_x = SREAD8(SCX);
    const uint8_t scroll_y = SREAD8(SCY);

    uint16_t colours[4];
    const uint8_t palette = SREAD8(BGP);
    fill_shade_table(palette, colours);

    // TODO: optimise: only fetch attributes and data once per 8 pixels
    // TODO: merge bg and window code somehow
    for (uint8_t scan_x = 0; scan_x < SCREEN_WIDTH; scan_x++) {

        // Position in screen memory after scrolling
        const Position screen_pos = {scroll_x + scan_x, scroll_y + ly};

        const uint16_t map_offset = get_tile_map_offset(screen_pos);
        const uint16_t map_addr = map_start + map_offset;
        const TileAttributes attributes = get_tile_attributes(gb, map_addr);

        if (gb->cart.is_colour) {
            for (uint8_t i = 0; i < 4; ++i) {
                colours[i] = gb->ppu.bg_palette[attributes.palette * 4 + i];
            }
        }

        const uint16_t data_addr = data_start + get_tile_data_offset(gb, map_addr, signed_tile_num);

        uint8_t data[2];

        const uint8_t line = attributes.is_flipped_y ? 7 - screen_pos.y % 8 : screen_pos.y % 8;

        get_tile_row_data(gb, line, attributes.vram_bank, data_addr, data);

        const uint16_t colour = get_tile_pixel(screen_pos, data, attributes.is_flipped_x, colours);

        // Position on the physical display
        const Position display_pos = {scan_x, ly};

        plot_tile_pixel(gb->ppu.framebuffer, display_pos, colour);
    }
}

static void render_window_scan(GameBoy *gb, const uint8_t ly) {
    if (!RREG(LCDC, LCDC_WINDOW_DISPLAY)) {
        return;
    }

    const uint16_t map_start = (RREG(LCDC, LCDC_WINDOW_TILE_MAP) ? 0x9C00 : 0x9800);

    uint16_t data_start;
    const bool signed_tile_num = get_bg_tile_data_start(gb, &data_start);

    uint16_t colours[4];
    const uint8_t palette = SREAD8(BGP);
    fill_shade_table(palette, colours);

    const uint8_t window_x = SREAD8(WX) - 7;
    const uint8_t window_y = SREAD8(WY);

    for (uint8_t scan_x = 0; scan_x < SCREEN_WIDTH; scan_x++) {
        if (ly < window_y) {
            return;
        }

        if (scan_x < window_x) {
            continue;
        }

        // Position in screen memory after scrolling
        const Position screen_pos = {window_x + scan_x, ly - window_y};

        const uint16_t map_addr = map_start + get_tile_map_offset(screen_pos);
        const uint16_t data_addr = data_start + get_tile_data_offset(gb, map_addr, signed_tile_num);

        uint8_t data[2];
        const uint8_t line = screen_pos.y % 8;
        get_tile_row_data(gb, line, 0, data_addr, data);

        const uint16_t colour = get_tile_pixel(screen_pos, data, false, colours);

        // Position on the physical display
        const Position display_pos = {scan_x, ly};

        plot_tile_pixel(gb->ppu.framebuffer, display_pos, colour);
    }
}

static void render_sprite_scan(GameBoy *gb, const uint8_t ly) {
    if (!RREG(LCDC, LCDC_OBJ_DISPLAY)) {
        return;
    }

    const bool tall_sprites = RREG(LCDC, LCDC_OBJ_SIZE);
    const uint8_t height = tall_sprites ? 16 : 8;
    uint8_t visible_sprite_count = 0;

    for (int i = 0; i < 40; i++) {
        const Sprite sprite = gb->ppu.sprite_buffer[i];
        const int16_t x = sprite.x - 8;
        const int16_t y = sprite.y - 16;

        if ((y > ly || y + height <= ly) || visible_sprite_count == 10) {
            continue;
        }

        visible_sprite_count++;
        const uint16_t palette_addr = (GET_BIT(sprite.attributes, SPRITE_ATTR_PALETTE)) ? OBP1 : OBP0;
        const uint8_t palette = SREAD8(palette_addr);

        uint16_t shades[4];
        fill_shade_table(palette, shades);

        // A tall sprite has the first bit removed
        const uint8_t tile = tall_sprites ? sprite.tile & 0x7F : sprite.tile;
        uint8_t row_index = ly - y;

        if (GET_BIT(sprite.attributes, SPRITE_ATTR_FLIP_Y)) {
            row_index = (height - 1) - row_index;
        }

        const uint16_t row_offset = (tile * 16) + row_index * 2;

        uint16_t data[2];
        data[0] = SREAD16(0x8000 + row_offset + 0);
        data[1] = SREAD16(0x8000 + row_offset + 1);

        const bool is_flipped_x = GET_BIT(sprite.attributes, SPRITE_ATTR_FLIP_X);
        const bool is_behind_bg = GET_BIT(sprite.attributes, SPRITE_ATTR_PRIORITY);

        // Draw the pixels of the sprite, however if the sprite is offscreen
        // then only draw the visible pixels
        for (uint8_t px = (x < 0 ? -x : 0); px < 8; px++) {
            if (x + px > SCREEN_WIDTH) {
                break;
            }

            const uint8_t bit = is_flipped_x ? px : 7 - px;
            const uint8_t shade_num = GET_BIT(data[0], bit) | (GET_BIT(data[1], bit) << 1);

            // White is transparent for sprites
            if (shade_num == 0) {
                continue;
            }

            const uint16_t shade = shades[shade_num];
            const uint32_t buf_offset = (x + px) + (ly * SCREEN_WIDTH);

            // If the sprite is behind the background, it is only visible above white
            if (is_behind_bg && gb->ppu.framebuffer[buf_offset] != WHITE) {
                continue;
            }

            gb->ppu.framebuffer[buf_offset] = shade;
        }
    }
}

// Sorting comparison function for sprite priority
static int sprite_cmp(const void *a, const void *b) { return ((Sprite *)a)->x - ((Sprite *)b)->x; }

// Fills the sprite buffer with sprite structs from memory
void fill_sprite_buffer(GameBoy *gb) {
    for (uint8_t i = 0; i < 40; i++) {
        Sprite *sprite = &gb->ppu.sprite_buffer[i];
        const uint16_t address = 0xFE00 + (i * 4);

        sprite->y = SREAD8(address + 0);
        sprite->x = SREAD8(address + 1);
        sprite->tile = SREAD8(address + 2);
        sprite->attributes = SREAD8(address + 3);
    }

    qsort(gb->ppu.sprite_buffer, 40, sizeof(Sprite), sprite_cmp);
}

// Returns the colour associated with a shade number tiles
// Monochrome GameBoy only
static uint16_t get_shade(const uint8_t num) {
    switch (num) {
    case 0:
        return WHITE;
    case 1:
        return LGREY;
    case 2:
        return DGREY;
    case 3:
        return BLACK;
    default:
        ASSERT_NOT_REACHED();
    }
}

// Modifies a table to add the color shades for bg tiles according to the palette register provided
// Monochrome GameBoy only
static void fill_shade_table(const uint8_t palette, uint16_t *colours) {
    uint8_t i = 0;
    uint8_t j = 0;

    for (; i < 8; i += 2, j++) {
        const uint8_t shade_num = (GET_BIT(palette, i + 1) << 1) | GET_BIT(palette, i);
        colours[j] = get_shade(shade_num);
    }
}

void palette_index_write(GameBoy *gb, const uint16_t address, const uint8_t value) {
    assert(address == BGPI || address == OBPI);

    uint16_t *palette = NULL;
    uint16_t data_reg_addr;

    if (address == BGPI) {
        palette = gb->ppu.bg_palette;
        data_reg_addr = BGPD;
    } else {
        palette = gb->ppu.obj_palette;
        data_reg_addr = OBPD;
    }

    const uint8_t index = value & PI_INDEX;
    const uint16_t colour = palette[index / 2];

    uint8_t data;

    // First byte of colour data
    // Contains all 5 bits of red and the lower 3 bits of green
    if (index % 2 == 0) {
        data = colour & 0xFF;
    // Second byte of colour data
    // Contains all 5 bits of blue and the upper 2 bits of green
    } else {
        data = (colour & 0xFF00) >> 8;
    }

    SWRITE8(data_reg_addr, data);
}

void palette_data_write(GameBoy *gb, const uint16_t address, const uint8_t value) {
    assert(address == BGPD || address == OBPD);

    uint16_t *palette = NULL;
    uint16_t index_reg_addr;

    if (address == BGPD) {
        palette = gb->ppu.bg_palette;
        index_reg_addr = BGPI;
    } else {
        palette = gb->ppu.obj_palette;
        index_reg_addr = OBPI;
    }

    const uint8_t index_reg = SREAD8(index_reg_addr);
    const uint8_t index = index_reg & PI_INDEX;

    uint16_t *colour = &palette[index / 2];

    // First byte of colour data
    if (index % 2 == 0) {
        *colour = (*colour & 0xFF00) | value;
    // Second byte of colour data
    } else {
        *colour = (*colour & 0xFF) | (value << 8);
        *colour &= 0x7FFF; // transparency is never set
    }

    const bool auto_incr = (index_reg & PI_AUTO_INCR) >> 7;

    if (auto_incr) {
        const uint8_t new_index = (index + 1) % PI_INDEX;
        SWRITE8(index_reg_addr, (index_reg & ~PI_INDEX) | new_index);
    }
}
