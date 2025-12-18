#include <genesis.h>
#include "entities/player.h"
#include "assetLoader.h"
#include "core/config.h"
#include "systems/physics.h"
#include "systems/collision.h"

// Global player instance
Player player;

void playerInit() {
    // Initialize position (center of screen)
    player.posX = FIX32(160);
    player.posY = FIX32(100);
    player.velX = FIX32(0);
    player.velY = FIX32(0);
    
    // Initialize stats
    player.health = 100;
    player.maxHealth = 100;
    player.stamina = 100;
    player.maxStamina = 100;
    
    // Initialize state
    player.currentState = PLAYER_STATE_IDLE;
    player.onGround = FALSE;
    player.facingRight = TRUE;
    
    // Initialize abilities (locked at start)
    player.canDash = FALSE;
    player.dashCooldown = 0;
    player.dashTimer = 0;
    player.hasDashAbility = FALSE;
    
    player.canDoubleJump = FALSE;
    player.hasDoubleJumpAbility = FALSE;
    
    player.parryWindow = PLAYER_PARRY_WINDOW;
    player.hasParryAbility = FALSE;
    
    // Sprite reference will be set by asset loader
    player.sprite = playerSprite;
}

void playerUpdate() {
    // Check ground collision
    player.onGround = checkGroundCollision(player.posY);
    
    // Apply physics if not dashing
    if (player.currentState != PLAYER_STATE_DASHING) {
        // Apply gravity if not on ground
        if (!player.onGround) {
            applyGravity(&player.velY);
        } else {
            // On ground - stop falling
            if (player.velY > FIX32(0)) {
                player.velY = FIX32(0);
            }
            
            // If was jumping/falling, return to idle or running
            if (player.currentState == PLAYER_STATE_JUMPING || 
                player.currentState == PLAYER_STATE_FALLING) {
                s16 velXInt = player.velX >> 16; // Convert fix32 to int
                if (velXInt == 0) {
                    setPlayerState(PLAYER_STATE_IDLE);
                } else {
                    setPlayerState(PLAYER_STATE_RUNNING);
                }
            }
        }
        
        // Apply friction
        applyFriction(&player.velX, &player.velY, player.onGround);
        
        // Update position based on velocity
        integrateVelocity(&player.posX, &player.posY, player.velX, player.velY);
    }
    
    // Handle state-specific updates
    switch (player.currentState) {
        case PLAYER_STATE_DASHING:
            // Update dash timer
            if (player.dashTimer > 0) {
                player.dashTimer--;
            } else {
                setPlayerState(PLAYER_STATE_IDLE);
                player.canDash = FALSE;
            }
            break;
            
        case PLAYER_STATE_PARRYING:
            // Update parry window
            if (player.parryWindow > 0) {
                player.parryWindow--;
            } else {
                setPlayerState(PLAYER_STATE_IDLE);
            }
            break;
            
        case PLAYER_STATE_JUMPING:
        case PLAYER_STATE_FALLING:
            // Check if we should transition to falling
            if (player.velY > FIX32(0) && player.currentState == PLAYER_STATE_JUMPING) {
                setPlayerState(PLAYER_STATE_FALLING);
            }
            break;
            
        default:
            break;
    }
    
    // Update dash cooldown
    if (player.dashCooldown > 0) {
        player.dashCooldown--;
    } else {
        player.canDash = TRUE;
    }
    
    // Regenerate stamina slowly
    if (player.stamina < player.maxStamina) {
        player.stamina++;
    }
    
    // Keep player in bounds
    if (player.posX < FIX32(0)) {
        player.posX = FIX32(0);
        player.velX = FIX32(0);
    }
    if (player.posX > FIX32(SCREEN_WIDTH - 16)) {
        player.posX = FIX32(SCREEN_WIDTH - 16);
        player.velX = FIX32(0);
    }
    
    // Update sprite position
    if (player.sprite) {
        s16 spriteX = player.posX >> 16; // Convert fix32 to int
        s16 spriteY = player.posY >> 16;
        SPR_setPosition(player.sprite, spriteX, spriteY);
        SPR_setHFlip(player.sprite, !player.facingRight);
    }
}

void setPlayerState(PlayerState newState) {
    // Validate state transitions
    PlayerState oldState = player.currentState;
    
    // Some states can't be interrupted
    if (oldState == PLAYER_STATE_DASHING && player.dashTimer > 0) {
        return; // Can't interrupt dash
    }
    
    if (oldState == PLAYER_STATE_HURT) {
        return; // Can't interrupt hurt state
    }
    
    // Update state
    player.currentState = newState;
    
    // Reset velocity when entering idle
    if (newState == PLAYER_STATE_IDLE) {
        player.velX = FIX32(0);
    }
}

void movePlayerRight() {
    // Can't move while dashing or hurt
    if (player.currentState == PLAYER_STATE_DASHING || 
        player.currentState == PLAYER_STATE_HURT) {
        return;
    }
    
    // Set velocity
    player.velX = PLAYER_WALK_SPEED;
    player.facingRight = TRUE;
    
    // Update state if on ground
    if (player.onGround) {
        setPlayerState(PLAYER_STATE_RUNNING);
    }
}

void movePlayerLeft() {
    // Can't move while dashing or hurt
    if (player.currentState == PLAYER_STATE_DASHING || 
        player.currentState == PLAYER_STATE_HURT) {
        return;
    }
    
    // Set velocity
    player.velX = -PLAYER_WALK_SPEED;
    player.facingRight = FALSE;
    
    // Update state if on ground
    if (player.onGround) {
        setPlayerState(PLAYER_STATE_RUNNING);
    }
}

void playerJump() {
    // Can only jump if on ground or have double jump
    if (player.onGround) {
        player.velY = PLAYER_JUMP_VELOCITY;
        setPlayerState(PLAYER_STATE_JUMPING);
        player.canDoubleJump = player.hasDoubleJumpAbility;
    } else if (player.canDoubleJump && player.hasDoubleJumpAbility) {
        // Double jump
        player.velY = PLAYER_JUMP_VELOCITY;
        player.canDoubleJump = FALSE;
    }
}

void playerDash() {
    // Check if can dash
    if (!player.hasDashAbility || !player.canDash) {
        return;
    }
    
    // Check stamina
    if (player.stamina < 20) {
        return;
    }
    
    // Consume stamina
    player.stamina -= 20;
    
    // Set dash state
    setPlayerState(PLAYER_STATE_DASHING);
    player.dashTimer = PLAYER_DASH_DURATION;
    player.dashCooldown = 30; // 0.5 second cooldown at 60fps
    
    // Apply dash velocity in facing direction
    if (player.facingRight) {
        player.velX = FIX32(8.0);
    } else {
        player.velX = FIX32(-8.0);
    }
    player.velY = FIX32(0);
}

void playerParry() {
    // Check if has ability
    if (!player.hasParryAbility) {
        return;
    }
    
    // Can't parry in certain states
    if (player.currentState == PLAYER_STATE_DASHING ||
        player.currentState == PLAYER_STATE_HURT) {
        return;
    }
    
    // Enter parry state
    setPlayerState(PLAYER_STATE_PARRYING);
    player.parryWindow = PLAYER_PARRY_WINDOW;
    player.velX = FIX32(0);
}

void playerAttack() {
    // Basic attack stub - to be implemented
    // Check stamina
    if (player.stamina < 10) {
        return;
    }
    
    // Can't attack in certain states
    if (player.currentState == PLAYER_STATE_DASHING ||
        player.currentState == PLAYER_STATE_PARRYING ||
        player.currentState == PLAYER_STATE_HURT) {
        return;
    }
    
    // Consume stamina
    player.stamina -= 10;
    
    // Set attack state (to be implemented with animation)
    setPlayerState(PLAYER_STATE_ATTACKING);
}

// ============================================================================
// Input Handling (merged from playerController)
// ============================================================================

void playerJoyEvent(u16 joy, u16 changed, u16 state) {
    // Only handle input from first controller
    if (joy != JOY_1) return;

    // Check for button presses (changed & state = newly pressed)
    if (changed & state) {
        // Jump button (B)
        if (changed & state & BUTTON_B) {
            playerJump();
        }

        // Dash button (A) - if ability unlocked
        if (changed & state & BUTTON_A) {
            if (player.hasDashAbility) {
                playerDash();
            }
        }

        // Parry button (C) - if ability unlocked
        if (changed & state & BUTTON_C) {
            if (player.hasParryAbility) {
                playerParry();
            }
        }

        // Attack button (X/Y/Z) - for 6-button controller
        if (changed & state & (BUTTON_X | BUTTON_Y | BUTTON_Z)) {
            playerAttack();
        }
    }
}

void playerHandleInput() {
    // Read current joystick state
    u16 joyValue = JOY_readJoypad(JOY_1);

    // If both left and right are pressed, do nothing
    if ((joyValue & BUTTON_RIGHT) && (joyValue & BUTTON_LEFT)) {
        if (player.onGround && player.currentState != PLAYER_STATE_JUMPING) {
            setPlayerState(PLAYER_STATE_IDLE);
        }
        return;
    }

    // Handle horizontal movement
    if (joyValue & BUTTON_RIGHT) {
        movePlayerRight();
        if (player.sprite) {
            SPR_setHFlip(player.sprite, FALSE);
        }
    } else if (joyValue & BUTTON_LEFT) {
        movePlayerLeft();
        if (player.sprite) {
            SPR_setHFlip(player.sprite, TRUE);
        }
    } else {
        // No horizontal input - go to idle if on ground and not jumping
        if (player.onGround && player.currentState != PLAYER_STATE_JUMPING) {
            setPlayerState(PLAYER_STATE_IDLE);
        }
    }
}
