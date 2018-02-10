#include "lxgbc.h"
#include "ram.h"
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
    
    // Reset the clock
    ppu->clock = 0;
}

// Renders the picture on the screen scanline by scanline
void ppu_do_clock(gbc_system *gbc) {

    // Set the mode according to the clock
    unsigned char ly_val = read_byte(gbc->ram, LY);
    unsigned char mode;

    // V-Blank (10 lines)
    if(ly_val > 144 && ly_val >= 154) {
        mode = 1; 
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
    
    // Get the memory locations of the background tile data and map
    unsigned short bg_tile_data = (read_register(gbc->ram, LCDC, LCDC_BG_WINDOW_TILE_DATA) ? 0x8000 : 0x8800);
    unsigned short bg_tile_map = (read_register(gbc->ram, LCDC, LCDC_BG_TILE_MAP) ? 0x9C00 : 0x9800);

    // Get the current background scroll position
    unsigned char x = read_byte(gbc->ram, SCX);
    unsigned char y = read_byte(gbc->ram, SCY);
    
    // Render the background tiles
    for(; x < 32; x++) {
        for(; y < 32; y++) {

            // Get the tile to render
            unsigned char tile_number = read_byte(gbc->ram, bg_tile_map + (x * y));
            unsigned short tile_pointer = read_byte(gbc->ram, bg_tile_data + tile_number);

            // Render the tile on the screen
            render_tile(gbc, x * 8, y * 8, tile_pointer);
        } 
    }

    // Increment the vertical line register
    // End of scanline
    if(gbc->ppu->clock == 114) {
        gbc->ppu->clock = 0;
    } else {
        write_byte(gbc->ram, LY, (ly_val == 153) ? 0 : ly_val + 1);
    }

    gbc->ppu->clock++;
}

// Renders a tile to the display at the specified coordinates
// Gets tile data from ram at the pointer given
static void render_tile(gbc_system *gbc, unsigned char x, unsigned char y, unsigned short addr) {

    // Get the palette data
    SDL_Colour shades[4];
    fill_shade_table(gbc, shades);

    // Render the rows of the tile
    for(unsigned char row = 0; row < 8; row++) {
       
        // Get the most and least significant bits of each pixel
        unsigned char ls_byte = read_byte(gbc->ram, addr + 2 * row);
        unsigned char ms_byte = read_byte(gbc->ram, addr + 2 * row + 1);

        // Render the pixels
        for(unsigned char i = 0; i < 8; i++) {
            
            // Get the colour from the most the most and least significant bits
            unsigned char shade_num = ((ls_byte >> i) & 1) | (((ms_byte > i) & 1) << 1);
            SDL_Colour shade = shades[shade_num];

            // Change the colour and render the pixel
            SDL_SetRenderDrawColor(gbc->ppu->renderer, shade.r, shade.g, shade.b, shade.a);
            SDL_RenderDrawPoint(gbc->ppu->renderer, x, y);
        }

        SDL_RenderPresent(gbc->ppu->renderer);
    }
}

// Returns the colour associated with a shade number
// Monochrome GameBoy only
static SDL_Colour get_shade(unsigned char num) {

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

// Simulates the renderering of the lcd in debug mode
// so that programs don't hang waiting for a v-blank or h-blank
// TODO: Improve
void simulate_ppu(gbc_system *gbc) {
    
    // Increment the Y coordinate register
    unsigned char y = read_byte(gbc->ram, LY);
    if(y < 153) {
        write_byte(gbc->ram, LY, y + 1);
    } else { write_byte(gbc->ram, LY, 0x0);
    }
}
