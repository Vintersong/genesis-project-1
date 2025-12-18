#ifndef PLAYER_H
#define PLAYER_H

#include <genesis.h>
#include "core/config.h"

typedef enum {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_RUNNING,
    PLAYER_STATE_JUMPING,
    PLAYER_STATE_FALLING,
    PLAYER_STATE_DASHING,
    PLAYER_STATE_ATTACKING,
    PLAYER_STATE_PARRYING,
    PLAYER_STATE_BLOCKING,
    PLAYER_STATE_HURT,
    PLAYER_STATE_DEAD
} PlayerState;

typedef struct {
    // Position and movement (fixed-point for smooth sub-pixel movement)
    fix32 posX;
    fix32 posY;
    fix32 velX;
    fix32 velY;
    
    // Stats
    u16 health;
    u16 maxHealth;
    u16 stamina;
    u16 maxStamina;
    
    // State
    PlayerState currentState;
    bool onGround;
    bool facingRight;
    
    // Abilities
    bool canDash;
    u16 dashCooldown;
    u16 dashTimer;
    bool hasDashAbility;
    
    bool canDoubleJump;
    bool hasDoubleJumpAbility;
    
    u16 parryWindow;
    bool hasParryAbility;
    
    // Sprite reference (SGDK sprite pointer)
    Sprite* sprite;
    
} Player;

extern Player player;

// Core player functions
/**
 * @brief Initialize player at starting position with default stats
 */
void playerInit();

/**
 * @brief Updates player position and state
 * 
 * Called every frame to handle player physics,
 * state transitions, and collision detection.
 */
void playerUpdate();

/**
 * @brief Change player state with validation
 * @param newState The state to transition to
 */
void setPlayerState(PlayerState newState);

// Movement functions
void movePlayerLeft();
void movePlayerRight();
void playerJump();
void playerDash();

// Combat functions
void playerParry();
void playerAttack();

// Input handling functions
/**
 * @brief Handle button press events
 * @param joy Joystick number
 * @param changed Buttons that changed state
 * @param state Current button state
 */
void playerJoyEvent(u16 joy, u16 changed, u16 state);

/**
 * @brief Handle continuous input (D-pad) each frame
 */
void playerHandleInput();

#endif // PLAYER_H