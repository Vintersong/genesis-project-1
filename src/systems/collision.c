#include <genesis.h>
#include "systems/collision.h"
#include "core/config.h"
#include "world/tilemap.h"

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

bool checkGroundCollision(fix32 posX, fix32 posY) {
    // posX/posY are the sprite's top-left; the feet sit at posY + hitbox size.
    // Probe the pixel row just below the feet at both bottom corners (inset 1px
    // so touching a wall doesn't count as ground).
    fix32 feetY = posY + FIX32(PLAYER_HITBOX_SIZE);
    fix32 leftX = posX + FIX32(1);
    fix32 rightX = posX + FIX32(PLAYER_HITBOX_SIZE - 2);

    return tilemapIsSolidAtPixel(leftX, feetY) || tilemapIsSolidAtPixel(rightX, feetY);
}
