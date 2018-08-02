#include "jgbc.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"

static void update_render_mode(GameBoy *, const uint8_t, const bool);

static void render_bg_scan(GameBoy *, const uint8_t);
static void render_sprite_scan(GameBoy *, const uint8_t);

static Colour get_shade(const uint8_t);
static void fill_shade_table(const uint8_t, Colour *);


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
        SCREEN_WIDTH * SCREEN_SCALE,
        SCREEN_HEIGHT * SCREEN_SCALE,
        SDL_WINDOW_SHOWN
    );

    gb->ppu.renderer = SDL_CreateRenderer(
        gb->ppu.window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

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

    SDL_RenderSetScale(gb->ppu.renderer, SCREEN_SCALE, SCREEN_SCALE);
}

void render(GameBoy *gb) {

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

    const bool lcd_on = read_register(gb, LCDC, LCDC_LCD_ENABLE);
    uint8_t ly_val = read_byte(gb, LY, false);

    update_render_mode(gb, ly_val, lcd_on);

    if(!lcd_on) {
        write_byte(gb, LY, 0, false);
        gb->ppu.scan_clock = 0;
        return;
    }

    gb->ppu.scan_clock += gb->cpu.ticks;

    if(gb->ppu.scan_clock >= CLOCKS_PER_SCANLINE) { 

        gb->ppu.scan_clock = 0;

        ly_val = (ly_val == 153) ? 0 : ly_val + 1;
        write_byte(gb, LY, ly_val, false);

        // Render scanlines (144 pixel tall screen)
        if(ly_val < 144) {
            render_bg_scan(gb, ly_val);
            render_sprite_scan(gb, ly_val);
        }
        // End of frame, request vblank interrupt
        else if(ly_val == 144) {
            write_register(gb, IF, IEF_VBLANK, 1); 

            if(gb->ppu.window != NULL) {
                render(gb);
            }
        }

        // Check if LY == LYC
        // And request an interrupt
        if(read_byte(gb, LYC, false) == ly_val) {
            write_register(gb, STAT, STAT_COINCID_FLAG, 1);

            // If the LY == LYC interrupt is enabled, request it
            if(read_register(gb, STAT, STAT_COINCID_INT)) {
                write_register(gb, IF, IEF_LCD_STAT, 1);
            }

        } else {
            write_register(gb, STAT, STAT_COINCID_FLAG, 0);
        }
    }
}

// Updates the mode in the STAT register based on the ly and scan clock 
static void update_render_mode(GameBoy *gb, const uint8_t ly_val, const bool lcd_on) {
    
    const uint8_t curr_mode = read_byte(gb, STAT, false) & 0x3;
    uint8_t new_mode = 0;
    bool request_int = false;

    // V-Blank (10 lines) or when the screen is off
    if(ly_val >= 144 || !lcd_on) {
        new_mode = 1;
        request_int = read_register(gb, STAT, STAT_VBLANK_INT);
    }
    // Screen rendering (144 lines aka height of screen in px)
    else {
    
        // OAM Transfer
        if(gb->ppu.scan_clock < 80) {
            new_mode = 2;
            request_int = read_register(gb, STAT, STAT_OAM_INT);
        }
        // Pixel Transfer
        else if(gb->ppu.scan_clock >= 80 && gb->ppu.scan_clock <= 252) {
            new_mode = 3;
        } 
        // H-Blank
        else if(gb->ppu.scan_clock > 252) {
            new_mode = 0; 
            request_int = read_register(gb, STAT, STAT_HBLANK_INT);
        }
    }

    if(new_mode != curr_mode) {

        // We've changed mode and the interrupt for this mode is active
        // So request a LCD STAT interrupt
        if(request_int) {
            write_register(gb, IF, IEF_LCD_STAT, 1);
        }

        write_register(gb, STAT, STAT_MODE1, (new_mode >> 1) & 1);
        write_register(gb, STAT, STAT_MODE0, (new_mode >> 0) & 1);
    }
}

static void render_bg_scan(GameBoy *gb, const uint8_t ly) {

    if(!read_register(gb, LCDC, LCDC_BG_DISPLAY)) {
        return;
    } 

    const uint16_t bg_tile_map_start = (read_register(gb, LCDC, LCDC_BG_TILE_MAP) ? 0x9C00 : 0x9800);

    // When this bit is enabled, the tile data is stored starting at 0x8000
    // The tile numbers are unsigned (0 to 255)
    // When this bit is disabled, the tile data is stored starting at 0x8800 (pattern 0 at 0x9000)
    // The tile numbers are signed (-128 to 127)
    bool signed_tile_num;
    uint16_t bg_tile_data_start;

    if(read_register(gb, LCDC, LCDC_BG_WINDOW_TILE_DATA)) {
        bg_tile_data_start = 0x8000;
        signed_tile_num = false;
    } else {
        bg_tile_data_start = 0x8800;
        signed_tile_num = true;
    }

    const bool window_enabled = read_register(gb, LCDC, LCDC_WINDOW_DISPLAY);
    const uint16_t window_tile_map_start = (read_register(gb, LCDC, LCDC_WINDOW_TILE_MAP) ? 0x9C00 : 0x9800);
        
    const uint8_t scroll_x = read_byte(gb, SCX, false);
    const uint8_t scroll_y = read_byte(gb, SCY, false);

    Colour shades[4];
    const uint8_t palette = read_byte(gb, BGP, false);
    fill_shade_table(palette, shades);

    uint16_t tile_map;
    uint8_t pos_x;
    uint8_t pos_y;

    for(uint8_t x = 0; x < SCREEN_WIDTH; x++) {

        pos_x = scroll_x + x;
        pos_y = scroll_y + ly; 
        tile_map = bg_tile_map_start; 

        if(window_enabled) {
            const uint8_t window_x = read_byte(gb, WX, false) - 7;
            const uint8_t window_y = read_byte(gb, WY, false);

            if(ly >= window_y && x >= window_x) {
                tile_map = window_tile_map_start;
                pos_x = window_x + x;
                pos_y = ly - window_y;
            }
        } 

        const uint16_t tile_col = pos_x / 8;
        const uint16_t tile_row = (pos_y / 8) * 32;

        const uint8_t tile_y = (pos_y % 8) * 2;

        const uint16_t tile_addr_map = tile_map + tile_col + tile_row;
        int16_t tile_number;
        uint16_t tile_addr_data;

        if(signed_tile_num) {
            tile_number = (int8_t) read_byte(gb, tile_addr_map, false);
            tile_addr_data = bg_tile_data_start + ((tile_number + 128) * 16);
        } else {
            tile_number = read_byte(gb, tile_addr_map, false);
            tile_addr_data = bg_tile_data_start + (tile_number * 16);
        }

        const uint8_t data_byte_1 = read_byte(gb, tile_addr_data + tile_y + 0, false);
        const uint8_t data_byte_2 = read_byte(gb, tile_addr_data + tile_y + 1, false);

        uint8_t bit = 7 - (pos_x % 8);

        const uint8_t shade_num = GET_BIT(data_byte_1, bit) | (GET_BIT(data_byte_2, bit) << 1);
        const Colour shade = shades[shade_num];

        const uint32_t buf_offset = (x * 3) + (ly * SCREEN_WIDTH * 3);

        gb->ppu.framebuffer[buf_offset + 0] = shade.r; // R
        gb->ppu.framebuffer[buf_offset + 1] = shade.g; // G
        gb->ppu.framebuffer[buf_offset + 2] = shade.b; // B
    }
}

static void render_sprite_scan(GameBoy *gb, const uint8_t ly) {

    if(!read_register(gb, LCDC, LCDC_OBJ_DISPLAY)) {
        return;
    }

    const bool tall_sprites = read_register(gb, LCDC, LCDC_OBJ_SIZE);
    const uint8_t height = (tall_sprites) ? 16 : 8;

    for(int i = 0; i < 40; i++) {
        
        const Sprite sprite = gb->ppu.sprite_buffer[i];
        const int16_t x = sprite.x - 8;
        const int16_t y = sprite.y - 16;

        if(y <= ly && y + height > ly) {

            const uint16_t palette_addr = (GET_BIT(sprite.attributes, SPRITE_ATTR_PALETTE)) ? OBP1 : OBP0;
            const uint8_t palette = read_byte(gb, palette_addr, false);

            Colour shades[4];
            fill_shade_table(palette, shades);

            // A tall sprite has the first bit removed 
            const uint8_t tile = (tall_sprites) ? sprite.tile & 0x7F : sprite.tile;
            uint8_t row_index = ly - y;

            if(GET_BIT(sprite.attributes, SPRITE_ATTR_FLIP_Y)) {
                row_index = (height - 1) - row_index; 
            }

            const uint16_t row_offset = (tile * 16) + row_index * 2;
            const uint16_t data_byte_1 = read_short(gb, 0x8000 + row_offset + 0, false);
            const uint16_t data_byte_2 = read_short(gb, 0x8000 + row_offset + 1, false);

            const bool is_flipped_x = GET_BIT(sprite.attributes, SPRITE_ATTR_FLIP_X);
            const bool is_behind_bg = GET_BIT(sprite.attributes, SPRITE_ATTR_PRIORITY);

            // Draw the pixels of the sprite, however if the sprite is offscreen
            // then only draw the visible pixels
            for(uint8_t px = ((x < 0) ? -x : 0); px < 8; px++) {

                if(x + px > SCREEN_WIDTH) {
                    break; 
                }

                const uint8_t bit = (is_flipped_x) ? px : 7 - px;

                uint8_t shade_num = GET_BIT(data_byte_1, bit) | (GET_BIT(data_byte_2, bit) << 1);

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

int sprite_cmp(const void *a, const void *b) {
    return ((Sprite *) b)->x - ((Sprite *) a)->x;
}

void get_sprites(GameBoy *gb) {

    for(uint8_t i = 0; i < 40; i++) {
    
        Sprite *sprite = &gb->ppu.sprite_buffer[i];
        uint16_t address = 0xFE00 + (i * 4);

        sprite->y = read_byte(gb, address + 0, false); 
        sprite->x = read_byte(gb, address + 1, false); 
        sprite->tile = read_byte(gb, address + 2, false); 
        sprite->attributes = read_byte(gb, address + 3, false);
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