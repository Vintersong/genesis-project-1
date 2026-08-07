#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#include <genesis.h>

// Declare the global variables that will be used across files.
// Use 'extern' to indicate that they are defined 
extern u16 ind;
extern u16 levelTileBaseIndex;
extern int scrollBackground_offset;
extern Sprite *playerSprite;
extern Sprite *enemySprite;
extern Sprite *bossSprite;
 
// Declare the functions...
void initializeAssets();
void loadPlayerAssets();
void loadEnemyAssets();
void loadBossAssets();
void loadLevelAssets();
void updateBackgroundScroll();

#endif // ASSET_LOADER_H