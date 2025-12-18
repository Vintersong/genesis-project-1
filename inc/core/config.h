#ifndef CONFIG_H
#define CONFIG_H

#include <genesis.h>

// Screen dimensions
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 224

// Physics constants
#define GRAVITY FIX32(0.5)
#define MAX_FALL_SPEED FIX32(8.0)
#define GROUND_FRICTION FIX32(0.85)
#define AIR_FRICTION FIX32(0.95)

// Player constants
#define PLAYER_WALK_SPEED FIX32(2.0)
#define PLAYER_RUN_SPEED FIX32(4.0)
#define PLAYER_JUMP_VELOCITY FIX32(-8.0)
#define PLAYER_DASH_DISTANCE 32  // pixels
#define PLAYER_DASH_DURATION 8   // frames
#define PLAYER_PARRY_WINDOW 20   // frames

// Game constants
#define TARGET_FPS 60
#define TILE_DIMENSION 8  // Tile size in pixels (renamed to avoid SGDK conflict)

// Zone IDs
#define ZONE_CPU 0
#define ZONE_GPU 1
#define ZONE_RAM 2
#define ZONE_STORAGE 3
#define ZONE_HUB 4
#define ZONE_BIOS 5
#define ZONE_RESERVED 6

#endif // CONFIG_H
