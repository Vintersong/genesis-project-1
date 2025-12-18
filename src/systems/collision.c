#include <genesis.h>
#include "systems/collision.h"
#include "core/config.h"

// Ground level for simple collision (will be improved with tilemap collision later)
#define GROUND_Y 180

bool collisionAABB(CollisionBox* box1, CollisionBox* box2) {
    if (!box1 || !box2) return FALSE;
    
    return (box1->x < box2->x + box2->width &&
            box1->x + box1->width > box2->x &&
            box1->y < box2->y + box2->height &&
            box1->y + box1->height > box2->y);
}

bool collisionPointRect(s16 px, s16 py, CollisionBox* box) {
    if (!box) return FALSE;
    
    return (px >= box->x && 
            px <= box->x + box->width &&
            py >= box->y && 
            py <= box->y + box->height);
}

bool checkGroundCollision(fix32 playerY) {
    // Simple ground detection - convert fix32 to integer for comparison
    s16 yPos = playerY >> 16; // Shift right by 16 bits for fix32 conversion
    return yPos >= GROUND_Y;
}
