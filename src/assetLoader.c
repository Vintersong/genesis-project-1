#include <genesis.h>
#include <resources.h>
#include "entities/player.h"

void loadPlayerAssets();
void loadLevelAssets();
void updateBackgroundScroll();
void initializeAssets();

//Level Design Assets
u16 ind = TILE_USER_INDEX;
int scrollBackground_offset = 0;
int scrollForeground_offset = 0;
Sprite *playerSprite;


void initializeAssets()
{
    SPR_init();
    loadPlayerAssets();
    loadLevelAssets();
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
}

void loadPlayerAssets()
{
    PAL_setPalette(PAL2, pSprite.palette->data, DMA);
    playerSprite = SPR_addSprite(
                        &pSprite,
                        160,  // Just a reasonable default
                        180,  // playerInit() will override this anyway
                    TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
}

void loadLevelAssets()
{
    // Background B - Use VDP_drawImageEx for Image resources
    PAL_setPalette(PAL0, background.palette->data, DMA);
    VDP_drawImageEx(BG_B,
                    &background,
                    TILE_ATTR_FULL(PAL0,
                        FALSE,
                        FALSE,
                        FALSE,
                        ind),
                    0, 0,
                    FALSE,
                    TRUE); // <- This extra argument is for a bitmap, not for a tilemap
    ind += background.tileset->numTile;
    
    // Background A - Your original code was correct here
    PAL_setPalette(PAL1, foreground.palette->data, DMA);
    VDP_drawImageEx(BG_A,
                    &foreground,
                    TILE_ATTR_FULL(PAL1,
                        FALSE,
                        FALSE,
                        FALSE,
                        ind),
                    0, 0,
                    FALSE,
                    TRUE);
    ind +=foreground.tileset->numTile;
}

void updateBackgroundScroll()
{
    scrollBackground_offset -=1;
    scrollForeground_offset -=2;
    VDP_setHorizontalScroll(BG_A, scrollForeground_offset);
    VDP_setHorizontalScroll(BG_B, scrollBackground_offset);
}