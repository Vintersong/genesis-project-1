#include <genesis.h>
#include <resources.h>
#include "world/tilemap.h"
#include "core/config.h"

static Map* currentMap = NULL;
static u8 collisionGrid[TILEMAP_HEIGHT_TILES][TILEMAP_WIDTH_TILES];

// Placeholder ability-test level layout, matching res/level.png tile-for-tile
// until real per-zone level data (e.g. a Tiled export) exists. Column ranges
// are derived from actual player physics (see src/entities/player.c,
// inc/core/config.h): a plain jump apexes ~7-8 tiles up, a double jump
// (retriggered near the first apex) reaches ~15 tiles up, and a dash covers a
// flat 8-tile burst with no gravity. Layout, left to right:
//   - cols  0-11: spawn ground
//   - cols 12-14: basic-jump pit (3 tiles wide, trivially jumpable)
//   - cols 20-24, row 19: floating platform reachable by a single jump (6 tiles up)
//   - cols 33-37, row 13: floating platform requiring a double jump (12 tiles up)
//   - cols 45-52: dash-only pit (8 tiles wide, exceeds comfortable jump range)
//   - cols 53-79: finish ground
// Both pits keep a 1-tile-thick floor at the very bottom row as a shallow
// safety catch (there's no fall-death/respawn wiring yet), so a missed
// jump/dash just drops the player a short way instead of falling forever.
static bool isPitColumn(u16 x) {
    return (x >= 12 && x <= 14) || (x >= 45 && x <= 52);
}

static void buildPlaceholderCollisionGrid(void) {
    u16 x, y;

    for (y = 0; y < TILEMAP_HEIGHT_TILES; y++) {
        for (x = 0; x < TILEMAP_WIDTH_TILES; x++) {
            bool isGroundRow = (y >= TILEMAP_HEIGHT_TILES - 3);
            bool isPitFloorRow = (y == TILEMAP_HEIGHT_TILES - 1);
            bool isPit = isPitColumn(x);

            bool isSolidGround = isPit ? isPitFloorRow : isGroundRow;
            bool isSingleJumpPlatform = (y == 19) && (x >= 20) && (x <= 24);
            bool isDoubleJumpPlatform = (y == 13) && (x >= 33) && (x <= 37);

            collisionGrid[y][x] = (isSolidGround || isSingleJumpPlatform || isDoubleJumpPlatform)
                                    ? TILE_SOLID : TILE_EMPTY;
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
