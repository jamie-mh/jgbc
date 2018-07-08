#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "ppu.h"

static void update_render_mode(gbc_system *, const unsigned char, const bool);
static void render_bg_scan(gbc_system *, const unsigned char);
static void render_sprite_scan(gbc_system *, const unsigned char);

static void render_tile_pixel(gbc_system *, const unsigned char, const unsigned char, const unsigned short, const SDL_Colour *, const bool);
static gbc_sprite *get_sprites(gbc_system *);

static SDL_Colour get_shade(const unsigned char);
static void fill_bg_shade_table(gbc_system *, SDL_Colour *);
static void fill_sprite_shade_table(gbc_system *, const unsigned char, SDL_Colour *);


// Creates an empty framebuffer
void init_ppu(gbc_ppu *ppu) {

    ppu->framebuffer = calloc(SCREEN_WIDTH * SCREEN_HEIGHT * 8, sizeof(char));
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

    ppu->renderer = SDL_CreateRenderer(
        ppu->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    ppu->texture = SDL_CreateTexture(
        ppu->renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    SDL_RenderSetScale(ppu->renderer, SCREEN_SCALE, SCREEN_SCALE);
    render_to_window(ppu);
}

// Updates the mode in the STAT register based on the ly and scan clock 
static void update_render_mode(gbc_system *gbc, const unsigned char ly_val, const bool lcd_on) {
    
    const unsigned char curr_mode = read_byte(gbc->ram, STAT) & 0x3;
    unsigned char new_mode = 0;
    bool request_int = false;

    // V-Blank (10 lines) or when the screen is off
    if(ly_val >= 144 || !lcd_on) {
        new_mode = 1;
        request_int = read_register(gbc->ram, STAT, STAT_VBLANK_INT);
    }
    // Screen rendering (144 lines aka height of screen in px)
    else {
    
        // OAM Transfer
        if(gbc->ppu->scan_clock < 80) {
            new_mode = 2;
            request_int = read_register(gbc->ram, STAT, STAT_OAM_INT);
        }
        // Pixel Transfer
        else if(gbc->ppu->scan_clock >= 80 && gbc->ppu->scan_clock <= 252) {
            new_mode = 3;
        } 
        // H-Blank
        else if(gbc->ppu->scan_clock > 252) {
            new_mode = 0; 
            request_int = read_register(gbc->ram, STAT, STAT_HBLANK_INT);
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

// Renders the picture on the screen scanline by scanline
void update_ppu(gbc_system *gbc, const int clocks) {

    const bool lcd_on = read_register(gbc->ram, LCDC, LCDC_LCD_ENABLE);
    unsigned char ly_val = read_byte(gbc->ram, LY);

    update_render_mode(gbc, ly_val, lcd_on);

    // When the LCD is off, ly and the scanline clock are reset
    if(!lcd_on) {
        write_byte(gbc, LY, 0, false);
        gbc->ppu->scan_clock = 0;
        return;
    }

    if(gbc->ppu->sprite_buffer == NULL) {
        gbc->ppu->sprite_buffer = get_sprites(gbc); 
    }

    gbc->ppu->scan_clock += clocks;

    if(gbc->ppu->scan_clock >= CLOCKS_PER_SCANLINE) { 

        gbc->ppu->scan_clock = 0;

        if(ly_val == 153) {
            ly_val = 0;
        } else {
            ly_val++;
        }
         
        write_byte(gbc, LY, ly_val, false);

        // Render all 144 lines on the screen 
        if(ly_val < 144) {
            render_bg_scan(gbc, ly_val);
            render_sprite_scan(gbc, ly_val);
        }
        // End of frame, request vblank interrupt
        else if(ly_val == 144) {
            write_register(gbc, IF, IEF_VBLANK, 1); 

            free(gbc->ppu->sprite_buffer);
            gbc->ppu->sprite_buffer = get_sprites(gbc);
        }

        // Check if LY == LYC
        if(read_byte(gbc->ram, LYC) == ly_val) {
            write_register(gbc, STAT, STAT_COINCID_FLAG, 1);

            // If the LY == LYC interrupt is enabled, request it
            if(read_register(gbc->ram, STAT, STAT_COINCID_INT)) {
                write_register(gbc, IF, IEF_LCD_STAT, 1);
            }
        } else {
            write_register(gbc, STAT, STAT_COINCID_FLAG, 0);
        }
    }
}

// Renders a background scanline to the display at the specified coordinates
// Gets tile data from ram at the pointer given
static void render_bg_scan(gbc_system *gbc, const unsigned char ly) {

    if(!read_register(gbc->ram, LCDC, LCDC_BG_DISPLAY)) {
        return;
    }

    // Get the start of the tile map data 
    unsigned short bg_tile_map_start = (read_register(gbc->ram, LCDC, LCDC_BG_TILE_MAP) ? 0x9C00 : 0x9800);

    // When this bit is enabled, the tile data is stored starting at 0x8000
    // The tile numbers are unsigned (0 to 255)
    // When this bit is disabled, the tile data is stored starting at 0x8800 (pattern 0 at 0x9000)
    // The tile numbers are signed (-128 to 127)
    bool signed_tile_num;
    unsigned short bg_tile_data_start;

    if(read_register(gbc->ram, LCDC, LCDC_BG_WINDOW_TILE_DATA)) {
        bg_tile_data_start = 0x8000;
        signed_tile_num = false;
    } else {
        bg_tile_data_start = 0x8800;
        signed_tile_num = true;
    }

    // Get the current background scroll position
    const unsigned char scroll_x = read_byte(gbc->ram, SCX);
    const unsigned char scroll_y = read_byte(gbc->ram, SCY);

    // Get the palette data
    SDL_Colour shades[4];
    fill_bg_shade_table(gbc, shades);

    short tile_number;
    unsigned short tile_start;
    
    // Render a scanline of background tiles 
    for(unsigned char x = 0; x < SCREEN_WIDTH; x += 8) {

        // Get the current background position (wrap around if it overflows)
        const unsigned char bg_x = (scroll_x + x > 255) ? (scroll_x + x) - 255 : scroll_x + x; 
        const unsigned char bg_y = (scroll_y + ly > 255) ? (scroll_y + ly) - 255 : scroll_y + ly; 

        // Get the offset of the tile in the memory map, the lowest multiple of 8
        const unsigned short map_offset = floor(bg_x / 8) + (floor(bg_y / 8) * 32); 

        // Read the tile number at the offset position, however it may be signed
        if(signed_tile_num) {
            tile_number = (signed char) read_byte(gbc->ram, bg_tile_map_start + map_offset);
            tile_start = bg_tile_data_start + ((tile_number + 128) * 16);
        } else {
            tile_number = read_byte(gbc->ram, bg_tile_map_start + map_offset); 
            tile_start = bg_tile_data_start + (tile_number * 16);
        }
        
        // Read two bytes from memory at the pixel row location
        const unsigned short pixel_row = read_short(gbc->ram, tile_start + (bg_y % 8) * 2);

        for(unsigned char px = 0; px < 8; px++) {
            render_tile_pixel(gbc, x + px, ly, pixel_row, shades, false);
        }
    }
}

// Renders a sprite scanline to the display at the specified coordinates
static void render_sprite_scan(gbc_system *gbc, const unsigned char ly) {

    if(!read_register(gbc->ram, LCDC, LCDC_OBJ_DISPLAY)) {
        return;
    }

    // Are the tiles 8x8 or 8x16?
    bool tall_sprites = read_register(gbc->ram, LCDC, LCDC_OBJ_SIZE);
    const unsigned char height = (tall_sprites) ? 16 : 8;

    for(int i = 0; i < 40; i++) {
        
        gbc_sprite sprite = gbc->ppu->sprite_buffer[i];
        const unsigned char x = sprite.x - 8;
        const unsigned char y = sprite.y - 16;

        // If the sprite is on the screen and is on the ly line
        if(x > 0 && x < SCREEN_WIDTH + 8 && y <= ly && y + height > ly) {

            SDL_Colour shades[4];
            fill_sprite_shade_table(gbc, sprite.attributes, shades);

            // A tall sprite has the first bit removed 
            const unsigned char tile = (tall_sprites) ? sprite.tile & 0x7F : sprite.tile;
            const unsigned char row_index = ly - y;

            // Get the byte of data that matches this sprite line
            const unsigned short row_offset = (tile * 16) + row_index * 2;
            const unsigned short row_data = read_short(gbc->ram, 0x8000 + row_offset);

            // TODO: Implement sprite flipping

            for(unsigned char px = 0; px < 8; px++) {
                render_tile_pixel(gbc, x + px, ly, row_data, shades, true);
            } 
        }
    } 
}

// Renders a tile pixel to the framebuffer 
static void render_tile_pixel(gbc_system *gbc, const unsigned char x, const unsigned char y, const unsigned short line, const SDL_Colour *shades, const bool sprite) {

    const unsigned char ls_byte = (line & 0xFF00) >> 8;
    const unsigned char ms_byte = (line & 0x00FF);

    // Get the colour from the most the most and least significant bits
    const unsigned char bit = 7 - (x % 8);
    const unsigned char shade_num = ((ms_byte >> bit) & 1) | (((ls_byte >> bit) & 1) << 1);

    // If the color is white for sprites, ignore it, it should be transparent
    if(!sprite || (sprite && shade_num > 0)) {
        SDL_Colour shade = shades[shade_num];

        const unsigned int buf_offset = (x * 4) + (y * SCREEN_WIDTH * 4);

        gbc->ppu->framebuffer[buf_offset + 0] = shade.r; // R
        gbc->ppu->framebuffer[buf_offset + 1] = shade.g; // G
        gbc->ppu->framebuffer[buf_offset + 2] = shade.b; // B
        gbc->ppu->framebuffer[buf_offset + 3] = shade.a; // A
    }
}

// Returns an array of sprites in the OAM 
static gbc_sprite *get_sprites(gbc_system *gbc) {

    gbc_sprite *sprites = malloc(40 * sizeof(gbc_sprite));

    for(int i = 0; i < 40; i++) {
    
        gbc_sprite *sprite = &sprites[i];
        unsigned short address = 0xFE00 + (i * 4);

        sprite->y = read_byte(gbc->ram, address + 0); 
        sprite->x = read_byte(gbc->ram, address + 1); 
        sprite->tile = read_byte(gbc->ram, address + 2); 
        sprite->attributes = read_byte(gbc->ram, address + 3);
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
        default:
        case 3: 
            return black;
    } 
}

// Modifies a table to add the color shades for bg tiles according to the palette register
// Monochrome GameBoy only
static void fill_bg_shade_table(gbc_system *gbc, SDL_Colour *table) {
    
    // Read the background palette register
    const unsigned char palette = read_byte(gbc->ram, BGP);
    
    // Fill the table
    for(unsigned char i = 0; i < 8; i += 2) {

        // Get the number from two bits at a time
        unsigned char shade_num = (((palette >> i) & 1) << 1) | ((palette >> (i + 1)) & 1);

        // Add the shade
        table[i / 2] = get_shade(shade_num); 
    }
}

// Modifies a table to add the color shades for sprite tiles according to the palette registers
// Monochrome GameBoy only
static void fill_sprite_shade_table(gbc_system *gbc, const unsigned char sprite_attr, SDL_Colour *table) {

    // Read the sprite palette register
    const unsigned char palette_addr = (GET_BIT(sprite_attr, 4)) ? 0xFF49 : 0xFF48;
    const unsigned char palette = read_byte(gbc->ram, palette_addr);
    
    // Fill the table
    for(unsigned char i = 0; i < 8; i += 2) {

        // Get the number from two bits at a time
        unsigned char shade_num = (((palette >> i) & 1) << 1) | ((palette >> (i + 1)) & 1);

        // Add the shade
        table[i / 2] = get_shade(shade_num); 
    }
}

// Renders the current framebuffer to the display
void render_to_window(gbc_ppu *ppu) {

    SDL_UpdateTexture(
        ppu->texture,
        NULL,
        ppu->framebuffer,
        SCREEN_WIDTH * 4 
    );

    SDL_RenderCopy(ppu->renderer, ppu->texture, NULL, NULL);
    SDL_RenderPresent(ppu->renderer);
}
