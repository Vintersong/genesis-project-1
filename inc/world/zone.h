#ifndef ZONE_H
#define ZONE_H

#include <genesis.h>
#include "core/config.h"

typedef struct {
    u8 zoneID;
    u8 subZoneCount;
    u8 currentSubZone;
    bool discovered;
    bool completed;
} Zone;

extern Zone currentZone;

/**
 * @brief Initialize zone system
 */
void zoneInit();

/**
 * @brief Load a specific zone
 * @param zoneID ID of the zone to load
 */
void loadZone(u8 zoneID);

/**
 * @brief Unload current zone
 */
void unloadZone();

/**
 * @brief Get current zone ID
 * @return Current zone ID
 */
u8 getCurrentZone();

/**
 * @brief Mark current zone as discovered
 */
void markZoneDiscovered();

/**
 * @brief Mark current zone as completed
 */
void markZoneCompleted();

#endif // ZONE_H
