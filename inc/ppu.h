#pragma once

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define FRAMERATE 60.0
#define CLOCKS_PER_SCANLINE 456 

#define SCREEN_INITIAL_SCALE 4
#define WINDOW_TITLE "jgbc"

// LCDC: LCD Control Register 
#define LCDC 0xFF40
#define LCDC_LCD_ENABLE 7 // LCD Display Enable
#define LCDC_WINDOW_TILE_MAP 6 // Window Tile Map Display Select
#define LCDC_WINDOW_DISPLAY 5 // Window Display Enable
#define LCDC_BG_WINDOW_TILE_DATA 4 // BG & Window Tile Data Select
#define LCDC_BG_TILE_MAP 3 // BG Tile Map Display Select
#define LCDC_OBJ_SIZE 2 // OBJ (Sprite) Size 
#define LCDC_OBJ_DISPLAY 1 // OBJ (Sprite) Display Enable
#define LCDC_BG_DISPLAY 0 // BG Display

// STAT: LCDC Status Register
#define STAT 0xFF41
#define STAT_COINCID_INT 6 // LYC=LY Coincidence Interrupt
#define STAT_OAM_INT 5 // Mode 2 OAM Interrupt
#define STAT_VBLANK_INT 4 // Mode 1 V-Blank Interrupt
#define STAT_HBLANK_INT 3 // Mode 0 H-Blank Interrupt
#define STAT_COINCID_FLAG 2 // Coincidence Flag
#define STAT_MODE1 1 // Mode Flag 1/2 
#define STAT_MODE0 0 // Mode Flag 2/2

// Sprite Attributes
#define SPRITE_ATTR_PRIORITY 7
#define SPRITE_ATTR_FLIP_Y 6
#define SPRITE_ATTR_FLIP_X 5
#define SPRITE_ATTR_PALETTE 4

// SCY: Scroll Y
#define SCY 0xFF42

// SCX: Scroll X
#define SCX 0xFF43

// LY: LCDC Y-Coordinate
#define LY 0xFF44

// LYC: LY Compare
#define LYC 0xFF45

// Window Position
#define WX 0xFF4B
#define WY 0xFF4A

// BGP: BG Palette Data (NON CGB)
#define BGP 0xFF47

// 0BP0: Object Palette 0 Data (NON CGB)
#define OBP0 0xFF48

// 0BP1: Object Palette 1 Data (NON CGB)
#define OBP1 0xFF49

// Direct Memory Access 
#define DMA 0xFF46

// GameBoy Monochrome Palette
#define WHITE 0xFF, 0xFF, 0xFF
#define BLACK 0x00, 0x00, 0x00 
#define LGREY 0xD3, 0xD3, 0xD3 
#define DGREY 0x80, 0x80, 0x80 

typedef struct Colour {
    uint8_t r, g, b;
}
Colour;

typedef struct Position {
    uint8_t x, y;
}
Position;

void init_ppu(GameBoy *gb);
void init_window(GameBoy *gb);

void render(GameBoy *gb);
void update_ppu(GameBoy *gb);

void get_sprites(GameBoy *gb);
