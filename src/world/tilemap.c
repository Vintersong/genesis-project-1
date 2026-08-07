#include <genesis.h>
#include <resources.h>
#include "world/tilemap.h"
#include "core/config.h"

static Map* currentMap = NULL;
static u8 collisionGrid[TILEMAP_HEIGHT_TILES][TILEMAP_WIDTH_TILES];

// Placeholder collision layout matching res/level.png until real per-zone level
// data (e.g. a Tiled export) exists: flat ground across the bottom 3 tile rows,
// plus one floating platform for testing mid-air solid tiles.
static void buildPlaceholderCollisionGrid(void) {
    u16 x, y;

    for (y = 0; y < TILEMAP_HEIGHT_TILES; y++) {
        for (x = 0; x < TILEMAP_WIDTH_TILES; x++) {
            bool isGroundRow = (y >= TILEMAP_HEIGHT_TILES - 3);
            bool isTestPlatform = (y == 18) && (x >= 20) && (x < 26);
            collisionGrid[y][x] = (isGroundRow || isTestPlatform) ? TILE_SOLID : TILE_EMPTY;
        }
    }
}

void tilemapLoad(u8 zoneID, u16 baseTileIndex) {
    (void)zoneID; // all zones share the placeholder level map for now

    tilemapUnload();

    PAL_setPalette(PAL1, level_palette.data, DMA);
    VDP_loadTileSet(&level_tileset, baseTileIndex, DMA);
    currentMap = MAP_create(&level_map, BG_A,
                    TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, baseTileIndex));

    buildPlaceholderCollisionGrid();

    MAP_scrollTo(currentMap, 0, 0);
}

void tilemapUnload(void) {
    if (currentMap) {
        MAP_release(currentMap);
        currentMap = NULL;
    }
}

void tilemapScrollTo(s16 x, s16 y) {
    if (!currentMap) return;
    MAP_scrollTo(currentMap, x, y);
}

bool tilemapIsSolidAtPixel(fix32 worldX, fix32 worldY) {
    s16 px = F32_toInt(worldX);
    s16 py = F32_toInt(worldY);

    if (px < 0 || py < 0) return FALSE;

    u16 tileX = px / TILE_DIMENSION;
    u16 tileY = py / TILE_DIMENSION;

    if (tileX >= TILEMAP_WIDTH_TILES || tileY >= TILEMAP_HEIGHT_TILES) return FALSE;

    return collisionGrid[tileY][tileX] != TILE_EMPTY;
}
