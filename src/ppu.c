#include "lxgbc.h"
#include "ram.h"
#include "cpu.h"
#include "ppu.h"

// Opens a SDL window, WIP
void init_ppu(gbc_ppu *ppu, const char scale) {

    // Init video subsystem
    SDL_Init(SDL_INIT_VIDEO);

    // Create window
    SDL_CreateWindowAndRenderer(
        SCREEN_WIDTH * scale,
        SCREEN_HEIGHT * scale,
        0,
        &ppu->window,
        &ppu->renderer                            
    );

    // Set the rendering scale
    SDL_RenderSetScale(ppu->renderer, scale, scale);

    // Set the screen to white
    SDL_SetRenderDrawColor(ppu->renderer, 255, 255, 255, 255);
    SDL_RenderClear(ppu->renderer);
    
    // Reset the clock
    ppu->clock = 0;
}

// Renders the picture on the screen scanline by scanline
void ppu_do_clock(gbc_system *gbc) {

    // Set the mode according to the clock
    unsigned char ly_val = read_byte(gbc->ram, LY);
    unsigned char mode;

    // V-Blank (10 lines)
    if(ly_val >= 144 && ly_val <= 154) {
        mode = 1;
    
        // Check for v-blank interrupt
        if(ly_val == 144 && read_register(gbc->ram, IE, IEF_VBLANK)) {
            write_register(gbc->ram, IF, IEF_VBLANK, 1); 
        }
    }
    // Screen rendering (144 lines aka height of screen in px)
    else {
    
        // OAM Transfer (20 clocks)
        if(gbc->ppu->clock >= 0 && gbc->ppu->clock < 20) {
            mode = 2;
        }
        // Pixel Transfer (43 clocks)
        else if(gbc->ppu->clock >= 20 && gbc->ppu->clock < 63) {
            mode = 3;  
        } 
        // H-Blank (51 clocks) 
        else if(gbc->ppu->clock >= 63 && gbc->ppu->clock <= 114) {
            mode = 0; 
        }
    }

    // Write the mode to the two bits of the register
    write_register(gbc->ram, STAT, STAT_MODE0, (mode >> 1) & 1);
    write_register(gbc->ram, STAT, STAT_MODE1, (mode >> 0) & 1);
    
    // Render pixels to the screen in screen rendering mode
    // The rendering lasts for 43 clocks, we are going to draw the line and wait
    // It's not super accurate but it's good enough and shouldn't be too noticable
    if(mode == 3) {

        // Render the background scanline
        render_bg_scan(gbc, ly_val);
    }

    // End of scanline
    if(gbc->ppu->clock == 114) {
        gbc->ppu->clock = 0;
    } else {
        gbc->ppu->clock++;
    }

    // Increment the vertical line register
    write_byte(gbc->ram, LY, (ly_val == 153) ? 0 : ly_val + 1);
}

// Renders a tile to the display at the specified coordinates
// Gets tile data from ram at the pointer given
static void render_bg_scan(gbc_system *gbc, unsigned char ly) {

    // Get the start of the tile map data 
    unsigned short bg_tile_map_start = (read_register(gbc->ram, LCDC, LCDC_BG_TILE_MAP) ? 0x9C00 : 0x9800);

    // When this bit is enabled, the tile data is stored starting at 0x8000
    // The tile numbers are unsigned (0 to 255)
    // When this bit is disabled, the tile data is stored starting at 0x8800 (pattern 0 at 0x9000)
    // The tile numbers are signed (-128 to 127)
    unsigned short bg_tile_data_start = (read_register(gbc->ram, LCDC, LCDC_BG_WINDOW_TILE_DATA) ? 0x8000 : 0x9000);

    // Get the current background scroll position
    unsigned char scroll_x = read_byte(gbc->ram, SCX);
    unsigned char scroll_y = read_byte(gbc->ram, SCY);

    // Get the palette data
    SDL_Colour shades[4];
    fill_shade_table(gbc, shades);

    short tile_number;
    unsigned char x;
    unsigned char y;
    
    // Render the tile scans and fill the screen horizontally
    for(unsigned char i = 0; i < SCREEN_WIDTH; i++) {
    
        // Get the offset of the tile in the memory map
        unsigned char map_offset = floor((double) i / 8.0) * floor((double) ly / 8.0);
        
        // Get the address current tile (16 bytes per tile)
        unsigned char tile_start = bg_tile_data_start + (map_offset * 16);

        // Get the row of pixels in the tile
        unsigned char ls_byte = read_byte(gbc->ram, tile_start + (ly * 2));
        unsigned char ms_byte = read_byte(gbc->ram, tile_start + (ly * 2) + 1);
       
        // Get the colour from the most the most and least significant bits
        
        /*unsigned char shade_num = ((ls_byte >> bit) & 1) | (((ms_byte > bit) & 1) << 1);*/
        /*SDL_Colour shade = shades[shade_num];*/
    
        // Draw the pixel on the screen
        /*SDL_SetRenderDrawColor(gbc->ppu->renderer, shade.r, shade.g, shade.b, shade.a);*/
        /*SDL_RenderDrawPoint(gbc->ppu->renderer, i, ly * SCREEN_WIDTH);*/
    }

    SDL_RenderPresent(gbc->ppu->renderer);
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
        unsigned char shade_num = ((palette >> i) & 1) | ((palette >> (i + 1)) & 1);  

        // Add the shade
        table[i] = get_shade(shade_num); 
    }
}
