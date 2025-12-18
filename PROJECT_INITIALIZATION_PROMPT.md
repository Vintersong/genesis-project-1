# SGDK Vaporwave Platformer - Project Initialization Prompt

## Project Overview
Create the complete folder structure and initial C source files for a Sega Genesis/Mega Drive game using SGDK (Sega Genesis Development Kit). This is a Metroidvania-style platformer with vaporwave aesthetics, featuring zone-based progression through a computer system architecture theme.

---

## Target Platform
- **Hardware:** Sega Genesis/Mega Drive
- **Framework:** SGDK 1.80+
- **Language:** C (C99 standard)
- **Resolution:** 320x224 pixels
- **Memory:** 64KB RAM, up to 4MB ROM
- **Build System:** SGDK Makefile

---

## Project Structure to Create

```
genesis-vaporwave-game/
├── res/                          # Resources (create empty for now)
├── inc/                          # Header files
│   ├── core/
│   │   ├── game.h
│   │   ├── memory.h
│   │   └── config.h
│   ├── systems/
│   │   ├── camera.h
│   │   ├── collision.h
│   │   ├── input.h
│   │   ├── physics.h
│   │   └── animation.h
│   ├── entities/
│   │   └── player.h
│   ├── gameplay/
│   │   ├── abilities.h
│   │   └── stats.h
│   ├── world/
│   │   ├── zone.h
│   │   └── transition.h
│   └── ui/
│       └── hud.h
├── src/                          # Source files
│   ├── main.c
│   ├── core/
│   │   ├── game.c
│   │   └── memory.c
│   ├── systems/
│   │   ├── camera.c
│   │   ├── collision.c
│   │   ├── input.c
│   │   ├── physics.c
│   │   └── animation.c
│   ├── entities/
│   │   └── player.c
│   ├── gameplay/
│   │   ├── abilities.c
│   │   └── stats.c
│   ├── world/
│   │   ├── zone.c
│   │   └── transition.c
│   └── ui/
│       └── hud.c
├── Makefile
└── README.md
```

---

## SGDK-Specific Requirements

### Standard SGDK Includes
Every C file that uses SGDK functions must include:
```c
#include <genesis.h>
```

### SGDK Data Types
Use these Genesis-specific types:
- `u8` - unsigned 8-bit integer
- `u16` - unsigned 16-bit integer  
- `u32` - unsigned 32-bit integer
- `s8` - signed 8-bit integer
- `s16` - signed 16-bit integer
- `s32` - signed 32-bit integer
- `fix32` - fixed-point 32-bit number (for smooth movement/physics)
- `bool` - boolean (TRUE/FALSE)

### Fixed-Point Arithmetic
Genesis has no floating-point unit. Use SGDK's fixed-point system:
```c
fix32 velocity = FIX32(4.5);     // Convert float to fix32
fix32 gravity = FIX32(0.5);
s16 intValue = fix32ToInt(velocity);  // Convert back to integer
```

### Include Guards
All header files must use include guards:
```c
#ifndef FILENAME_H
#define FILENAME_H

// Header content

#endif // FILENAME_H
```

---

## Core Constants and Configuration

### config.h - Game Configuration
```c
#ifndef CONFIG_H
#define CONFIG_H

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
#define TILE_SIZE 8

// Zone IDs
#define ZONE_CPU 0
#define ZONE_GPU 1
#define ZONE_RAM 2
#define ZONE_STORAGE 3
#define ZONE_HUB 4
#define ZONE_BIOS 5
#define ZONE_RESERVED 6

#endif // CONFIG_H
```

---

## Core Data Structures

### Player State Machine
```c
typedef enum {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_RUNNING,
    PLAYER_STATE_JUMPING,
    PLAYER_STATE_FALLING,
    PLAYER_STATE_DASHING,
    PLAYER_STATE_ATTACKING,
    PLAYER_STATE_PARRYING,
    PLAYER_STATE_BLOCKING,
    PLAYER_STATE_HURT,
    PLAYER_STATE_DEAD
} PlayerState;
```

### Player Structure
```c
typedef struct {
    // Position and movement (fixed-point for smooth sub-pixel movement)
    fix32 posX;
    fix32 posY;
    fix32 velX;
    fix32 velY;
    
    // Stats
    u16 health;
    u16 maxHealth;
    u16 stamina;
    u16 maxStamina;
    
    // State
    PlayerState currentState;
    bool onGround;
    bool facingRight;
    
    // Abilities
    bool canDash;
    u16 dashCooldown;
    u16 dashTimer;
    bool hasDashAbility;
    
    bool canDoubleJump;
    bool hasDoubleJumpAbility;
    
    u16 parryWindow;
    bool hasParryAbility;
    
    // Sprite reference (SGDK sprite pointer)
    Sprite* sprite;
    
} Player;
```

### Zone Structure
```c
typedef struct {
    u8 zoneID;
    u8 subZoneCount;
    u8 currentSubZone;
    bool discovered;
    bool completed;
} Zone;
```

### Game State
```c
typedef enum {
    GAME_STATE_TITLE,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER,
    GAME_STATE_TRANSITION
} GameState;
```

---

## Initial Implementation Requirements

### 1. main.c - Game Entry Point
Create the main game loop with:
- System initialization (SPR_init, JOY_init, etc.)
- Player initialization
- Basic game loop calling update functions
- VBlank synchronization using `SYS_doVBlankProcess()`

**Structure:**
```c
int main() {
    // Initialize systems
    gameInit();
    playerInit();
    
    // Main loop
    while(1) {
        // Update
        inputUpdate();
        playerUpdate();
        cameraUpdate();
        
        // Render
        SPR_update();
        SYS_doVBlankProcess();
    }
    
    return 0;
}
```

### 2. player.c/h - Player System
Implement:
- `playerInit()` - Initialize player at starting position
- `playerUpdate()` - Update player physics and state each frame
- `setPlayerState()` - Change player state with validation
- Movement functions:
  - `movePlayerLeft()`
  - `movePlayerRight()`
  - `playerJump()`
  - `playerDash()` (stub for now)
  - `playerParry()` (stub for now)

**Key behaviors:**
- Apply gravity when not on ground
- Update position based on velocity
- Handle state transitions
- Use fixed-point arithmetic for all movement calculations

### 3. input.c/h - Input Handling
Implement:
- `inputInit()` - Set up joystick event handler
- `inputUpdate()` - Read joystick state each frame
- `joyEventHandler()` - Handle button press/release events

**SGDK Functions to use:**
- `JOY_init()`
- `JOY_setEventHandler()`
- `JOY_readJoypad(JOY_1)`
- Button constants: `BUTTON_LEFT`, `BUTTON_RIGHT`, `BUTTON_UP`, `BUTTON_DOWN`, `BUTTON_A`, `BUTTON_B`, `BUTTON_C`, `BUTTON_START`

### 4. camera.c/h - Camera System
Implement:
- `cameraInit()` - Initialize camera position
- `cameraUpdate()` - Follow player with smooth interpolation
- `cameraSetTarget()` - Set what camera follows
- `cameraGetX()` / `cameraGetY()` - Get current camera position

**Camera behavior:**
- Follow player with slight lag (smooth follow)
- Constrain to screen boundaries
- Dead zone in center (don't move camera for small player movements)

### 5. collision.c/h - Collision Detection
Implement basic AABB collision:
- `collisionAABB()` - Check if two boxes overlap
- `collisionPointRect()` - Check if point is inside rectangle
- Ground detection (for now, use simple Y-position threshold)

**Collision box structure:**
```c
typedef struct {
    s16 x;
    s16 y;
    u16 width;
    u16 height;
} CollisionBox;
```

### 6. physics.c/h - Physics System
Implement:
- `physicsUpdate()` - Apply physics forces
- `applyGravity()` - Add gravity to entities
- `applyFriction()` - Slow down moving entities
- `integrateVelocity()` - Update position from velocity

**Use fixed-point math for all physics calculations**

### 7. game.c/h - Game State Management
Implement:
- `gameInit()` - Initialize all game systems
- `gameUpdate()` - Update current game state
- `gameChangeState()` - Transition between game states
- Global game state variable

### 8. zone.c/h - Zone System (Basic)
Implement stubs for:
- `zoneInit()`
- `loadZone(u8 zoneID)`
- `unloadZone()`
- `getCurrentZone()`

For now, just track current zone ID. Full implementation comes later.

### 9. stats.c/h - Stats Management
Implement:
- `statsInit()` - Initialize player stats
- `takeDamage()` - Reduce health
- `healDamage()` - Restore health
- `useStamina()` - Consume stamina
- `regenStamina()` - Restore stamina over time

### 10. hud.c/h - HUD Display
Implement basic text display:
- `hudInit()` - Set up HUD
- `hudUpdate()` - Update HUD each frame
- `hudRender()` - Draw health/stamina (use VDP text for now)

**Use SGDK text functions:**
- `VDP_drawText()` for displaying text
- Display current zone, health, stamina

---

## Coding Standards

### Naming Conventions
- **Functions:** camelCase (`playerUpdate`, `movePlayerLeft`)
- **Structs:** PascalCase (`Player`, `CollisionBox`)
- **Enums:** UPPER_CASE with prefix (`PLAYER_STATE_IDLE`, `ZONE_CPU`)
- **Constants:** UPPER_CASE (`MAX_HEALTH`, `SCREEN_WIDTH`)
- **Global variables:** camelCase with descriptive names
- **Local variables:** camelCase, short but clear

### Function Organization
```c
// Function declarations at top of .c file (if needed internally)
static void helperFunction();

// Public functions
void publicFunction() {
    // Implementation
}

// Static/private functions at bottom
static void helperFunction() {
    // Implementation
}
```

### Comments
- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Document all public functions in headers:
```c
/**
 * @brief Updates player position and state
 * 
 * Called every frame to handle player physics,
 * state transitions, and collision detection.
 */
void playerUpdate();
```

### Error Handling
For Genesis development, avoid dynamic allocation. Pre-allocate all structures.
```c
// Good - static allocation
static Player player;

// Avoid - dynamic allocation (no malloc on Genesis)
// Player* player = malloc(sizeof(Player));
```

---

## SGDK Build System

### Makefile Template
```makefile
# Project name
PROJECT = vaporwave-game

# SGDK path (adjust to your installation)
SGDK = $(SGDK_HOME)

# Source files
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c) \
      $(wildcard $(SRC_DIR)/core/*.c) \
      $(wildcard $(SRC_DIR)/systems/*.c) \
      $(wildcard $(SRC_DIR)/entities/*.c) \
      $(wildcard $(SRC_DIR)/gameplay/*.c) \
      $(wildcard $(SRC_DIR)/world/*.c) \
      $(wildcard $(SRC_DIR)/ui/*.c)

# Include directories
INC = -Iinc \
      -Iinc/core \
      -Iinc/systems \
      -Iinc/entities \
      -Iinc/gameplay \
      -Iinc/world \
      -Iinc/ui

# Resources
RES = res

# Output
OUT = out

# Use SGDK makefile
include $(SGDK)/makefile.gen
```

---

## Implementation Steps

### Step 1: Create Directory Structure
Create all folders as specified in the project structure section.

### Step 2: Create All Header Files
For each .h file, create:
1. Include guards
2. Necessary includes
3. Type definitions (enums, structs)
4. Function declarations
5. Extern variable declarations (if needed)

### Step 3: Create All Source Files
For each .c file, create:
1. Include corresponding .h file
2. Include SGDK genesis.h
3. Static variables (if needed)
4. Function implementations
5. Initialize with basic/stub implementations

### Step 4: Implement main.c
Create functional main game loop that:
1. Initializes all systems
2. Runs game loop
3. Calls update functions
4. Handles VBlank

### Step 5: Test Compilation
Ensure project compiles without errors using SGDK build system.

---

## Initial Feature Set (Minimal Viable Product)

At this stage, the game should:
- ✅ Compile successfully
- ✅ Initialize Genesis hardware
- ✅ Display player sprite (placeholder for now)
- ✅ Accept controller input
- ✅ Move player left/right with D-pad
- ✅ Jump with B button
- ✅ Apply gravity
- ✅ Basic ground collision (Y-position threshold)
- ✅ Camera follows player
- ✅ Display basic HUD text (health, zone name)
- ✅ Run at stable 60 FPS

## What NOT to Implement Yet
- ❌ Actual graphics/sprites (use placeholders)
- ❌ Zone loading from data files
- ❌ Enemy systems
- ❌ Combat mechanics beyond stubs
- ❌ Animation system (use static sprites)
- ❌ Audio system
- ❌ Save/load system
- ❌ Full collision with tilemaps
- ❌ Dash/parry mechanics (just stubs)

---

## Expected Output

After implementation, the project should:
1. Compile to a .bin ROM file
2. Run in Genesis emulator (Gens, Blastem, etc.)
3. Show a controllable sprite moving on screen
4. Respond to D-pad and buttons
5. Apply basic physics (gravity, movement)
6. Display debug text showing game state

---

## Testing Checklist

- [ ] Project compiles without errors
- [ ] Project compiles without warnings
- [ ] ROM runs in emulator
- [ ] Player sprite visible on screen
- [ ] Left/Right movement works
- [ ] Jump works
- [ ] Gravity applies correctly
- [ ] Can't walk off screen
- [ ] HUD displays correctly
- [ ] Game runs at 60 FPS (no slowdown)

---

## Additional Notes

### Memory Management
- All structs should be statically allocated
- Use TILE_USER_INDEX for custom tiles
- Track VRAM usage carefully
- Genesis has only 64KB RAM - be conservative

### Genesis Hardware Limits
- Max 80 sprites total
- Max 20 sprites per scanline
- Max 320 pixels of sprite width per scanline
- 4 background planes (but use 2-3 for compatibility)
- 4 palettes of 16 colors each (one color is transparent)

### SGDK Documentation
Reference official SGDK docs for:
- Sprite management: SPR_* functions
- VDP functions: VDP_* functions
- Input: JOY_* functions
- DMA transfers: DMA_* functions

---

## Final Implementation Notes

**This is a FOUNDATION ONLY.**

The goal is to create a solid, compilable base that:
- Has proper file organization
- Uses correct SGDK patterns
- Follows Genesis best practices
- Can be built upon incrementally

**Focus on:**
- Clean, readable code
- Proper use of SGDK types and functions
- Fixed-point math for movement
- Stable game loop
- Modular architecture for future expansion

**Defer until later phases:**
- Graphics and art assets
- Level data and loading
- Complex game mechanics
- Enemy AI and combat
- Audio implementation

---

## Success Criteria

You will know the implementation is successful when:
1. ✅ All files compile without errors or warnings
2. ✅ ROM runs in Genesis emulator
3. ✅ Player sprite appears and moves smoothly
4. ✅ Input is responsive
5. ✅ Physics feels correct
6. ✅ Code is organized and maintainable
7. ✅ Ready for next implementation phase (zones, enemies, etc.)

---

**This prompt should be sufficient for an AI coding assistant to generate the complete initial project structure and basic implementation. All code should follow SGDK conventions and Genesis hardware limitations.**
