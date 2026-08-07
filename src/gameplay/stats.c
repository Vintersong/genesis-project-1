#include <genesis.h>
#include "gameplay/stats.h"
#include "entities/player.h"

void statsInit() {
    player.base.health = 100;
    player.base.maxHealth = 100;
    player.stamina = 100;
    player.maxStamina = 100;
}

bool takeDamage(u16 damage) {
    if (player.base.health > damage) {
        player.base.health -= damage;
        return TRUE; // Still alive
    } else {
        player.base.health = 0;
        setPlayerState(PLAYER_STATE_DEAD);
        return FALSE; // Dead
    }
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
