#include <genesis.h>
#include "gameplay/pickup.h"
#include "entities/player.h"
#include "world/zone.h"
#include "systems/collision.h"
#include "core/config.h"

#define PICKUP_TRIGGER_SIZE 16   // proximity box width/height, in pixels

Pickup pickups[MAX_PICKUPS];

static void setPickup(u8 index, PickupType type, u8 zoneID, s16 x, s16 y) {
    pickups[index].type = type;
    pickups[index].zoneID = zoneID;
    pickups[index].posX = FIX32(x);
    pickups[index].posY = FIX32(y);
    pickups[index].collected = FALSE;
    pickups[index].triggerBox.x = x - PICKUP_TRIGGER_SIZE / 2;
    pickups[index].triggerBox.y = y - PICKUP_TRIGGER_SIZE / 2;
    pickups[index].triggerBox.width = PICKUP_TRIGGER_SIZE;
    pickups[index].triggerBox.height = PICKUP_TRIGGER_SIZE;
}

void pickupInit(void) {
    // Placeholder positions until real zone tilemaps/level data exist
    // (loadZone() in world/zone.c is still a TODO for actual level geometry)
    setPickup(0, PICKUP_DASH_MODULE,       ZONE_GPU,     220, 164);
    setPickup(1, PICKUP_DOUBLE_JUMP_BOOTS, ZONE_RAM,     220, 164);
    setPickup(2, PICKUP_PARRY_CHIP,        ZONE_BIOS,    220, 164);
    setPickup(3, PICKUP_HEALTH_UPGRADE,    ZONE_STORAGE, 100, 164);
    setPickup(4, PICKUP_HEALTH_UPGRADE,    ZONE_CPU,     260, 164);
    setPickup(5, PICKUP_STAMINA_UPGRADE,   ZONE_CPU,     100, 164);
    setPickup(6, PICKUP_STAMINA_UPGRADE,   ZONE_RESERVED, 220, 164);
    // Slots PICKUP_COUNT..MAX_PICKUPS-1 are unused headroom for future pickups.
}

void pickupUpdate(void) {
    u8 zoneID = getCurrentZone();

    CollisionBox playerBox;
    playerBox.x = player.base.posX >> 16;
    playerBox.y = player.base.posY >> 16;
    playerBox.width = PLAYER_HITBOX_SIZE;
    playerBox.height = PLAYER_HITBOX_SIZE;

    for (u8 i = 0; i < PICKUP_COUNT; i++) {
        if (pickups[i].collected || pickups[i].zoneID != zoneID) {
            continue;
        }

        if (collisionAABB(&playerBox, &pickups[i].triggerBox)) {
            pickupCollect(i);
        }
    }
}

void pickupCollect(u8 index) {
    pickups[index].collected = TRUE;

    switch (pickups[index].type) {
        case PICKUP_DASH_MODULE:
            player.hasDashAbility = TRUE;
            break;

        case PICKUP_DOUBLE_JUMP_BOOTS:
            player.hasDoubleJumpAbility = TRUE;
            break;

        case PICKUP_PARRY_CHIP:
            player.hasParryAbility = TRUE;
            break;

        case PICKUP_HEALTH_UPGRADE:
            player.base.maxHealth += HEALTH_UPGRADE_AMOUNT;
            player.base.health = player.base.maxHealth;
            break;

        case PICKUP_STAMINA_UPGRADE:
            player.maxStamina += STAMINA_UPGRADE_AMOUNT;
            player.stamina = player.maxStamina;
            break;
    }
}

u8 pickupGetCollectedCount(void) {
    u8 count = 0;
    for (u8 i = 0; i < PICKUP_COUNT; i++) {
        if (pickups[i].collected) {
            count++;
        }
    }
    return count;
}

u8 pickupGetTotalCount(void) {
    return PICKUP_COUNT;
}
