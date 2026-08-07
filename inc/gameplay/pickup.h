#ifndef PICKUP_H
#define PICKUP_H

#include <genesis.h>
#include "core/config.h"
#include "systems/collision.h"

#define MAX_PICKUPS 8   // array capacity, headroom for future pickups
#define PICKUP_COUNT 7  // number actually placed by pickupInit()

#define HEALTH_UPGRADE_AMOUNT 20
#define STAMINA_UPGRADE_AMOUNT 20

typedef enum {
    PICKUP_DASH_MODULE,       // unlocks Player.hasDashAbility
    PICKUP_DOUBLE_JUMP_BOOTS, // unlocks Player.hasDoubleJumpAbility
    PICKUP_PARRY_CHIP,        // unlocks Player.hasParryAbility
    PICKUP_HEALTH_UPGRADE,    // raises Player.base.maxHealth by HEALTH_UPGRADE_AMOUNT
    PICKUP_STAMINA_UPGRADE    // raises Player.maxStamina by STAMINA_UPGRADE_AMOUNT
} PickupType;

typedef struct {
    fix32 posX, posY;
    u8 zoneID;
    PickupType type;
    bool collected;
    CollisionBox triggerBox;
} Pickup;

extern Pickup pickups[MAX_PICKUPS];

/**
 * @brief Place the fixed set of placeholder pickups across zones.
 * All start uncollected.
 */
void pickupInit(void);

/**
 * @brief Check player overlap against every uncollected pickup in the
 * current zone and auto-collect on touch.
 */
void pickupUpdate(void);

/**
 * @brief Mark a pickup collected and apply its effect to the player
 * (unlock an ability or raise a max stat).
 * @param index Index into the pickups array
 */
void pickupCollect(u8 index);

/**
 * @brief Number of pickups collected so far (for a completion checklist/HUD).
 */
u8 pickupGetCollectedCount(void);

/**
 * @brief Total number of pickups that exist in the game.
 */
u8 pickupGetTotalCount(void);

#endif // PICKUP_H
