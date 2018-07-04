#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "ppu.h"

static void update_render_mode(gbc_system *, unsigned char);
static void render_bg_scan(gbc_system *, unsigned char);
static void render_sprite_scan(gbc_system *, const struct Sprite *, unsigned char);

static void render_tile_pixel(gbc_system *, const unsigned char, const unsigned char, const unsigned short, SDL_Colour *);
static struct Sprite *get_sprites(gbc_system *);

static SDL_Colour get_shade(const unsigned char);
static void fill_shade_table(gbc_system *, SDL_Colour *);


// Opens a SDL window
void init_ppu(gbc_ppu *ppu, const char scale) {

    SDL_Init(SDL_INIT_VIDEO);

    ppu->window = SDL_CreateWindow(
        MAIN_WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH * scale,
        SCREEN_HEIGHT * scale,
        SDL_WINDOW_SHOWN
    );

    ppu->renderer = SDL_CreateRenderer(
        ppu->window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    ppu->texture = SDL_CreateTexture(
        ppu->renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
    
    ppu->framebuffer = calloc(SCREEN_WIDTH * SCREEN_HEIGHT * 8, sizeof(char));
    SDL_RenderSetScale(ppu->renderer, scale, scale);
    render(ppu);

    ppu->scan_clock = 0;
    ppu->frame_clock = 0;
}

// Updates the mode in the STAT register based on the ly and scan clock 
static void update_render_mode(gbc_system *gbc, unsigned char ly_val) {

    unsigned char mode = 0;

    // V-Blank (10 lines)
    if(ly_val >= 144 && ly_val <= 154) {
        mode = 1;
    }
    // Screen rendering (144 lines aka height of screen in px)
    else {
    
        // OAM Transfer (20 clocks)
        if(gbc->ppu->scan_clock < 20) {
            mode = 2;
        }
        // Pixel Transfer (43 clocks)
        else if(gbc->ppu->scan_clock >= 20 && gbc->ppu->scan_clock < 63) {
            mode = 3;  
        } 
        // H-Blank (51 clocks) 
        else if(gbc->ppu->scan_clock >= 63 && gbc->ppu->scan_clock <= 114) {
            mode = 0; 
        }
    }

    // Write the mode to the two bits of the register
    write_register(gbc, STAT, STAT_MODE0, (mode >> 1) & 1);
    write_register(gbc, STAT, STAT_MODE1, (mode >> 0) & 1);
}

// Renders the picture on the screen scanline by scanline
void update_ppu(gbc_system *gbc, int clocks) {

    unsigned char ly_val = read_byte(gbc->ram, LY);
    update_render_mode(gbc, ly_val);

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

            /*struct Sprite *sprites = get_sprites(gbc);*/
            /*render_sprite_scan(gbc, sprites, ly_val);*/
            /*free(sprites);*/
        }
        // End of frame, request vblank
        else if(ly_val == 144) {
            write_register(gbc, IF, IEF_VBLANK, 1); 
        }

        // Update the coincidence flag (0: LYC != LY, 1: LYC == LY)
        const bool coincidence = (read_byte(gbc->ram, LYC) == ly_val);
        write_register(gbc, STAT, STAT_COINCID_FLAG, coincidence);

        // If the LYC == LY interrupt is enabled, request it
        if(coincidence && read_register(gbc->ram, STAT, STAT_COINCID_INT)) {
            write_register(gbc, IF, IEF_LCD_STAT, 1); 
        }
    }
}

// Renders a background scanline to the display at the specified coordinates
// Gets tile data from ram at the pointer given
static void render_bg_scan(gbc_system *gbc, unsigned char ly) {

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
    fill_shade_table(gbc, shades);

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
            render_tile_pixel(gbc, x + px, ly, pixel_row, shades);
        }
    }
}

// Renders a sprite scanline to the display at the specified coordinates
static void render_sprite_scan(gbc_system *gbc, const struct Sprite *sprites, const unsigned char ly) {

    // Are the tiles 8x8 or 8*16?
    bool tall_sprites = read_register(gbc->ram, LCDC, LCDC_OBJ_SIZE);
    const unsigned char height = (tall_sprites) ? 16 : 8;

    // Get the palette data
    SDL_Colour shades[4];
    fill_shade_table(gbc, shades);

    for(int i = 0; i < 40; i++) {
        
        struct Sprite sprite = sprites[i];
        const unsigned char x = sprite.x - 8;
        const unsigned char y = sprite.y - 16;

        // If the sprite is on the screen
        if(sprite.x > 0 && sprite.x < 168 && y >= ly && y + height <= ly) {
            /*printf("yeah boi\n");*/

            // A tall sprite has the first bit removed 
            const unsigned char tile = (tall_sprites) ? sprite.tile & 0x7F : sprite.tile;
            const unsigned char line_number = ly - y;
            printf("%d\n", line_number);

            // Get the byte of data that matches this sprite line
            const unsigned short line_offset = (tile * 8 * height * 2) + line_number;
            const unsigned short line_data = read_short(gbc->ram, 0x8000 + line_offset);

            for(unsigned char px = 0; px < 8; px++) {
                render_tile_pixel(gbc, x + px, y, line_data, shades);
            } 
        }
    } 
}

// Renders a tile pixel to the framebuffer 
static void render_tile_pixel(gbc_system *gbc, const unsigned char x, const unsigned char y, const unsigned short line, SDL_Colour *shades) {

    const unsigned char ls_byte = (line & 0xFF00) >> 8;
    const unsigned char ms_byte = (line & 0x00FF);

    // Get the colour from the most the most and least significant bits
    const unsigned char bit = 7 - (x % 8);
    const unsigned char shade_num = ((ms_byte >> bit) & 1) | (((ls_byte >> bit) & 1) << 1);
    SDL_Colour shade = shades[shade_num];

    const unsigned int buf_offset = (x * 4) + (y * SCREEN_WIDTH * 4);

    gbc->ppu->framebuffer[buf_offset + 0] = shade.r; // R
    gbc->ppu->framebuffer[buf_offset + 1] = shade.g; // G
    gbc->ppu->framebuffer[buf_offset + 2] = shade.b; // B
    gbc->ppu->framebuffer[buf_offset + 3] = shade.a; // A
}

// Returns an array of sprites in the OAM 
static struct Sprite *get_sprites(gbc_system *gbc) {

    struct Sprite *sprites = malloc(40 * sizeof(struct Sprite));

    for(int i = 0; i < 40; i++) {
    
        struct Sprite *sprite = &sprites[i];
        unsigned short address = 0xFE00 + (i * 4);

        sprite->x = read_byte(gbc->ram, address + 0); 
        sprite->y = read_byte(gbc->ram, address + 1); 
        sprite->tile = read_byte(gbc->ram, address + 2); 
        sprite->attributes = read_byte(gbc->ram, address + 3);
    }

    return sprites;
}

// Returns the colour associated with a shade number
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

// Modifies a table to add the color shades according to the palette register
// Monochrome GameBoy only
static void fill_shade_table(gbc_system *gbc, SDL_Colour *table) {
    
    // Read the background palette register
    unsigned char palette = read_byte(gbc->ram, BGP);
    
    // Fill the table
    for(unsigned char i = 0; i < 8; i += 2) {

        // Get the number from two bits at a time
        unsigned char shade_num = (((palette >> i) & 1) << 1) | ((palette >> (i + 1)) & 1);

        // Add the shade
        table[i / 2] = get_shade(shade_num); 
    }
}

// Renders the current framebuffer to the display
void render(gbc_ppu *ppu) {

    SDL_UpdateTexture(
        ppu->texture,
        NULL,
        ppu->framebuffer,
        SCREEN_WIDTH * 4 
    );

    SDL_RenderCopy(ppu->renderer, ppu->texture, NULL, NULL);
    SDL_RenderPresent(ppu->renderer);
}
