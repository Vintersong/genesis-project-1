#include <genesis.h>
#include "core/entity.h"
#include "core/config.h"
#include "systems/physics.h"
#include "systems/collision.h"

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
    if (e->posX > FIX32(SCREEN_WIDTH - 16)) {
        e->posX = FIX32(SCREEN_WIDTH - 16);
        e->velX = FIX32(0);
    }
}
