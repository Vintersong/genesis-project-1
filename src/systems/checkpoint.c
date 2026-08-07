#include <genesis.h>
#include "gameplay/checkpoint.h"
#include "gameplay/stats.h"
#include "entities/player.h"
#include "world/zone.h"
#include "systems/collision.h"
#include "core/config.h"

#define CHECKPOINT_TRIGGER_SIZE 24   // proximity box width/height, in pixels

Checkpoint checkpoints[NUM_ZONES];

// Zone whose checkpoint is currently the player's respawn point.
// Starts at ZONE_HUB so a fresh game always has somewhere valid to respawn.
static u8 lastActivatedZone = ZONE_HUB;

static void setCheckpointPosition(u8 zoneID, s16 x, s16 y) {
    checkpoints[zoneID].zoneID = zoneID;
    checkpoints[zoneID].posX = FIX32(x);
    checkpoints[zoneID].posY = FIX32(y);
    checkpoints[zoneID].interactZone.x = x - CHECKPOINT_TRIGGER_SIZE / 2;
    checkpoints[zoneID].interactZone.y = y - CHECKPOINT_TRIGGER_SIZE / 2;
    checkpoints[zoneID].interactZone.width = CHECKPOINT_TRIGGER_SIZE;
    checkpoints[zoneID].interactZone.height = CHECKPOINT_TRIGGER_SIZE;
}

void checkpointInit(void) {
    for (u8 i = 0; i < NUM_ZONES; i++) {
        checkpoints[i].activated = FALSE;
    }

    // Placeholder positions until real zone tilemaps/spawn markers exist
    // (loadZone() in world/zone.c is still a TODO for actual level geometry)
    setCheckpointPosition(ZONE_CPU, 40, 164);
    setCheckpointPosition(ZONE_GPU, 40, 164);
    setCheckpointPosition(ZONE_RAM, 40, 164);
    setCheckpointPosition(ZONE_STORAGE, 40, 164);
    setCheckpointPosition(ZONE_HUB, 160, 164);
    setCheckpointPosition(ZONE_BIOS, 40, 164);
    setCheckpointPosition(ZONE_RESERVED, 40, 164);

    // The HUB checkpoint is lit from the start so death before ever resting
    // still has a valid respawn point.
    checkpoints[ZONE_HUB].activated = TRUE;
    lastActivatedZone = ZONE_HUB;
}

void checkpointUpdate(void) {
    u8 zoneID = getCurrentZone();
    Checkpoint* cp = &checkpoints[zoneID];

    CollisionBox playerBox;
    playerBox.x = player.base.posX >> 16;
    playerBox.y = player.base.posY >> 16;
    playerBox.width = PLAYER_HITBOX_SIZE;
    playerBox.height = PLAYER_HITBOX_SIZE;

    bool inRange = collisionAABB(&playerBox, &cp->interactZone);

    if (inRange && !cp->activated && (JOY_readJoypad(JOY_1) & BUTTON_UP)) {
        checkpointActivate(zoneID);
    }
}

void checkpointActivate(u8 zoneID) {
    checkpoints[zoneID].activated = TRUE;
    lastActivatedZone = zoneID;

    // Bonfire-style: resting fully restores health and stamina
    healDamage(player.base.maxHealth);
    player.stamina = player.maxStamina;
}

void checkpointRespawnPlayer(void) {
    // TODO: implement respawn-on-death.
    //
    // checkpoints[lastActivatedZone] holds the last checkpoint the player
    // activated (fix32 posX/posY, plus .zoneID). A few decisions are yours:
    //
    //  - If lastActivatedZone != getCurrentZone(), the player died in a
    //    different zone than their last checkpoint. Do you call loadZone()
    //    to send them back there, or keep it simpler for now and only
    //    support same-zone respawns?
    //  - Reset player.base.velX/velY (they died mid-air/mid-dash) and
    //    player.base.currentState back to PLAYER_STATE_IDLE.
    //  - Should this also fully heal (like checkpointActivate does), or
    //    only enough to survive?
}
