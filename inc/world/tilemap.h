#ifndef TILEMAP_H
#define TILEMAP_H

#include <genesis.h>

typedef enum {
    TILE_EMPTY = 0,
    TILE_SOLID = 1
} TileCollisionFlag;

/**
 * @brief Load the tilemap for a zone onto BG_A and rebuild its collision grid
 * @param zoneID Zone whose tilemap should be loaded (currently all zones share
 *        the placeholder level.png until real per-zone level data exists)
 * @param baseTileIndex First free VRAM tile index to load the tileset into
 *        (caller owns tile index bookkeeping, see assetLoader.c's `ind`)
 */
void tilemapLoad(u8 zoneID, u16 baseTileIndex);

/**
 * @brief Release the currently loaded tilemap, if any
 */
void tilemapUnload(void);

/**
 * @brief Scroll BG_A to the given world position (camera-locked, 1:1)
 */
void tilemapScrollTo(s16 x, s16 y);

/**
 * @brief Check whether the tile at a given pixel position is solid
 * @param worldX World X position in pixels (fixed-point)
 * @param worldY World Y position in pixels (fixed-point)
 * @return TRUE if the tile is solid, FALSE if empty or out of bounds
 */
bool tilemapIsSolidAtPixel(fix32 worldX, fix32 worldY);

#endif // TILEMAP_H
