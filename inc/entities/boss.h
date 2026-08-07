#ifndef BOSS_H
#define BOSS_H

#include <genesis.h>
#include "core/config.h"
#include "core/entity.h"

typedef enum {
    BOSS_STATE_IDLE,
    BOSS_STATE_RUNNING,
    BOSS_STATE_JUMPING,
    BOSS_STATE_FALLING,
    BOSS_STATE_HURT,
    BOSS_STATE_DEAD
} BossState;

typedef struct {
    Entity base;
    u8 currentPhase;   // reserved for future phase logic; always 0 in this plan
    u16 attackTimer;   // reserved for future attack-pattern logic; unused in this plan
} Boss;

extern Boss boss;

/**
 * @brief Initialize boss at a default position with default stats
 */
void bossInit(void);

/**
 * @brief Update boss physics and state each frame
 */
void bossUpdate(void);

/**
 * @brief Reduce boss health and transition to HURT/DEAD as appropriate
 * @param amount Damage amount
 */
void bossTakeDamage(u16 amount);

#endif // BOSS_H
