#include <genesis.h>
#include "entities/boss.h"
#include "assetLoader.h"
#include "core/config.h"
#include "core/entity.h"
#include "systems/physics.h"
#include "systems/collision.h"

Boss boss;

void bossInit(void) {
    boss.base.posX = FIX32(280);
    boss.base.posY = FIX32(100);
    boss.base.velX = FIX32(0);
    boss.base.velY = FIX32(0);

    boss.base.health = 200;
    boss.base.maxHealth = 200;

    boss.base.currentState = BOSS_STATE_IDLE;
    boss.base.onGround = FALSE;
    boss.base.facingRight = TRUE;

    boss.currentPhase = 0;
    boss.attackTimer = 0;

    boss.base.sprite = bossSprite;
}

void bossUpdate(void) {
    if (boss.base.currentState == BOSS_STATE_DEAD) {
        return;
    }

    entityApplyPhysics(&boss.base);
    entityKeepInBounds(&boss.base);

    if (boss.base.currentState != BOSS_STATE_HURT) {
        boss.base.currentState = boss.base.onGround ? BOSS_STATE_IDLE : BOSS_STATE_FALLING;
    }

    if (boss.base.sprite) {
        s16 spriteX = boss.base.posX >> 16;
        s16 spriteY = boss.base.posY >> 16;
        SPR_setPosition(boss.base.sprite, spriteX, spriteY);
        SPR_setHFlip(boss.base.sprite, !boss.base.facingRight);
    }
}

void bossTakeDamage(u16 amount) {
    bool died = entityTakeDamage(&boss.base, amount);
    boss.base.currentState = died ? BOSS_STATE_DEAD : BOSS_STATE_HURT;
}
