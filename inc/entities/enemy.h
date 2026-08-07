#ifndef ENEMY_H
#define ENEMY_H

#include <genesis.h>
#include "core/config.h"
#include "core/entity.h"

typedef enum {
    ENEMY_STATE_IDLE,
    ENEMY_STATE_RUNNING,
    ENEMY_STATE_JUMPING,
    ENEMY_STATE_FALLING,
    ENEMY_STATE_HURT,
    ENEMY_STATE_DEAD
} EnemyState;

typedef struct {
    Entity base;
} Enemy;

extern Enemy enemy;

/**
 * @brief Initialize enemy at a default position with default stats
 */
void enemyInit(void);

/**
 * @brief Update enemy physics and state each frame
 */
void enemyUpdate(void);

/**
 * @brief Reduce enemy health and transition to HURT/DEAD as appropriate
 * @param amount Damage amount
 */
void enemyTakeDamage(u16 amount);

#endif // ENEMY_H
