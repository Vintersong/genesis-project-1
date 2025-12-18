#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#include <genesis.h>

// Declare the global variables that will be used across files.
// Use 'extern' to indicate that they are defined 
extern u16 ind;
extern int scrollBackground_offset;
extern int scrollForeground_offset;
extern Sprite *playerSprite;
 
// Declare the functions...
void initializeAssets();
void loadPlayerAssets();
void loadLevelAssets();
void updateBackgroundScroll();

#endif // ASSET_LOADER_H