#include <genesis.h>
#include "core/game.h"
#include "core/config.h"
#include "camera.h"
#include "assetLoader.h"

int main() {
    // Initialize all game systems
    gameInit();
    
    // Main game loop
    while (1) {
        // Update game state
        gameUpdate();
        
        // Update camera
        mainCamera();
        
        // Update background scrolling
        updateBackgroundScroll();
        
        // Update all sprites
        SPR_update();
        
        // Wait for VBlank and process
        SYS_doVBlankProcess();
    }
    
    return 0;
}
