#include <genesis.h>
#include "core/entity.h"
#include "core/config.h"
#include "systems/physics.h"
#include "systems/collision.h"
#include "world/tilemap.h"

void entityApplyPhysics(Entity* e) {
    if (!e) return;

    e->onGround = checkGroundCollision(e->posX, e->posY);

    if (!e->onGround) {
        applyGravity(&e->velY);
    } else if (e->velY > FIX32(0)) {
        e->velY = FIX32(0);
    }

    applyFriction(&e->velX, &e->velY, e->onGround);
    integrateVelocity(&e->posX, &e->posY, e->velX, e->velY);

    // Landing: if we moved down into a solid tile, snap the feet to its top edge.
    // Max fall speed (8px/frame) equals one tile, so we can't tunnel through.
    if (e->velY >= FIX32(0) && checkGroundCollision(e->posX, e->posY)) {
        s16 feetPx = F32_toInt(e->posY) + PLAYER_HITBOX_SIZE;
        s16 tileTop = (feetPx / TILE_DIMENSION) * TILE_DIMENSION;
        e->posY = FIX32(tileTop - PLAYER_HITBOX_SIZE);
        e->velY = FIX32(0);
        e->onGround = TRUE;
    }
}

bool entityTakeDamage(Entity* e, u16 amount) {
    if (!e) return FALSE;

    if (amount >= e->health) {
        e->health = 0;
    } else {
        e->health -= amount;
    }

    return e->health == 0;
}

void entityKeepInBounds(Entity* e) {
    if (!e) return;

    if (e->posX < FIX32(0)) {
        e->posX = FIX32(0);
        e->velX = FIX32(0);
    }
    if (e->posX > FIX32(TILEMAP_WIDTH_TILES * TILE_DIMENSION - PLAYER_HITBOX_SIZE)) {
        e->posX = FIX32(TILEMAP_WIDTH_TILES * TILE_DIMENSION - PLAYER_HITBOX_SIZE);
        e->velX = FIX32(0);
    }
}
