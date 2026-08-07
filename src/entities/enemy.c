#include <genesis.h>
#include "entities/enemy.h"
#include "assetLoader.h"
#include "core/config.h"
#include "core/entity.h"
#include "systems/physics.h"
#include "systems/collision.h"

Enemy enemy;

void enemyInit(void) {
    enemy.base.posX = FIX32(220);
    enemy.base.posY = FIX32(100);
    enemy.base.velX = FIX32(0);
    enemy.base.velY = FIX32(0);

    enemy.base.health = 30;
    enemy.base.maxHealth = 30;

    enemy.base.currentState = ENEMY_STATE_IDLE;
    enemy.base.onGround = FALSE;
    enemy.base.facingRight = TRUE;

    enemy.base.sprite = enemySprite;
}

void enemyUpdate(void) {
    if (enemy.base.currentState == ENEMY_STATE_DEAD) {
        return;
    }

    entityApplyPhysics(&enemy.base);
    entityKeepInBounds(&enemy.base);

    if (enemy.base.currentState != ENEMY_STATE_HURT) {
        enemy.base.currentState = enemy.base.onGround ? ENEMY_STATE_IDLE : ENEMY_STATE_FALLING;
    }

    if (enemy.base.sprite) {
        s16 spriteX = enemy.base.posX >> 16;
        s16 spriteY = enemy.base.posY >> 16;
        SPR_setPosition(enemy.base.sprite, spriteX, spriteY);
        SPR_setHFlip(enemy.base.sprite, !enemy.base.facingRight);
    }
}

void enemyTakeDamage(u16 amount) {
    bool died = entityTakeDamage(&enemy.base, amount);
    enemy.base.currentState = died ? ENEMY_STATE_DEAD : ENEMY_STATE_HURT;
}
