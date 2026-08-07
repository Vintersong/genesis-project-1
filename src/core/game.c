#include <genesis.h>
#include "core/game.h"
#include "core/config.h"
#include "systems/input.h"
#include "entities/player.h"
#include "entities/enemy.h"
#include "entities/boss.h"
#include "assetLoader.h"
#include "world/zone.h"
#include "ui/hud.h"
#include "gameplay/stats.h"
#include "gameplay/checkpoint.h"
#include "gameplay/pickup.h"

// Global game state
GameState currentGameState = GAME_STATE_TITLE;

void gameInit() {
    // Initialize SGDK systems
    SPR_init();
    
    // Initialize input system
    inputInit();
    
    // Initialize zone system
    zoneInit();
    
    // Initialize stats
    statsInit();

    // Initialize checkpoints (depends on zone system for zoneID indexing)
    checkpointInit();

    // Initialize pickups (depends on zone system for zoneID indexing)
    pickupInit();

    // Initialize assets
    initializeAssets();
    
    // Initialize player
    playerInit();

    // Initialize enemy and boss
    enemyInit();
    bossInit();

    // Initialize HUD
    hudInit();
    
    // Start in playing state (skip title for now)
    gameChangeState(GAME_STATE_PLAYING);
}

void gameUpdate() {
    switch (currentGameState) {
        case GAME_STATE_TITLE:
            break;
        case GAME_STATE_MENU:
            // Title screen logic (to be implemented)
            // For now, just transition to playing on button press
            if (JOY_readJoypad(JOY_1) & BUTTON_START) {
                gameChangeState(GAME_STATE_PLAYING);
            }
            break;
            
        case GAME_STATE_PLAYING:
            // Main gameplay update
            inputUpdate();
            playerUpdate();
            enemyUpdate();
            bossUpdate();
            checkpointUpdate();
            pickupUpdate();

            if (player.base.currentState == PLAYER_STATE_DEAD) {
                checkpointRespawnPlayer();
            }

            hudUpdate();
            hudRender();
            // Camera update will be called from main
            break;
            
        case GAME_STATE_PAUSED:
            // Pause menu logic
            if (JOY_readJoypad(JOY_1) & BUTTON_START) {
                gameChangeState(GAME_STATE_PLAYING);
            }
            break;
            
        case GAME_STATE_GAME_OVER:
            // Game over screen logic
            break;
            
        case GAME_STATE_TRANSITION:
            // Zone transition logic
            break;
            
        default:
            break;
    }
}

void gameChangeState(GameState newState) {
    // GameState oldState = currentGameState;  // Reserved for future state transition logic
    currentGameState = newState;
    
    // Handle state entry logic
    switch (newState) {
        case GAME_STATE_TITLE:
            // Reset game
            break;
        case GAME_STATE_MENU:
            // Game menu
            break;            
        case GAME_STATE_PLAYING:
            // Resume or start gameplay
            break;
            
        case GAME_STATE_PAUSED:
            // Pause display
            break;
            
        case GAME_STATE_GAME_OVER:
            // Game over display
            break;
            
        case GAME_STATE_TRANSITION:
            // Zone transition effect
            break;
            
        default:
            break;
    }
}

GameState gameGetState() {
    return currentGameState;
}
