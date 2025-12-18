#include <genesis.h>
#include "systems/physics.h"
#include "core/config.h"

void applyGravity(fix32* velY) {
    if (!velY) return;
    
    // Add gravity to vertical velocity
    *velY = *velY + GRAVITY;
    
    // Clamp to max fall speed
    if (*velY > MAX_FALL_SPEED) {
        *velY = MAX_FALL_SPEED;
    }
}

void applyFriction(fix32* velX, fix32* velY, bool onGround) {
    if (!velX || !velY) return;
    
    // Apply appropriate friction based on ground state
    // Manually multiply fix32 values (simplified approach)
    if (onGround) {
        // Ground friction = 0.85, so multiply by 0.85
        *velX = (*velX * 85) / 100;
    } else {
        // Air friction = 0.95
        *velX = (*velX * 95) / 100;
    }
    
    // Stop completely if velocity is very small (prevent infinite sliding)
    // Check if absolute value is less than a threshold
    if (*velX < FIX32(0.1) && *velX > FIX32(-0.1)) {
        *velX = FIX32(0);
    }
}

void integrateVelocity(fix32* posX, fix32* posY, fix32 velX, fix32 velY) {
    if (!posX || !posY) return;
    
    *posX = *posX + velX;
    *posY = *posY + velY;
}

void clampVelocity(fix32* velX, fix32* velY, fix32 maxVelX, fix32 maxVelY) {
    if (!velX || !velY) return;
    
    // Clamp horizontal velocity
    if (*velX > maxVelX) {
        *velX = maxVelX;
    } else if (*velX < -maxVelX) {
        *velX = -maxVelX;
    }
    
    // Clamp vertical velocity
    if (*velY > maxVelY) {
        *velY = maxVelY;
    } else if (*velY < -maxVelY) {
        *velY = -maxVelY;
    }
}
