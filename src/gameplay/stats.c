#include <genesis.h>
#include "gameplay/stats.h"
#include "entities/player.h"

void statsInit() {
    player.health = 100;
    player.maxHealth = 100;
    player.stamina = 100;
    player.maxStamina = 100;
}

bool takeDamage(u16 damage) {
    if (player.health > damage) {
        player.health -= damage;
        return TRUE; // Still alive
    } else {
        player.health = 0;
        setPlayerState(PLAYER_STATE_DEAD);
        return FALSE; // Dead
    }
}

void healDamage(u16 amount) {
    player.health += amount;
    if (player.health > player.maxHealth) {
        player.health = player.maxHealth;
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
