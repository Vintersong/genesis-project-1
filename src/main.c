#include <genesis.h>
#include "core/game.h"
#include "core/config.h"
#include "camera.h"
#include "assetLoader.h"
#include "core/entity.h"
#include "entities/player.h"
#include "entities/enemy.h"
#include "entities/boss.h"

int main() {
    // Initialize all game systems
    gameInit();
    
    // Main game loop
    while (1) {
        // Update game state
        gameUpdate();
        
        // Update camera
        mainCamera();

        // Place sprites using this frame's final camera position
        entitySyncSprite(&player.base);
        entitySyncSprite(&enemy.base);
        entitySyncSprite(&boss.base);
        
        // Update background scrolling
        updateBackgroundScroll();
        
        // Update all sprites
        SPR_update();
        
        // Wait for VBlank and process
        SYS_doVBlankProcess();
    }
    
    return 0;
}
