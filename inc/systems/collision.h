#ifndef COLLISION_H
#define COLLISION_H

#include <genesis.h>

/**
 * @brief Axis-Aligned Bounding Box for collision detection
 */
typedef struct {
    s16 x;
    s16 y;
    u16 width;
    u16 height;
} CollisionBox;

/**
 * @brief Check if two collision boxes overlap
 * @param box1 First collision box
 * @param box2 Second collision box
 * @return TRUE if boxes overlap, FALSE otherwise
 */
bool collisionAABB(CollisionBox* box1, CollisionBox* box2);

/**
 * @brief Check if a point is inside a rectangle
 * @param px Point X coordinate
 * @param py Point Y coordinate
 * @param box Collision box to check against
 * @return TRUE if point is inside box, FALSE otherwise
 */
bool collisionPointRect(s16 px, s16 py, CollisionBox* box);

/**
 * @brief Check if player is on ground (simple Y threshold for now)
 * @param playerY Player's Y position
 * @return TRUE if on ground, FALSE otherwise
 */
bool checkGroundCollision(fix32 playerY);

#endif // COLLISION_H
