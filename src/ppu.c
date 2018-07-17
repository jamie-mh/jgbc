#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "ppu.h"

static void update_render_mode(gbc_system *, const unsigned char, const bool);

static void render_bg_scan(gbc_system *, const unsigned char);
static void render_sprite_scan(gbc_system *, const unsigned char);

static gbc_sprite *get_sprites(gbc_system *);

static SDL_Colour get_shade(const unsigned char);
static void fill_shade_table(const unsigned char, SDL_Colour *);


// Creates an empty framebuffer
void init_ppu(gbc_ppu *ppu) {
    ppu->window = NULL;
    ppu->renderer = NULL;
    ppu->texture = NULL;

    ppu->framebuffer = calloc(SCREEN_WIDTH * SCREEN_HEIGHT * 3, sizeof(char));
    ppu->sprite_buffer = NULL;

    ppu->scan_clock = 0;
    ppu->frame_clock = 0;
}

// Creates a SDL window for use without the debugger
void init_window(gbc_ppu *ppu) {

    SDL_Init(SDL_INIT_VIDEO);

    ppu->window = SDL_CreateWindow(
        MAIN_WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH * SCREEN_SCALE,
        SCREEN_HEIGHT * SCREEN_SCALE,
        SDL_WINDOW_SHOWN
    );

    SDL_DisplayMode mode;
    mode.refresh_rate = FRAMERATE;
    SDL_SetWindowDisplayMode(ppu->window, &mode);

    ppu->renderer = SDL_CreateRenderer(
        ppu->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    ppu->texture = SDL_CreateTexture(
        ppu->renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    SDL_RenderSetScale(ppu->renderer, SCREEN_SCALE, SCREEN_SCALE);
    render_to_window(ppu);
}

// Renders the picture on the screen scanline by scanline
void update_ppu(gbc_system *gbc) {

    const bool lcd_on = read_register(gbc, LCDC, LCDC_LCD_ENABLE);
    unsigned char ly_val = read_byte(gbc, LY, false);

    update_render_mode(gbc, ly_val, lcd_on);

    // When the LCD is off, ly and the scanline clock is reset
    if(!lcd_on) {
        write_byte(gbc, LY, 0, false);
        gbc->ppu->scan_clock = 0;
        return;
    }

    if(gbc->ppu->sprite_buffer == NULL) {
        gbc->ppu->sprite_buffer = get_sprites(gbc); 
    }

    gbc->ppu->scan_clock += gbc->clocks;

    if(gbc->ppu->scan_clock >= CLOCKS_PER_SCANLINE) { 

        gbc->ppu->scan_clock = 0;

        ly_val = (ly_val == 153) ? 0 : ly_val + 1;
        write_byte(gbc, LY, ly_val, false);

        // Render scanlines (144 pixel tall screen)
        if(ly_val < 144) {
            render_bg_scan(gbc, ly_val);
            render_sprite_scan(gbc, ly_val);
        }
        // End of frame, request vblank interrupt
        else if(ly_val == 144) {
            write_register(gbc, IF, IEF_VBLANK, 1); 

            free(gbc->ppu->sprite_buffer);
            gbc->ppu->sprite_buffer = get_sprites(gbc);

            if(gbc->ppu->window != NULL) {
                render_to_window(gbc->ppu);
            }
        }

        // Check if LY == LYC
        // And request an interrupt
        if(read_byte(gbc, LYC, false) == ly_val) {
            write_register(gbc, STAT, STAT_COINCID_FLAG, 1);

            // If the LY == LYC interrupt is enabled, request it
            if(read_register(gbc, STAT, STAT_COINCID_INT)) {
                write_register(gbc, IF, IEF_LCD_STAT, 1);
            }

        } else {
            write_register(gbc, STAT, STAT_COINCID_FLAG, 0);
        }
    }
}

// Updates the mode in the STAT register based on the ly and scan clock 
static void update_render_mode(gbc_system *gbc, const unsigned char ly_val, const bool lcd_on) {
    
    const unsigned char curr_mode = read_byte(gbc, STAT, false) & 0x3;
    unsigned char new_mode = 0;
    bool request_int = false;

    // V-Blank (10 lines) or when the screen is off
    if(ly_val >= 144 || !lcd_on) {
        new_mode = 1;
        request_int = read_register(gbc, STAT, STAT_VBLANK_INT);
    }
    // Screen rendering (144 lines aka height of screen in px)
    else {
    
        // OAM Transfer
        if(gbc->ppu->scan_clock < 80) {
            new_mode = 2;
            request_int = read_register(gbc, STAT, STAT_OAM_INT);
        }
        // Pixel Transfer
        else if(gbc->ppu->scan_clock >= 80 && gbc->ppu->scan_clock <= 252) {
            new_mode = 3;
        } 
        // H-Blank
        else if(gbc->ppu->scan_clock > 252) {
            new_mode = 0; 
            request_int = read_register(gbc, STAT, STAT_HBLANK_INT);
        }
    }

    if(new_mode != curr_mode) {

        // We've changed mode and the interrupt for this mode is active
        // So request a LCD STAT interrupt
        if(request_int) {
            write_register(gbc, IF, IEF_LCD_STAT, 1);
        }

        // Write the mode to the two bits of the register
        write_register(gbc, STAT, STAT_MODE1, (new_mode >> 1) & 1);
        write_register(gbc, STAT, STAT_MODE0, (new_mode >> 0) & 1);
    }
}

// Renders a background scanline to the display at the specified coordinates
// Gets tile data from ram at the pointer given
static void render_bg_scan(gbc_system *gbc, const unsigned char ly) {

    if(!read_register(gbc, LCDC, LCDC_BG_DISPLAY)) {
        return;
    } 

    // Get the start of the tile map data 
    const unsigned short bg_tile_map_start = (read_register(gbc, LCDC, LCDC_BG_TILE_MAP) ? 0x9C00 : 0x9800);

    // When this bit is enabled, the tile data is stored starting at 0x8000
    // The tile numbers are unsigned (0 to 255)
    // When this bit is disabled, the tile data is stored starting at 0x8800 (pattern 0 at 0x9000)
    // The tile numbers are signed (-128 to 127)
    bool signed_tile_num;
    unsigned short bg_tile_data_start;

    if(read_register(gbc, LCDC, LCDC_BG_WINDOW_TILE_DATA)) {
        bg_tile_data_start = 0x8000;
        signed_tile_num = false;
    } else {
        bg_tile_data_start = 0x8800;
        signed_tile_num = true;
    }

    const bool window_enabled = read_register(gbc, LCDC, LCDC_WINDOW_DISPLAY);
    const unsigned short window_tile_map_start = (read_register(gbc, LCDC, LCDC_WINDOW_TILE_MAP) ? 0x9C00 : 0x9800);
        
    // Get the current background scroll position
    const unsigned char scroll_x = read_byte(gbc, SCX, false);
    const unsigned char scroll_y = read_byte(gbc, SCY, false);

    SDL_Colour shades[4];
    const unsigned char palette = read_byte(gbc, BGP, false);
    fill_shade_table(palette, shades);

    unsigned short tile_map;
    unsigned char pos_x;
    unsigned char pos_y;

    for(unsigned char x = 0; x < SCREEN_WIDTH; x++) {

        pos_x = scroll_x + x;
        pos_y = scroll_y + ly; 
        tile_map = bg_tile_map_start; 

        if(window_enabled) {
            const unsigned char window_x = read_byte(gbc, WX, false) - 7;
            const unsigned char window_y = read_byte(gbc, WY, false);

            if(ly >= window_y && x >= window_x) {
                tile_map = window_tile_map_start;
                pos_x = window_x + x;
                pos_y = ly - window_y;
            }
        } 

        // Get the current position of the tile in the map 
        const unsigned short tile_col = pos_x / 8;
        const unsigned short tile_row = (pos_y / 8) * 32;

        // Get the pixel position inside the tile
        const unsigned char tile_y = (pos_y % 8) * 2;

        const unsigned short tile_addr_map = tile_map + tile_col + tile_row;
        signed short tile_number;
        unsigned short tile_addr_data;

        if(signed_tile_num) {
            tile_number = (signed char) read_byte(gbc, tile_addr_map, false);
            tile_addr_data = bg_tile_data_start + ((tile_number + 128) * 16);
        } else {
            tile_number = read_byte(gbc, tile_addr_map, false);
            tile_addr_data = bg_tile_data_start + (tile_number * 16);
        }

        // Read the row of data (2 bytes)
        const unsigned char data_byte_1 = read_byte(gbc, tile_addr_data + tile_y + 0, false);
        const unsigned char data_byte_2 = read_byte(gbc, tile_addr_data + tile_y + 1, false);

        unsigned char bit = 7 - (pos_x % 8);

        const unsigned char shade_num = GET_BIT(data_byte_1, bit) | (GET_BIT(data_byte_2, bit) << 1);
        const SDL_Colour shade = shades[shade_num];

        const unsigned int buf_offset = (x * 3) + (ly * SCREEN_WIDTH * 3);

        gbc->ppu->framebuffer[buf_offset + 0] = shade.r; // R
        gbc->ppu->framebuffer[buf_offset + 1] = shade.g; // G
        gbc->ppu->framebuffer[buf_offset + 2] = shade.b; // B
    }
}

// Renders a sprite scanline to the display at the specified coordinates
static void render_sprite_scan(gbc_system *gbc, const unsigned char ly) {

    if(!read_register(gbc, LCDC, LCDC_OBJ_DISPLAY)) {
        return;
    }

    // Are the tiles 8x8 or 8x16?
    const bool tall_sprites = read_register(gbc, LCDC, LCDC_OBJ_SIZE);
    const unsigned char height = (tall_sprites) ? 16 : 8;

    for(int i = 0; i < 40; i++) {
        
        const gbc_sprite sprite = gbc->ppu->sprite_buffer[i];
        const short x = sprite.x - 8;
        const short y = sprite.y - 16;

        // If the sprite is on the screen and is on the ly line
        if(y <= ly && y + height > ly) {

            const unsigned short palette_addr = (GET_BIT(sprite.attributes, SPRITE_ATTR_PALETTE)) ? OBP1 : OBP0;
            const unsigned char palette = read_byte(gbc, palette_addr, false);

            SDL_Colour shades[4];
            fill_shade_table(palette, shades);

            // A tall sprite has the first bit removed 
            const unsigned char tile = (tall_sprites) ? sprite.tile & 0x7F : sprite.tile;
            unsigned char row_index = ly - y;

            if(GET_BIT(sprite.attributes, SPRITE_ATTR_FLIP_Y)) {
                row_index = (height - 1) - row_index; 
            }

            // Get the data that matches this sprite line
            const unsigned short row_offset = (tile * 16) + row_index * 2;
            const unsigned short data_byte_1 = read_short(gbc, 0x8000 + row_offset + 0, false);
            const unsigned short data_byte_2 = read_short(gbc, 0x8000 + row_offset + 1, false);

            const bool is_flipped_x = GET_BIT(sprite.attributes, SPRITE_ATTR_FLIP_X);
            const bool is_behind_bg = GET_BIT(sprite.attributes, SPRITE_ATTR_PRIORITY);

            // Draw the pixels of the sprite, however if the sprite is offscreen
            // then only draw the visible pixels
            for(unsigned char px = ((x < 0) ? -x : 0); px < 8; px++) {

                if(x + px > SCREEN_WIDTH) {
                    break; 
                }

                const unsigned char bit = (is_flipped_x) ? px : 7 - px;

                // Get the colour from the most the most and least significant bits
                unsigned char shade_num = GET_BIT(data_byte_1, bit) | (GET_BIT(data_byte_2, bit) << 1);

                // White is transparent for sprites
                if(shade_num == 0) {
                    continue; 
                }
                
                const SDL_Colour shade = shades[shade_num];
                const unsigned int buf_offset = ((x + px) * 3) + (ly * SCREEN_WIDTH * 3);

                // If the sprite is behind the background, it is only visible above white
                if(is_behind_bg && gbc->ppu->framebuffer[buf_offset] != 0xE0) {
                    continue; 
                }

                gbc->ppu->framebuffer[buf_offset + 0] = shade.r; // R
                gbc->ppu->framebuffer[buf_offset + 1] = shade.g; // G
                gbc->ppu->framebuffer[buf_offset + 2] = shade.b; // B
            } 
        }
    } 
}

// Returns an array of sprites in the OAM 
static gbc_sprite *get_sprites(gbc_system *gbc) {

    gbc_sprite *sprites = malloc(40 * sizeof(gbc_sprite));

    for(unsigned char i = 0; i < 40; i++) {
    
        gbc_sprite *sprite = &sprites[i];
        unsigned short address = 0xFE00 + (i * 4);

        sprite->y = read_byte(gbc, address + 0, false); 
        sprite->x = read_byte(gbc, address + 1, false); 
        sprite->tile = read_byte(gbc, address + 2, false); 
        sprite->attributes = read_byte(gbc, address + 3, false);
    }

    return sprites;
}

// Returns the colour associated with a shade number tiles
// Monochrome GameBoy only
static SDL_Colour get_shade(const unsigned char num) {

    static const SDL_Colour white = {WHITE};
    static const SDL_Colour black = {BLACK};
    static const SDL_Colour lgrey = {LGREY};
    static const SDL_Colour dgrey = {DGREY};

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
static void fill_shade_table(const unsigned char palette, SDL_Colour *table) {
    unsigned char i = 0;
    unsigned char j = 0;

    for(; i < 8; i += 2, j++) {
        const unsigned char shade_num = (GET_BIT(palette, i + 1) << 1) | GET_BIT(palette, i);
        table[j] = get_shade(shade_num); 
    }
}

// Renders the current framebuffer to the display
void render_to_window(gbc_ppu *ppu) {

    SDL_UpdateTexture(
        ppu->texture,
        NULL,
        ppu->framebuffer,
        SCREEN_WIDTH * 3 
    );

    SDL_RenderCopy(ppu->renderer, ppu->texture, NULL, NULL);
    SDL_RenderPresent(ppu->renderer);
}
