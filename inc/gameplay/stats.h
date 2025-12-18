#ifndef STATS_H
#define STATS_H

#include <genesis.h>

/**
 * @brief Initialize player stats to default values
 */
void statsInit();

/**
 * @brief Deal damage to player
 * @param damage Amount of damage to deal
 * @return TRUE if player is still alive, FALSE if dead
 */
bool takeDamage(u16 damage);

/**
 * @brief Heal player
 * @param amount Amount of health to restore
 */
void healDamage(u16 amount);

/**
 * @brief Consume stamina
 * @param amount Amount of stamina to use
 * @return TRUE if had enough stamina, FALSE otherwise
 */
bool useStamina(u16 amount);

/**
 * @brief Regenerate stamina over time
 * Called each frame to slowly restore stamina
 */
void regenStamina();

/**
 * @brief Check if player has enough stamina
 * @param amount Amount to check
 * @return TRUE if has enough, FALSE otherwise
 */
bool hasStamina(u16 amount);

#endif // STATS_H
