#include <genesis.h>
#include "world/zone.h"
#include "world/tilemap.h"
#include "core/config.h"
#include "assetLoader.h"

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

    // Reuses the fixed VRAM tile slot freed by unloadZone() above, since only
    // one zone's tilemap is ever resident at a time. Deliberately not `ind`
    // here: `ind` keeps advancing as other assets load, so by the time a zone
    // transition happens it no longer points at the level tileset's slot.
    tilemapLoad(zoneID, levelTileBaseIndex);

    // TODO: Load zone-specific sprites, enemies, etc.
}

void unloadZone() {
    tilemapUnload();

    // TODO: Unload zone-specific sprites, enemies, etc.
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
