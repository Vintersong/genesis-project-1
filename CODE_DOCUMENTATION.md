# Genesis Game - Code Documentation

## Project Overview
This is a Sega Genesis/Mega Drive game project using the SGDK (Sega Genesis Development Kit). The game features a player character with movement, jumping, and a state machine system.

---

## Source Files

### 1. main.c
**Location:** `src/main.c`

**Purpose:** Entry point and main game loop

**Key Functions:**
- `main()` - Initializes the game and runs the main loop

**Game Loop:**
1. Initialize assets
2. Initialize player
3. Set up joystick event handler
4. Main loop:
   - Handle input and movements
   - Update player logic
   - Update background scrolling
   - Update camera
   - Update sprites
   - Process VBlank

**Dependencies:**
- genesis.h
- resources.h
- player.h
- assetLoader.h
- playerController.h
- camera.h

```c
#include <genesis.h>
#include <resources.h>
#include <player.h>
#include <assetLoader.h>
#include <playerController.h>
#include <camera.h>

int main()
{
    initializeAssets(); 
    playerInit();  
    JOY_setEventHandler(&joyEvent);

    while (1)
    {
        handleInputAndMovements();
        playerUpdate();
        updateBackgroundScroll();
        mainCamera();
        SPR_update();
       
        SYS_doVBlankProcess();
    }
    return (0);
}
```

---

### 2. player.c
**Location:** `src/player.c`

**Purpose:** Manages player state, physics, and movement

**Key Constants:**
- `GRAVITY` - FIX32(1) - Gravity constant for physics
- `JUMP_VELOCITY` - FIX32(-10) - Initial jump velocity

**Key Variables:**
- `player` - Static Player struct containing all player data

**Key Functions:**

#### `playerInit()`
Initializes the player with default values:
- Position: (0, 0)
- Velocities: (0, 0)
- Health: 100
- Stamina: 100
- Parry window: 20
- State: IDLE

#### `playerUpdate()`
Updates player state each frame:
- Applies gravity to vertical velocity
- Updates position based on velocity
- Handles state-specific behavior
- Decrements parry window when parrying

#### `movePlayerRight()`
Moves player to the right:
- Sets horizontal velocity to FIX32(4)
- Updates position
- Changes state to RUNNING

#### `movePlayerLeft()`
Moves player to the left:
- Sets horizontal velocity to FIX32(-4)
- Updates position
- Changes state to RUNNING

#### `moveJump()`
Makes the player jump:
- Only allows jumping if not already jumping
- Sets vertical velocity to JUMP_VELOCITY
- Changes state to JUMPING

**Fixed-Point Arithmetic:**
Uses SGDK's FIX32 for smooth movement and physics calculations.

```c
#include <player.h>
#include <genesis.h>

#define GRAVITY FIX32(1)
#define JUMP_VELOCITY FIX32(-10)
static Player player;

// [Functions implementation...]
```

---

### 3. playerController.c
**Location:** `src/playerController.c`

**Purpose:** Handles joystick input and translates it to player actions

**Key Variables:**
- `joyValue` - Static u16 holding current joystick state

**Key Functions:**

#### `joyEvent(u16 joy, u16 changed, u16 state)`
Event handler for button presses:
- Listens for JOY_1 (first controller)
- Button B triggers jump action
- Other buttons reserved for future implementation

#### `handleInputAndMovements()`
Processes continuous input (D-pad):
- Reads joystick state
- Handles conflicting inputs (left+right = idle)
- Manages horizontal movement
- Flips sprite based on direction
- Sets player to IDLE when no input

**Input Logic:**
- RIGHT + LEFT = No movement (idle state)
- RIGHT only = Move right, no horizontal flip
- LEFT only = Move left, horizontal flip enabled
- No input = Idle state

```c
#include <playerController.h>
#include <player.h>
#include <assetLoader.h>

static u16 joyValue;

void joyEvent(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) {
        if (changed & state) {
            switch (changed & state)
            {
                case BUTTON_B:
                    moveJump();
                    break;
                // Other buttons...
            }
        }
    }
}

void handleInputAndMovements() {
    joyValue = JOY_readJoypad(JOY_1);
    
    if ((joyValue & BUTTON_RIGHT) && (joyValue & BUTTON_LEFT)) {
        setPlayerState(PLAYER_STATE_IDLE);
        return;
    }
    
    if (joyValue & BUTTON_RIGHT) {
        SPR_setHFlip(playerSprite, FALSE);
        movePlayerRight();
    }
    else if(joyValue & BUTTON_LEFT) {
        SPR_setHFlip(playerSprite, TRUE);
        movePlayerLeft();
    }
    else {
        setPlayerState(PLAYER_STATE_IDLE);
    }
}
```

---

### 4. assetLoader.c
**Location:** `src/assetLoader.c`

**Purpose:** Loads and manages game assets (sprites, backgrounds, palettes)

**Global Variables:**
- `ind` - Tile index counter (starts at TILE_USER_INDEX)
- `scrollBackground_offset` - Background B scroll position
- `scrollForeground_offset` - Foreground (BG_A) scroll position
- `playerSprite` - Pointer to player sprite

**Key Functions:**

#### `initializeAssets()`
Master initialization function:
1. Initializes sprite engine
2. Loads player assets
3. Loads level assets
4. Sets scrolling mode

#### `loadPlayerAssets()`
Loads player sprite:
- Sets PAL2 palette for player
- Creates sprite at position (160, 180)
- Uses pSprite resource

#### `loadLevelAssets()`
Loads background layers:
- **Background B (PAL0):** Far background layer
- **Background A (PAL1):** Foreground layer
- Uses VDP_drawImageEx for proper image rendering
- Manages tile indices to prevent overlap

#### `updateBackgroundScroll()`
Creates parallax scrolling effect:
- Background scrolls at -1 pixels/frame
- Foreground scrolls at -2 pixels/frame (faster = closer)
- Creates depth illusion

**Asset Loading Flow:**
1. Load palette to VDP
2. Draw image to background plane
3. Increment tile index counter
4. Repeat for next layer

```c
#include <genesis.h>
#include <resources.h>
#include <player.h>

u16 ind = TILE_USER_INDEX;
int scrollBackground_offset = 0;
int scrollForeground_offset = 0;
Sprite *playerSprite;

void initializeAssets()
{
    SPR_init();
    loadPlayerAssets();
    loadLevelAssets();
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
}

// [Other functions...]
```

---

### 5. rom_head.c
**Location:** `src/boot/rom_head.c`

**Purpose:** Defines the ROM header required by Genesis hardware

**ROM Header Contents:**
- Console identifier: "SEGA MEGA DRIVE"
- Copyright: "(C)SGDK 2024"
- Domestic name: "SAMPLE PROGRAM"
- International name: "SAMPLE PROGRAM"
- Serial number: "GM 00000000-00"
- ROM size: 0x000FFFFF (1MB)
- RAM range: 0xE0FF0000 - 0xE0FFFFFF
- Backup RAM info
- Region codes: "JUE" (Japan, USA, Europe)

**Special Configurations:**
- Supports bank switching (SSF mapper)
- Supports MegaWiFi module
- Standard Mega Drive configuration

---

## Header Files

### player.h
**Location:** `inc/player.h`

**Enumerations:**
```c
typedef enum
{
    PLAYER_STATE_IDLE,
    PLAYER_STATE_RUNNING,
    PLAYER_STATE_JUMPING,
    PLAYER_STATE_ATTACKING,
    PLAYER_STATE_PARRYING
} PlayerState;
```

**Structures:**
```c
typedef struct
{
    fix32 posX, posY;           // Position (fixed-point)
    fix32 xVelocity, yVelocity; // Velocity (fixed-point)
    fix32 maxSpeed;             // Maximum speed
    u16 maxStamina;             // Maximum stamina
    u16 currentStamina;         // Current stamina
    u8 baseStamina;             // Base stamina value
    u16 maxHealth;              // Maximum health
    u16 currentHealth;          // Current health
    u16 baseHealth;             // Base health value
    u16 parryWindow;            // Parry timing window
    PlayerState currentState;   // Current state
} Player;
```

**Functions:**
- `playerInit()` - Initialize player
- `playerUpdate()` - Update player each frame
- `setPlayerState()` - Change player state
- `movePlayerRight()` - Move right
- `movePlayerLeft()` - Move left
- `moveJump()` - Jump

---

### playerController.h
**Location:** `inc/playerController.h`

**Functions:**
- `joyEvent()` - Handle button press events
- `handleInputAndMovements()` - Process continuous input

---

### assetLoader.h
**Location:** `inc/assetLoader.h`

**Global Variables:**
- `ind` - Tile index counter
- `scrollBackground_offset` - Background scroll offset
- `scrollForeground_offset` - Foreground scroll offset
- `playerSprite` - Player sprite pointer

**Functions:**
- `initializeAssets()` - Initialize all assets
- `loadPlayerAssets()` - Load player sprite
- `loadLevelAssets()` - Load background layers
- `updateBackgroundScroll()` - Update parallax scrolling

---

## Game Architecture

### Module Responsibilities

1. **Main Module** (`main.c`)
   - Game initialization
   - Main game loop coordination

2. **Player Module** (`player.c`, `player.h`)
   - Player data management
   - Physics and movement logic
   - State management

3. **Player Controller Module** (`playerController.c`, `playerController.h`)
   - Input handling
   - Joystick event processing
   - Input-to-action translation

4. **Asset Loader Module** (`assetLoader.c`, `assetLoader.h`)
   - Resource loading
   - Sprite management
   - Background rendering
   - Parallax scrolling

5. **State Machine Module** (Referenced but not implemented)
   - State transition logic
   - State validation

### Data Flow

```
Input (Joystick) 
    ↓
playerController.c (joyEvent, handleInputAndMovements)
    ↓
player.c (movePlayerRight, movePlayerLeft, moveJump)
    ↓
player.c (playerUpdate - applies physics)
    ↓
main.c (SPR_update - renders to screen)
```

---

## Technical Notes

### Fixed-Point Arithmetic
The project uses SGDK's `fix32` type for smooth movement:
- **Why?** Genesis doesn't have a floating-point unit
- **How?** Fixed-point provides decimal precision using integers
- **Usage:** All position and velocity values use FIX32()

### Sprite Management
- Sprites use SGDK's sprite engine (SPR_*)
- Player sprite allocated in `loadPlayerAssets()`
- Sprite flipping handled in `handleInputAndMovements()`

### Background Rendering
- Uses two background planes (BG_A and BG_B)
- Parallax scrolling creates depth
- Each plane has its own palette (PAL0, PAL1)
- Player uses separate palette (PAL2)

### State Machine
- Player has 5 states: IDLE, RUNNING, JUMPING, ATTACKING, PARRYING
- State transitions managed through `setPlayerState()`
- States affect player behavior and animations

---

## Future Improvements

1. **State Machine Implementation**
   - Create `stateMachine.c` with proper transition logic
   - Add state validation rules

2. **Combat System**
   - Implement attacking mechanics
   - Add parry timing system
   - Create hit detection

3. **Camera System**
   - Implement `mainCamera()` function
   - Add camera following logic
   - Handle screen boundaries

4. **Physics Refinements**
   - Add ground collision detection
   - Implement acceleration/deceleration
   - Add jump height variation based on button hold

5. **Animation System**
   - Add sprite animation for different states
   - Create smooth transitions between animations

---

## Build Information

**Framework:** SGDK (Sega Genesis Development Kit)  
**Target Platform:** Sega Genesis / Mega Drive  
**Language:** C  
**Build System:** SGDK build tools

---

*Last Updated: November 4, 2025*
