#ifndef PHYSICS_H
#define PHYSICS_H

#include <genesis.h>

/**
 * @brief Apply gravity to an entity's vertical velocity
 * @param velY Pointer to vertical velocity
 */
void applyGravity(fix32* velY);

/**
 * @brief Apply friction to velocity
 * @param velX Pointer to horizontal velocity
 * @param velY Pointer to vertical velocity
 * @param onGround Whether entity is on ground (ground friction is stronger)
 */
void applyFriction(fix32* velX, fix32* velY, bool onGround);

/**
 * @brief Update position based on velocity
 * @param posX Pointer to X position
 * @param posY Pointer to Y position
 * @param velX Horizontal velocity
 * @param velY Vertical velocity
 */
void integrateVelocity(fix32* posX, fix32* posY, fix32 velX, fix32 velY);

/**
 * @brief Clamp velocity to maximum values
 * @param velX Pointer to horizontal velocity
 * @param velY Pointer to vertical velocity
 * @param maxVelX Maximum horizontal velocity
 * @param maxVelY Maximum vertical velocity
 */
void clampVelocity(fix32* velX, fix32* velY, fix32 maxVelX, fix32 maxVelY);

#endif // PHYSICS_H
