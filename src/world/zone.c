#include <genesis.h>
#include "world/zone.h"
#include "core/config.h"

// Global current zone
Zone currentZone;

void zoneInit() {
    // Start in HUB zone
    currentZone.zoneID = ZONE_HUB;
    currentZone.subZoneCount = 1;
    currentZone.currentSubZone = 0;
    currentZone.discovered = TRUE;
    currentZone.completed = FALSE;
}

void loadZone(u8 zoneID) {
    // Unload current zone first
    unloadZone();
    
    // Load new zone
    currentZone.zoneID = zoneID;
    currentZone.discovered = TRUE;
    
    // Set zone-specific properties
    switch (zoneID) {
        case ZONE_CPU:
            currentZone.subZoneCount = 3;
            break;
        case ZONE_GPU:
            currentZone.subZoneCount = 3;
            break;
        case ZONE_RAM:
            currentZone.subZoneCount = 2;
            break;
        case ZONE_STORAGE:
            currentZone.subZoneCount = 4;
            break;
        case ZONE_HUB:
            currentZone.subZoneCount = 1;
            break;
        case ZONE_BIOS:
            currentZone.subZoneCount = 2;
            break;
        case ZONE_RESERVED:
            currentZone.subZoneCount = 1;
            break;
        default:
            currentZone.subZoneCount = 1;
            break;
    }
    
    currentZone.currentSubZone = 0;
    
    // TODO: Load zone tilemap, sprites, enemies, etc.
}

void unloadZone() {
    // TODO: Unload current zone assets
    // Free sprites, clear tilemap, etc.
}

u8 getCurrentZone() {
    return currentZone.zoneID;
}

void markZoneDiscovered() {
    currentZone.discovered = TRUE;
}

void markZoneCompleted() {
    currentZone.completed = TRUE;
}
