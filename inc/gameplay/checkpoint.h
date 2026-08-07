#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <genesis.h>
#include "core/config.h"
#include "systems/collision.h"

#define NUM_ZONES 7   // matches ZONE_CPU..ZONE_RESERVED in core/config.h

typedef struct {
    fix32 posX, posY;
    u8 zoneID;
    bool activated;
    CollisionBox interactZone;   // proximity trigger, centered on posX/posY
} Checkpoint;

extern Checkpoint checkpoints[NUM_ZONES];

/**
 * @brief Set up one checkpoint per zone at their (placeholder) world positions.
 * None are activated yet; the HUB checkpoint is activated by default so a
 * fresh game always has a valid respawn point.
 */
void checkpointInit(void);

/**
 * @brief Check player proximity each frame and activate a checkpoint on
 * interact (Up) input while in range.
 */
void checkpointUpdate(void);

/**
 * @brief Activate the checkpoint for a given zone: full heal/stamina restore
 * and set it as the current respawn point.
 * @param zoneID Zone whose checkpoint should be activated
 */
void checkpointActivate(u8 zoneID);

/**
 * @brief Respawn the player at the last activated checkpoint.
 * Called when the player dies.
 */
void checkpointRespawnPlayer(void);

#endif // CHECKPOINT_H
