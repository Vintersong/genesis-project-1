#include <genesis.h>
#include "entities/player.h"
#include "assetLoader.h"
#include "core/config.h"
#include "core/entity.h"
#include "systems/physics.h"
#include "systems/collision.h"

// Global player instance
Player player;

void playerInit() {
    // Initialize position (center of screen)
    player.base.posX = FIX32(160);
    player.base.posY = FIX32(100);
    player.base.velX = FIX32(0);
    player.base.velY = FIX32(0);

    // Initialize stats
    player.base.health = 100;
    player.base.maxHealth = 100;
    player.stamina = 100;
    player.maxStamina = 100;

    // Initialize state
    player.base.currentState = PLAYER_STATE_IDLE;
    player.base.onGround = FALSE;
    player.base.facingRight = TRUE;

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
    player.base.sprite = playerSprite;
}

void playerUpdate() {
    // Update onGround unconditionally every frame (not just when not dashing)
    // This prevents onGround from going stale during a dash
    player.base.onGround = checkGroundCollision(player.base.posX, player.base.posY);

    // Apply physics if not dashing
    if (player.base.currentState != PLAYER_STATE_DASHING) {
        // Check landing transition BEFORE entityApplyPhysics() runs friction/integration
        // This reads velocity at its pre-friction value, matching original behavior
        if (player.base.onGround &&
            (player.base.currentState == PLAYER_STATE_JUMPING ||
             player.base.currentState == PLAYER_STATE_FALLING)) {
            s16 velXInt = player.base.velX >> 16; // Convert fix32 to int
            if (velXInt == 0) {
                setPlayerState(PLAYER_STATE_IDLE);
            } else {
                setPlayerState(PLAYER_STATE_RUNNING);
            }
        }

        // Now apply physics (which includes friction and integration)
        entityApplyPhysics(&player.base);
    }

    // Handle state-specific updates
    switch (player.base.currentState) {
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
            if (player.base.velY > FIX32(0) && player.base.currentState == PLAYER_STATE_JUMPING) {
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
    entityKeepInBounds(&player.base);

    // Update sprite position
    if (player.base.sprite) {
        s16 spriteX = player.base.posX >> 16; // Convert fix32 to int
        s16 spriteY = player.base.posY >> 16;
        SPR_setPosition(player.base.sprite, spriteX, spriteY);
        SPR_setHFlip(player.base.sprite, !player.base.facingRight);
    }
}

void setPlayerState(PlayerState newState) {
    // Validate state transitions
    PlayerState oldState = (PlayerState)player.base.currentState;

    // Some states can't be interrupted
    if (oldState == PLAYER_STATE_DASHING && player.dashTimer > 0) {
        return; // Can't interrupt dash
    }

    if (oldState == PLAYER_STATE_HURT) {
        return; // Can't interrupt hurt state
    }

    // Update state
    player.base.currentState = newState;

    // Reset velocity when entering idle
    if (newState == PLAYER_STATE_IDLE) {
        player.base.velX = FIX32(0);
    }
}

void movePlayerRight() {
    // Can't move while dashing or hurt
    if (player.base.currentState == PLAYER_STATE_DASHING ||
        player.base.currentState == PLAYER_STATE_HURT) {
        return;
    }

    // Set velocity
    player.base.velX = PLAYER_WALK_SPEED;
    player.base.facingRight = TRUE;

    // Update state if on ground
    if (player.base.onGround) {
        setPlayerState(PLAYER_STATE_RUNNING);
    }
}

void movePlayerLeft() {
    // Can't move while dashing or hurt
    if (player.base.currentState == PLAYER_STATE_DASHING ||
        player.base.currentState == PLAYER_STATE_HURT) {
        return;
    }

    // Set velocity
    player.base.velX = -PLAYER_WALK_SPEED;
    player.base.facingRight = FALSE;

    // Update state if on ground
    if (player.base.onGround) {
        setPlayerState(PLAYER_STATE_RUNNING);
    }
}

void playerJump() {
    // Can only jump if on ground or have double jump
    if (player.base.onGround) {
        player.base.velY = PLAYER_JUMP_VELOCITY;
        setPlayerState(PLAYER_STATE_JUMPING);
        player.canDoubleJump = player.hasDoubleJumpAbility;
    } else if (player.canDoubleJump && player.hasDoubleJumpAbility) {
        // Double jump
        player.base.velY = PLAYER_JUMP_VELOCITY;
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
    if (player.base.facingRight) {
        player.base.velX = FIX32(8.0);
    } else {
        player.base.velX = FIX32(-8.0);
    }
    player.base.velY = FIX32(0);
}

void playerParry() {
    // Check if has ability
    if (!player.hasParryAbility) {
        return;
    }

    // Can't parry in certain states
    if (player.base.currentState == PLAYER_STATE_DASHING ||
        player.base.currentState == PLAYER_STATE_HURT) {
        return;
    }

    // Enter parry state
    setPlayerState(PLAYER_STATE_PARRYING);
    player.parryWindow = PLAYER_PARRY_WINDOW;
    player.base.velX = FIX32(0);
}

void playerAttack() {
    // Basic attack stub - to be implemented
    // Check stamina
    if (player.stamina < 10) {
        return;
    }

    // Can't attack in certain states
    if (player.base.currentState == PLAYER_STATE_DASHING ||
        player.base.currentState == PLAYER_STATE_PARRYING ||
        player.base.currentState == PLAYER_STATE_HURT) {
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
        if (player.base.onGround && player.base.currentState != PLAYER_STATE_JUMPING) {
            setPlayerState(PLAYER_STATE_IDLE);
        }
        return;
    }

    // Handle horizontal movement
    if (joyValue & BUTTON_RIGHT) {
        movePlayerRight();
        if (player.base.sprite) {
            SPR_setHFlip(player.base.sprite, FALSE);
        }
    } else if (joyValue & BUTTON_LEFT) {
        movePlayerLeft();
        if (player.base.sprite) {
            SPR_setHFlip(player.base.sprite, TRUE);
        }
    } else {
        // No horizontal input - go to idle if on ground and not jumping
        if (player.base.onGround && player.base.currentState != PLAYER_STATE_JUMPING) {
            setPlayerState(PLAYER_STATE_IDLE);
        }
    }
}
