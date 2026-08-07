#include <genesis.h>
#include <resources.h>
#include "entities/player.h"
#include "world/tilemap.h"
#include "world/zone.h"

void loadPlayerAssets();
void loadEnemyAssets();
void loadBossAssets();
void loadLevelAssets();
void updateBackgroundScroll();
void initializeAssets();

//Level Design Assets
u16 ind = TILE_USER_INDEX;
u16 levelTileBaseIndex = 0;
int scrollBackground_offset = 0;
Sprite *playerSprite;
Sprite *enemySprite;
Sprite *bossSprite;


void initializeAssets()
{
    SPR_init();
    loadPlayerAssets();
    loadEnemyAssets();
    loadBossAssets();
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

void loadEnemyAssets()
{
    // Placeholder: reuse player sprite art on PAL3 until enemy art exists
    PAL_setPalette(PAL3, pSprite.palette->data, DMA);
    enemySprite = SPR_addSprite(
                        &pSprite,
                        220,  // enemyInit() will override this anyway
                        100,
                    TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
}

void loadBossAssets()
{
    // Placeholder: reuse player sprite art on PAL3 until boss art exists
    // PAL3 already set by loadEnemyAssets() (called first in initializeAssets()); no need to re-set it here.
    bossSprite = SPR_addSprite(
                        &pSprite,
                        280,  // bossInit() will override this anyway
                        100,
                    TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
}

void loadLevelAssets()
{
    // Background B - distant decorative parallax layer (Image resource)
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

    // Background A - solid, collidable level tilemap (camera-locked, see tilemap.c)
    // Base index is captured once: only one zone tilemap is ever VRAM-resident,
    // so every later loadZone() reload must target this same fixed slot, not
    // whatever `ind` has advanced to by the time a zone transition happens.
    levelTileBaseIndex = ind;
    tilemapLoad(getCurrentZone(), levelTileBaseIndex);
    ind += level_tileset.numTile;
}

void updateBackgroundScroll()
{
    scrollBackground_offset -=1;
    VDP_setHorizontalScroll(BG_B, scrollBackground_offset);
}
