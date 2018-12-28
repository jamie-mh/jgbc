#include "jgbc.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"

static void update_render_mode(GameBoy *gb, const uint8_t ly, const bool lcd_on);

static inline bool get_bg_tile_data_start(GameBoy *gb, uint16_t *start);
static inline uint16_t get_tile_map_offset(Position screen);
static inline uint16_t get_tile_data_offset(GameBoy *gb, const uint16_t map_addr, const bool signed_tile_num);
static inline void get_tile_row_data(GameBoy *gb, const uint8_t line, const uint16_t data_addr, uint8_t *data);
static inline Colour get_tile_pixel(Position screen_pos, uint8_t *data, Colour *shades);
static inline void plot_tile_pixel(uint8_t *framebuffer, Position display_pos, Colour shade);

static void render_bg_scan(GameBoy *, const uint8_t ly);
static void render_window_scan(GameBoy *gb, const uint8_t ly);
static void render_sprite_scan(GameBoy *gb, const uint8_t ly);

static int sprite_cmp(const void *a, const void *b);
static Colour get_shade(const uint8_t num);
static void fill_shade_table(const uint8_t palette, Colour *table);


void init_ppu(GameBoy *gb) {
    gb->ppu.framebuffer = calloc(SCREEN_WIDTH * SCREEN_HEIGHT * 3, sizeof(uint8_t));
    gb->ppu.scan_clock = 0;
    gb->ppu.frame_clock = 0;

    gb->ppu.window = NULL;
    gb->ppu.renderer = NULL;
    gb->ppu.texture = NULL;
}

void init_window(GameBoy *gb) {

    gb->ppu.window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH * SCREEN_INITIAL_SCALE,
        SCREEN_HEIGHT * SCREEN_INITIAL_SCALE,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    gb->ppu.renderer = SDL_CreateRenderer(
        gb->ppu.window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    SDL_RenderSetLogicalSize(gb->ppu.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_DisplayMode mode;
    mode.refresh_rate = FRAMERATE;
    SDL_SetWindowDisplayMode(gb->ppu.window, &mode);

    gb->ppu.texture = SDL_CreateTexture(
        gb->ppu.renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
}

void render(GameBoy *gb) {

    SDL_SetRenderDrawColor(gb->ppu.renderer, 0, 0, 0, 255);
    SDL_RenderClear(gb->ppu.renderer);

    SDL_UpdateTexture(
        gb->ppu.texture,
        NULL,
        gb->ppu.framebuffer,
        SCREEN_WIDTH * 3
    );

    SDL_RenderCopy(gb->ppu.renderer, gb->ppu.texture, NULL, NULL);
    SDL_RenderPresent(gb->ppu.renderer);
}

void update_ppu(GameBoy *gb) {

    const bool lcd_on = RREG(LCDC, LCDC_LCD_ENABLE);
    uint8_t ly = SREAD8(LY);

    update_render_mode(gb, ly, lcd_on);

    if(!lcd_on) {
        SWRITE8(LY, 0);
        gb->ppu.scan_clock = 0;
        return;
    }

    gb->ppu.scan_clock += gb->cpu.ticks;

    if(gb->ppu.scan_clock >= CLOCKS_PER_SCANLINE) { 

        gb->ppu.scan_clock = 0;
        ly = (ly == 153) ? 0 : ly + 1;
        SWRITE8(LY, ly);

        // Render scanlines (144 pixel tall screen)
        if(ly < 144) {
            render_bg_scan(gb, ly);
            render_window_scan(gb, ly);
            render_sprite_scan(gb, ly);
        }
        // End of frame, request vblank interrupt
        else if(ly == 144) {
            WREG(IF, IEF_VBLANK, 1);

            if(gb->ppu.window != NULL) {
                render(gb);
            }
        }

        // Check if LY == LYC
        // And request an interrupt
        if(read_byte(gb, LYC, false) == ly) {
            WREG(STAT, STAT_COINCID_FLAG, 1);

            // If the LY == LYC interrupt is enabled, request it
            if(RREG(STAT, STAT_COINCID_INT)) {
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
    uint8_t new_mode = 0;
    bool request_int = false;

    // V-Blank (10 lines)
    if(ly >= 144 || !lcd_on) {
        new_mode = 1;
        request_int = RREG(STAT, STAT_VBLANK_INT);
    }
    // Screen rendering (144 lines aka height of screen in px)
    else {
    
        // OAM Transfer
        if(gb->ppu.scan_clock < 80) {
            new_mode = 2;
            request_int = RREG(STAT, STAT_OAM_INT);
        }
        // Pixel Transfer
        else if(gb->ppu.scan_clock >= 80 && gb->ppu.scan_clock <= 252) {
            new_mode = 3;
        } 
        // H-Blank
        else if(gb->ppu.scan_clock > 252) {
            new_mode = 0; 
            request_int = RREG(STAT, STAT_HBLANK_INT);
        }
    }

    if(new_mode != curr_mode) {

        // We've changed mode and the interrupt for this mode is active
        // So request a LCD STAT interrupt
        if(request_int) {
            WREG(IF, IEF_LCD_STAT, 1);
        }

        WREG(STAT, STAT_MODE1, (new_mode >> 1) & 1);
        WREG(STAT, STAT_MODE0, (new_mode >> 0) & 1);
    }
}

// Get the start of the tile data for background and window tiles
// Returns true if the tile number is a signed integer
static inline bool get_bg_tile_data_start(GameBoy *gb, uint16_t *start) {

    // When this bit is enabled, the tile data is stored starting at 0x8000
    // The tile numbers are unsigned (0 to 255)
    // When this bit is disabled, the tile data is stored starting at 0x8800 (pattern 0 at 0x9000)
    // The tile numbers are signed (-128 to 127)
    if(RREG(LCDC, LCDC_BG_WINDOW_TILE_DATA)) {
        *start = 0x8000;
        return false;
    } else {
        *start = 0x8800;
        return true;
    }
}

// Get the position of the nearest tile in the map that matches any given screen position
static inline uint16_t get_tile_map_offset(Position screen) {
    return (screen.x / 8) + ((screen.y / 8) * 32);
}

// Get the offset of the tile data that matches a map address
static inline uint16_t get_tile_data_offset(GameBoy *gb, const uint16_t map_addr, 
        const bool signed_tile_num) {

    if(signed_tile_num) {
        const int8_t tile_number = (int8_t) SREAD8(map_addr);
        return ((tile_number + 128) * 16);
    } else {
        const uint8_t tile_number = SREAD8(map_addr);
        return (tile_number * 16);
    }
}

// Fetches two bytes in tile data memory for any given line in a tile
static inline void get_tile_row_data(GameBoy *gb, const uint8_t line, 
        const uint16_t data_addr, uint8_t *data) {
    
    const uint8_t tile_y = line * 2;
    data[0] = SREAD8(data_addr + tile_y + 0);
    data[1] = SREAD8(data_addr + tile_y + 1);
}

// Get the colour of a pixel in a tile
static inline Colour get_tile_pixel(Position screen_pos, uint8_t *data, Colour *shades) {

    uint8_t bit = 7 - (screen_pos.x % 8);
    
    const uint8_t shade_num = GET_BIT(data[0], bit) | (GET_BIT(data[1], bit) << 1);
    return shades[shade_num];
}

// Plots a single grayscale pixel on the screen at the given coordinate
static inline void plot_tile_pixel(uint8_t *framebuffer, Position display_pos, Colour shade) {

    const uint32_t buf_offset = (display_pos.x * 3) + (display_pos.y * SCREEN_WIDTH * 3);

    framebuffer[buf_offset + 0] = shade.r; // R
    framebuffer[buf_offset + 1] = shade.g; // G
    framebuffer[buf_offset + 2] = shade.b; // B
}

static void render_bg_scan(GameBoy *gb, const uint8_t ly) {

    if(!RREG(LCDC, LCDC_BG_DISPLAY)) {
        return;
    } 

    const uint16_t map_start = (RREG(LCDC, LCDC_BG_TILE_MAP) ? 0x9C00 : 0x9800);

    uint16_t data_start;
    bool signed_tile_num = get_bg_tile_data_start(gb, &data_start);

    const uint8_t scroll_x = SREAD8(SCX);
    const uint8_t scroll_y = SREAD8(SCY);

    Colour shades[4];
    const uint8_t palette = SREAD8(BGP);
    fill_shade_table(palette, shades);

    for(uint8_t scan_x = 0; scan_x < SCREEN_WIDTH; scan_x++) {

        // Position in screen memory after scrolling
        const Position screen_pos = { 
            scroll_x + scan_x, 
            scroll_y + ly 
        };

        const uint16_t map_addr = map_start + get_tile_map_offset(screen_pos); 
        const uint16_t data_addr = data_start + get_tile_data_offset(gb, map_addr, signed_tile_num);

        uint8_t data[2];
        const uint8_t line = screen_pos.y % 8;
        get_tile_row_data(gb, line, data_addr, data);

        Colour shade = get_tile_pixel(screen_pos, data, shades);

        // Position on the physical display
        const Position display_pos = { scan_x, ly };

        plot_tile_pixel(gb->ppu.framebuffer, display_pos, shade);
    }
}

static void render_window_scan(GameBoy *gb, const uint8_t ly) {

    if(!RREG(LCDC, LCDC_WINDOW_DISPLAY)) {
        return; 
    }

    const uint16_t map_start = (RREG(LCDC, LCDC_WINDOW_TILE_MAP) ? 0x9C00 : 0x9800);

    uint16_t data_start;
    bool signed_tile_num = get_bg_tile_data_start(gb, &data_start);

    Colour shades[4];
    const uint8_t palette = SREAD8(BGP);
    fill_shade_table(palette, shades);

    const uint8_t window_x = SREAD8(WX) - 7;
    const uint8_t window_y = SREAD8(WY);

    for(uint8_t scan_x = 0; scan_x < SCREEN_WIDTH; scan_x++) {

        if(ly < window_y) {
            return; 
        }

        if(scan_x < window_x) {
            continue; 
        }

        // Position in screen memory after scrolling
        const Position screen_pos = { 
            window_x + scan_x,
            ly - window_y
        };

        const uint16_t map_addr = map_start + get_tile_map_offset(screen_pos); 
        const uint16_t data_addr = data_start + get_tile_data_offset(gb, map_addr, signed_tile_num);

        uint8_t data[2];
        const uint8_t line = screen_pos.y % 8;
        get_tile_row_data(gb, line, data_addr, data);

        Colour shade = get_tile_pixel(screen_pos, data, shades);

        // Position on the physical display
        const Position display_pos = { scan_x, ly };

        plot_tile_pixel(gb->ppu.framebuffer, display_pos, shade);
    }
}

static void render_sprite_scan(GameBoy *gb, const uint8_t ly) {

    if(!RREG(LCDC, LCDC_OBJ_DISPLAY)) {
        return;
    }

    const bool tall_sprites = RREG(LCDC, LCDC_OBJ_SIZE);
    const uint8_t height = (tall_sprites) ? 16 : 8;

    for(int i = 0; i < 40; i++) {
        
        const Sprite sprite = gb->ppu.sprite_buffer[i];
        const int16_t x = sprite.x - 8;
        const int16_t y = sprite.y - 16;

        if(y <= ly && y + height > ly) {

            const uint16_t palette_addr = (GET_BIT(sprite.attributes, SPRITE_ATTR_PALETTE)) ? OBP1 : OBP0;
            const uint8_t palette = SREAD8(palette_addr);

            Colour shades[4];
            fill_shade_table(palette, shades);

            // A tall sprite has the first bit removed 
            const uint8_t tile = (tall_sprites) ? sprite.tile & 0x7F : sprite.tile;
            uint8_t row_index = ly - y;

            if(GET_BIT(sprite.attributes, SPRITE_ATTR_FLIP_Y)) {
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
            for(uint8_t px = ((x < 0) ? -x : 0); px < 8; px++) {

                if(x + px > SCREEN_WIDTH) {
                    break;
                }

                const uint8_t bit = (is_flipped_x) ? px : 7 - px;

                uint8_t shade_num = GET_BIT(data[0], bit) | (GET_BIT(data[1], bit) << 1);

                // White is transparent for sprites
                if(shade_num == 0) {
                    continue; 
                }
                
                const Colour shade = shades[shade_num];
                const uint32_t buf_offset = ((x + px) * 3) + (ly * SCREEN_WIDTH * 3);
                static const Colour white = {WHITE};

                // If the sprite is behind the background, it is only visible above white
                if(is_behind_bg && gb->ppu.framebuffer[buf_offset] != white.r) {
                    continue; 
                }

                gb->ppu.framebuffer[buf_offset + 0] = shade.r; // R
                gb->ppu.framebuffer[buf_offset + 1] = shade.g; // G
                gb->ppu.framebuffer[buf_offset + 2] = shade.b; // B
            } 
        }
    } 
}

// Sorting comparison function for sprite priority
static int sprite_cmp(const void *a, const void *b) {
    return ((Sprite *) b)->x - ((Sprite *) a)->x;
}

// Fills the sprite buffer with sprite structs from memory
void get_sprites(GameBoy *gb) {

    for(uint8_t i = 0; i < 40; i++) {
    
        Sprite *sprite = &gb->ppu.sprite_buffer[i];
        uint16_t address = 0xFE00 + (i * 4);

        sprite->y = SREAD8(address + 0); 
        sprite->x = SREAD8(address + 1); 
        sprite->tile = SREAD8(address + 2); 
        sprite->attributes = SREAD8(address + 3);
    }

    qsort(gb->ppu.sprite_buffer, 40, sizeof(Sprite), sprite_cmp);
}

// Returns the colour associated with a shade number tiles
// Monochrome GameBoy only
static Colour get_shade(const uint8_t num) {

    static const Colour white = {WHITE};
    static const Colour black = {BLACK};
    static const Colour lgrey = {LGREY};
    static const Colour dgrey = {DGREY};

    switch(num) {
        case 0: 
            return white;
        case 1:
            return lgrey;
        case 2: 
            return dgrey;
        case 3: 
            return black;
    } 

    assert(num <= 3);
    return black;
}

// Modifies a table to add the color shades for bg tiles according to the palette register provided
// Monochrome GameBoy only
static void fill_shade_table(const uint8_t palette, Colour *table) {
    uint8_t i = 0;
    uint8_t j = 0;

    for(; i < 8; i += 2, j++) {
        const uint8_t shade_num = (GET_BIT(palette, i + 1) << 1) | GET_BIT(palette, i);
        table[j] = get_shade(shade_num); 
    }
}
