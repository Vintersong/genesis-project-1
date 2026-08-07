#include <genesis.h>
#include "gameplay/stats.h"
#include "entities/player.h"
#include "core/entity.h"

void statsInit() {
    player.base.health = 100;
    player.base.maxHealth = 100;
    player.stamina = 100;
    player.maxStamina = 100;
}

bool takeDamage(u16 damage) {
    bool died = entityTakeDamage(&player.base, damage);
    if (died) {
        setPlayerState(PLAYER_STATE_DEAD);
    }
    return !died;
}

void healDamage(u16 amount) {
    player.base.health += amount;
    if (player.base.health > player.base.maxHealth) {
        player.base.health = player.base.maxHealth;
    }
}

bool useStamina(u16 amount) {
    if (player.stamina >= amount) {
        player.stamina -= amount;
        return TRUE;
    }
    return FALSE;
}

void regenStamina() {
    if (player.stamina < player.maxStamina) {
        // Regenerate 1 stamina per frame (60 stamina per second at 60fps)
        player.stamina++;
    }
}

bool hasStamina(u16 amount) {
    return player.stamina >= amount;
}
