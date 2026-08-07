# Combat Actor Entities (Entity Base, Enemy, Boss) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Introduce a shared `Entity` base struct, refactor `Player` onto it, fix the broken `Enemy` stub, and add a new `Boss` type — all as minimal stubs (physics + bounds only, no AI/combat logic).

**Architecture:** A new `Entity` struct (`inc/core/entity.h`, `src/core/entity.c`) holds position/velocity/health/state/collision/sprite fields and three shared helper functions (`entityApplyPhysics`, `entityTakeDamage`, `entityKeepInBounds`). `Player`, `Enemy`, and `Boss` each embed `Entity base;` as their first struct member, which is a valid `Entity*` per C's struct-layout guarantee. `Enemy`/`Boss` get single static global instances (matching the current `Player` pattern), wired into `initializeAssets()` and the `gameInit()`/`gameUpdate()` loop.

**Tech Stack:** C (C99), SGDK (Sega Genesis Development Kit), fixed-point (`fix32`) math, no dynamic allocation, no host-side unit test framework (68000/Genesis target).

## Global Constraints

- No `malloc`/dynamic allocation — all structs are statically allocated (per `PROJECT_INITIALIZATION_PROMPT.md`)
- Naming: functions camelCase, structs PascalCase, enums `UPPER_CASE` with type prefix (e.g. `ENEMY_STATE_IDLE`) — matches existing `player.h`/`config.h` conventions
- Every header uses `#ifndef`/`#define`/`#endif` include guards
- Single static instance per actor type (`Enemy enemy;`, `Boss boss;`) — no arrays/pools in this plan
- No combat/hit-detection wiring, no AI/decision logic, no phase logic — out of scope per spec
- Build via `$GDK/bin/make -f $GDK/makefile.gen -j1 release` (GDK env var already set to `D:/RetroProjects/SGDK` in this environment) — this is the "test" step for every task since there is no host-side test runner for 68000 code

**Spec:** `docs/superpowers/specs/2026-08-07-combat-actor-entities-design.md`

---

## File Structure

| File | Action | Responsibility |
|---|---|---|
| `inc/core/entity.h` | Create | `Entity` struct definition, declares shared helper function signatures |
| `src/core/entity.c` | Create | Implements `entityApplyPhysics`, `entityTakeDamage`, `entityKeepInBounds` |
| `inc/entities/player.h` | Modify | `Player` struct embeds `Entity base;` instead of flat fields |
| `src/entities/player.c` | Modify | All `player.X` field accesses become `player.base.X`; inlined physics/bounds logic replaced with `entityApplyPhysics`/`entityKeepInBounds` calls |
| `inc/entities/enemy.h` | Modify (fix) | Add missing `#ifndef` guard, real `EnemyState` enum, `Enemy` struct with `Entity base;`, full function declarations |
| `src/entities/enemy.c` | Modify (fix) | Implement `enemyInit`, `enemyUpdate`, `enemyTakeDamage` |
| `inc/entities/boss.h` | Create | `BossState` enum, `Boss` struct (`Entity base;` + `currentPhase`, `attackTimer`), function declarations |
| `src/entities/boss.c` | Create | Implement `bossInit`, `bossUpdate`, `bossTakeDamage` |
| `inc/assetLoader.h` | Modify | Declare `enemySprite`, `bossSprite`, `loadEnemyAssets()`, `loadBossAssets()` |
| `src/assetLoader.c` | Modify | Implement `loadEnemyAssets()`/`loadBossAssets()` (reuse `pSprite` resource, different palette slots); call both from `initializeAssets()` |
| `src/core/game.c` | Modify | Call `enemyInit()`/`bossInit()` in `gameInit()`, `enemyUpdate()`/`bossUpdate()` in `gameUpdate()`'s `GAME_STATE_PLAYING` case |

No Makefile changes — `src/core/*.c` and `src/entities/*.c` are already covered by the wildcard `SRC`/`INC` patterns.

---

## Task 1: `Entity` base struct and shared helpers

**Files:**
- Create: `inc/core/entity.h`
- Create: `src/core/entity.c`

**Interfaces:**
- Consumes: `CollisionBox` from `inc/systems/collision.h`; `applyGravity`, `applyFriction`, `integrateVelocity` from `inc/systems/physics.h`; `checkGroundCollision` from `inc/systems/collision.h`; `SCREEN_WIDTH` from `inc/core/config.h`
- Produces: `Entity` struct; `void entityApplyPhysics(Entity* e)`; `bool entityTakeDamage(Entity* e, u16 amount)`; `void entityKeepInBounds(Entity* e)`. These are used by Task 2 (Player refactor), Task 3 (Enemy), and Task 4 (Boss).

- [ ] **Step 1: Write `inc/core/entity.h`**

```c
#ifndef ENTITY_H
#define ENTITY_H

#include <genesis.h>
#include "systems/collision.h"

typedef struct {
    fix32 posX, posY;
    fix32 velX, velY;
    u16 health, maxHealth;
    u8 currentState;   // holds a per-type state enum value (e.g. PlayerState), cast at use sites
    bool onGround;
    bool facingRight;
    CollisionBox hitbox;
    Sprite* sprite;
} Entity;

/**
 * @brief Apply gravity, friction, ground check, and velocity integration to an entity
 * @param e Pointer to the entity to update
 */
void entityApplyPhysics(Entity* e);

/**
 * @brief Reduce an entity's health by amount, clamped at 0
 * @param e Pointer to the entity taking damage
 * @param amount Damage amount
 * @return TRUE if health reached 0, FALSE otherwise
 */
bool entityTakeDamage(Entity* e, u16 amount);

/**
 * @brief Clamp an entity's horizontal position to screen bounds
 * @param e Pointer to the entity to clamp
 */
void entityKeepInBounds(Entity* e);

#endif // ENTITY_H
```

- [ ] **Step 2: Write `src/core/entity.c`**

```c
#include <genesis.h>
#include "core/entity.h"
#include "core/config.h"
#include "systems/physics.h"
#include "systems/collision.h"

void entityApplyPhysics(Entity* e) {
    if (!e) return;

    e->onGround = checkGroundCollision(e->posY);

    if (!e->onGround) {
        applyGravity(&e->velY);
    } else if (e->velY > FIX32(0)) {
        e->velY = FIX32(0);
    }

    applyFriction(&e->velX, &e->velY, e->onGround);
    integrateVelocity(&e->posX, &e->posY, e->velX, e->velY);
}

bool entityTakeDamage(Entity* e, u16 amount) {
    if (!e) return FALSE;

    if (amount >= e->health) {
        e->health = 0;
    } else {
        e->health -= amount;
    }

    return e->health == 0;
}

void entityKeepInBounds(Entity* e) {
    if (!e) return;

    if (e->posX < FIX32(0)) {
        e->posX = FIX32(0);
        e->velX = FIX32(0);
    }
    if (e->posX > FIX32(SCREEN_WIDTH - 16)) {
        e->posX = FIX32(SCREEN_WIDTH - 16);
        e->velX = FIX32(0);
    }
}
```

- [ ] **Step 3: Build to verify it compiles**

Run: `cd "D:/RetroProjects/Genesis Projects/genesis-project-1" && GDK=D:/RetroProjects/SGDK "$GDK/bin/make" -f "$GDK/makefile.gen" -j1 release`
Expected: build proceeds without errors mentioning `entity.c`/`entity.h` (other errors are expected at this point since Task 2 hasn't run yet — only confirm no errors reference the new files; if the whole project already built cleanly before this task, it should still build cleanly now since nothing yet calls the new functions)

- [ ] **Step 4: Commit**

```bash
git add inc/core/entity.h src/core/entity.c
git commit -m "Add shared Entity base struct with physics/damage/bounds helpers"
```

---

## Task 2: Refactor `Player` onto the `Entity` base

**Files:**
- Modify: `inc/entities/player.h`
- Modify: `src/entities/player.c`

**Interfaces:**
- Consumes: `Entity`, `entityApplyPhysics`, `entityTakeDamage`, `entityKeepInBounds` from Task 1 (`inc/core/entity.h`)
- Produces: `Player.base` field of type `Entity` — Tasks 3/4 don't depend on this directly, but this establishes the pattern they follow

- [ ] **Step 1: Rewrite `inc/entities/player.h`**

```c
#ifndef PLAYER_H
#define PLAYER_H

#include <genesis.h>
#include "core/config.h"
#include "core/entity.h"

typedef enum
{
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

typedef struct
{
    Entity base;

    // Stats
    u16 stamina;
    u16 maxStamina;

    // Abilities
    bool canDash;
    u16 dashCooldown;
    u16 dashTimer;
    bool hasDashAbility;

    bool canDoubleJump;
    bool hasDoubleJumpAbility;

    u16 parryWindow;
    bool hasParryAbility;
} Player;

extern Player player;

// Core player functions
/**
 * @brief Initialize player at starting position with default stats
 */
void playerInit();

/**
 * @brief Updates player position and state
 *
 * Called every frame to handle player physics,
 * state transitions, and collision detection.
 */
void playerUpdate();

/**
 * @brief Change player state with validation
 * @param newState The state to transition to
 */
void setPlayerState(PlayerState newState);

// Movement functions
void movePlayerLeft();
void movePlayerRight();
void playerJump();
void playerDash();

// Combat functions
void playerParry();
void playerAttack();

// Input handling functions
/**
 * @brief Handle button press events
 * @param joy Joystick number
 * @param changed Buttons that changed state
 * @param state Current button state
 */
void playerJoyEvent(u16 joy, u16 changed, u16 state);

/**
 * @brief Handle continuous input (D-pad) each frame
 */
void playerHandleInput();

#endif // PLAYER_H
```

- [ ] **Step 2: Rewrite `src/entities/player.c`**

```c
#include <genesis.h>
#include "entities/player.h"
#include "assetLoader.h"
#include "core/config.h"
#include "core/entity.h"
#include "systems/physics.h"
#include "systems/collision.h"

// Global player instance
Player player;

void playerInit() {
    // Initialize position (center of screen)
    player.base.posX = FIX32(160);
    player.base.posY = FIX32(100);
    player.base.velX = FIX32(0);
    player.base.velY = FIX32(0);

    // Initialize stats
    player.base.health = 100;
    player.base.maxHealth = 100;
    player.stamina = 100;
    player.maxStamina = 100;

    // Initialize state
    player.base.currentState = PLAYER_STATE_IDLE;
    player.base.onGround = FALSE;
    player.base.facingRight = TRUE;

    // Initialize abilities (locked at start)
    player.canDash = FALSE;
    player.dashCooldown = 0;
    player.dashTimer = 0;
    player.hasDashAbility = FALSE;

    player.canDoubleJump = FALSE;
    player.hasDoubleJumpAbility = FALSE;

    player.parryWindow = PLAYER_PARRY_WINDOW;
    player.hasParryAbility = FALSE;

    // Sprite reference will be set by asset loader
    player.base.sprite = playerSprite;
}

void playerUpdate() {
    // Apply physics if not dashing
    if (player.base.currentState != PLAYER_STATE_DASHING) {
        entityApplyPhysics(&player.base);

        // If was jumping/falling and landed, return to idle or running
        if (player.base.onGround &&
            (player.base.currentState == PLAYER_STATE_JUMPING ||
             player.base.currentState == PLAYER_STATE_FALLING)) {
            s16 velXInt = player.base.velX >> 16; // Convert fix32 to int
            if (velXInt == 0) {
                setPlayerState(PLAYER_STATE_IDLE);
            } else {
                setPlayerState(PLAYER_STATE_RUNNING);
            }
        }
    }

    // Handle state-specific updates
    switch (player.base.currentState) {
        case PLAYER_STATE_DASHING:
            // Update dash timer
            if (player.dashTimer > 0) {
                player.dashTimer--;
            } else {
                setPlayerState(PLAYER_STATE_IDLE);
                player.canDash = FALSE;
            }
            break;

        case PLAYER_STATE_PARRYING:
            // Update parry window
            if (player.parryWindow > 0) {
                player.parryWindow--;
            } else {
                setPlayerState(PLAYER_STATE_IDLE);
            }
            break;

        case PLAYER_STATE_JUMPING:
        case PLAYER_STATE_FALLING:
            // Check if we should transition to falling
            if (player.base.velY > FIX32(0) && player.base.currentState == PLAYER_STATE_JUMPING) {
                setPlayerState(PLAYER_STATE_FALLING);
            }
            break;

        default:
            break;
    }

    // Update dash cooldown
    if (player.dashCooldown > 0) {
        player.dashCooldown--;
    } else {
        player.canDash = TRUE;
    }

    // Regenerate stamina slowly
    if (player.stamina < player.maxStamina) {
        player.stamina++;
    }

    // Keep player in bounds
    entityKeepInBounds(&player.base);

    // Update sprite position
    if (player.base.sprite) {
        s16 spriteX = player.base.posX >> 16; // Convert fix32 to int
        s16 spriteY = player.base.posY >> 16;
        SPR_setPosition(player.base.sprite, spriteX, spriteY);
        SPR_setHFlip(player.base.sprite, !player.base.facingRight);
    }
}

void setPlayerState(PlayerState newState) {
    // Validate state transitions
    PlayerState oldState = (PlayerState)player.base.currentState;

    // Some states can't be interrupted
    if (oldState == PLAYER_STATE_DASHING && player.dashTimer > 0) {
        return; // Can't interrupt dash
    }

    if (oldState == PLAYER_STATE_HURT) {
        return; // Can't interrupt hurt state
    }

    // Update state
    player.base.currentState = newState;

    // Reset velocity when entering idle
    if (newState == PLAYER_STATE_IDLE) {
        player.base.velX = FIX32(0);
    }
}

void movePlayerRight() {
    // Can't move while dashing or hurt
    if (player.base.currentState == PLAYER_STATE_DASHING ||
        player.base.currentState == PLAYER_STATE_HURT) {
        return;
    }

    // Set velocity
    player.base.velX = PLAYER_WALK_SPEED;
    player.base.facingRight = TRUE;

    // Update state if on ground
    if (player.base.onGround) {
        setPlayerState(PLAYER_STATE_RUNNING);
    }
}

void movePlayerLeft() {
    // Can't move while dashing or hurt
    if (player.base.currentState == PLAYER_STATE_DASHING ||
        player.base.currentState == PLAYER_STATE_HURT) {
        return;
    }

    // Set velocity
    player.base.velX = -PLAYER_WALK_SPEED;
    player.base.facingRight = FALSE;

    // Update state if on ground
    if (player.base.onGround) {
        setPlayerState(PLAYER_STATE_RUNNING);
    }
}

void playerJump() {
    // Can only jump if on ground or have double jump
    if (player.base.onGround) {
        player.base.velY = PLAYER_JUMP_VELOCITY;
        setPlayerState(PLAYER_STATE_JUMPING);
        player.canDoubleJump = player.hasDoubleJumpAbility;
    } else if (player.canDoubleJump && player.hasDoubleJumpAbility) {
        // Double jump
        player.base.velY = PLAYER_JUMP_VELOCITY;
        player.canDoubleJump = FALSE;
    }
}

void playerDash() {
    // Check if can dash
    if (!player.hasDashAbility || !player.canDash) {
        return;
    }

    // Check stamina
    if (player.stamina < 20) {
        return;
    }

    // Consume stamina
    player.stamina -= 20;

    // Set dash state
    setPlayerState(PLAYER_STATE_DASHING);
    player.dashTimer = PLAYER_DASH_DURATION;
    player.dashCooldown = 30; // 0.5 second cooldown at 60fps

    // Apply dash velocity in facing direction
    if (player.base.facingRight) {
        player.base.velX = FIX32(8.0);
    } else {
        player.base.velX = FIX32(-8.0);
    }
    player.base.velY = FIX32(0);
}

void playerParry() {
    // Check if has ability
    if (!player.hasParryAbility) {
        return;
    }

    // Can't parry in certain states
    if (player.base.currentState == PLAYER_STATE_DASHING ||
        player.base.currentState == PLAYER_STATE_HURT) {
        return;
    }

    // Enter parry state
    setPlayerState(PLAYER_STATE_PARRYING);
    player.parryWindow = PLAYER_PARRY_WINDOW;
    player.base.velX = FIX32(0);
}

void playerAttack() {
    // Basic attack stub - to be implemented
    // Check stamina
    if (player.stamina < 10) {
        return;
    }

    // Can't attack in certain states
    if (player.base.currentState == PLAYER_STATE_DASHING ||
        player.base.currentState == PLAYER_STATE_PARRYING ||
        player.base.currentState == PLAYER_STATE_HURT) {
        return;
    }

    // Consume stamina
    player.stamina -= 10;

    // Set attack state (to be implemented with animation)
    setPlayerState(PLAYER_STATE_ATTACKING);
}

// ============================================================================
// Input Handling (merged from playerController)
// ============================================================================

void playerJoyEvent(u16 joy, u16 changed, u16 state) {
    // Only handle input from first controller
    if (joy != JOY_1) return;

    // Check for button presses (changed & state = newly pressed)
    if (changed & state) {
        // Jump button (B)
        if (changed & state & BUTTON_B) {
            playerJump();
        }

        // Dash button (A) - if ability unlocked
        if (changed & state & BUTTON_A) {
            if (player.hasDashAbility) {
                playerDash();
            }
        }

        // Parry button (C) - if ability unlocked
        if (changed & state & BUTTON_C) {
            if (player.hasParryAbility) {
                playerParry();
            }
        }

        // Attack button (X/Y/Z) - for 6-button controller
        if (changed & state & (BUTTON_X | BUTTON_Y | BUTTON_Z)) {
            playerAttack();
        }
    }
}

void playerHandleInput() {
    // Read current joystick state
    u16 joyValue = JOY_readJoypad(JOY_1);

    // If both left and right are pressed, do nothing
    if ((joyValue & BUTTON_RIGHT) && (joyValue & BUTTON_LEFT)) {
        if (player.base.onGround && player.base.currentState != PLAYER_STATE_JUMPING) {
            setPlayerState(PLAYER_STATE_IDLE);
        }
        return;
    }

    // Handle horizontal movement
    if (joyValue & BUTTON_RIGHT) {
        movePlayerRight();
        if (player.base.sprite) {
            SPR_setHFlip(player.base.sprite, FALSE);
        }
    } else if (joyValue & BUTTON_LEFT) {
        movePlayerLeft();
        if (player.base.sprite) {
            SPR_setHFlip(player.base.sprite, TRUE);
        }
    } else {
        // No horizontal input - go to idle if on ground and not jumping
        if (player.base.onGround && player.base.currentState != PLAYER_STATE_JUMPING) {
            setPlayerState(PLAYER_STATE_IDLE);
        }
    }
}
```

- [ ] **Step 3: Build to verify it compiles**

Run: `cd "D:/RetroProjects/Genesis Projects/genesis-project-1" && GDK=D:/RetroProjects/SGDK "$GDK/bin/make" -f "$GDK/makefile.gen" -j1 release`
Expected: build succeeds with no errors (this is the first point where the whole project must build cleanly, since `player.c` is referenced from `main.c`/`game.c`)

- [ ] **Step 4: Commit**

```bash
git add inc/entities/player.h src/entities/player.c
git commit -m "Refactor Player onto shared Entity base struct"
```

---

## Task 3: Fix `Enemy`

**Files:**
- Modify: `inc/entities/enemy.h` (currently broken: missing `#ifndef` guard opener, empty struct)
- Modify: `src/entities/enemy.c` (currently: struct instance declared, no function implementations)

**Interfaces:**
- Consumes: `Entity`, `entityApplyPhysics`, `entityTakeDamage` from Task 1 (`inc/core/entity.h`); `enemySprite` from Task 5 (`inc/assetLoader.h`) — declared as `extern` here, defined in Task 5, so this task compiles standalone (the sprite pointer will just be unset/NULL until Task 5 runs, guarded by a null check)
- Produces: `Enemy enemy;` global instance; `void enemyInit(void)`; `void enemyUpdate(void)`; `void enemyTakeDamage(u16 amount)` — consumed by Task 6 (main loop integration)

- [ ] **Step 1: Rewrite `inc/entities/enemy.h`**

```c
#ifndef ENEMY_H
#define ENEMY_H

#include <genesis.h>
#include "core/config.h"
#include "core/entity.h"

typedef enum {
    ENEMY_STATE_IDLE,
    ENEMY_STATE_RUNNING,
    ENEMY_STATE_JUMPING,
    ENEMY_STATE_FALLING,
    ENEMY_STATE_HURT,
    ENEMY_STATE_DEAD
} EnemyState;

typedef struct {
    Entity base;
} Enemy;

extern Enemy enemy;

/**
 * @brief Initialize enemy at a default position with default stats
 */
void enemyInit(void);

/**
 * @brief Update enemy physics and state each frame
 */
void enemyUpdate(void);

/**
 * @brief Reduce enemy health and transition to HURT/DEAD as appropriate
 * @param amount Damage amount
 */
void enemyTakeDamage(u16 amount);

#endif // ENEMY_H
```

- [ ] **Step 2: Rewrite `src/entities/enemy.c`**

```c
#include <genesis.h>
#include "entities/enemy.h"
#include "assetLoader.h"
#include "core/config.h"
#include "core/entity.h"
#include "systems/physics.h"
#include "systems/collision.h"

Enemy enemy;

void enemyInit(void) {
    enemy.base.posX = FIX32(220);
    enemy.base.posY = FIX32(100);
    enemy.base.velX = FIX32(0);
    enemy.base.velY = FIX32(0);

    enemy.base.health = 30;
    enemy.base.maxHealth = 30;

    enemy.base.currentState = ENEMY_STATE_IDLE;
    enemy.base.onGround = FALSE;
    enemy.base.facingRight = TRUE;

    enemy.base.sprite = enemySprite;
}

void enemyUpdate(void) {
    if (enemy.base.currentState == ENEMY_STATE_DEAD) {
        return;
    }

    entityApplyPhysics(&enemy.base);
    entityKeepInBounds(&enemy.base);

    if (enemy.base.currentState != ENEMY_STATE_HURT) {
        enemy.base.currentState = enemy.base.onGround ? ENEMY_STATE_IDLE : ENEMY_STATE_FALLING;
    }

    if (enemy.base.sprite) {
        s16 spriteX = enemy.base.posX >> 16;
        s16 spriteY = enemy.base.posY >> 16;
        SPR_setPosition(enemy.base.sprite, spriteX, spriteY);
        SPR_setHFlip(enemy.base.sprite, !enemy.base.facingRight);
    }
}

void enemyTakeDamage(u16 amount) {
    bool died = entityTakeDamage(&enemy.base, amount);
    enemy.base.currentState = died ? ENEMY_STATE_DEAD : ENEMY_STATE_HURT;
}
```

- [ ] **Step 3: Build to verify it compiles**

Run: `cd "D:/RetroProjects/Genesis Projects/genesis-project-1" && GDK=D:/RetroProjects/SGDK "$GDK/bin/make" -f "$GDK/makefile.gen" -j1 release`
Expected: build fails with an undefined-reference/unresolved-symbol error for `enemySprite` (it's declared `extern` in `assetLoader.h` per Task 5's interface but not yet defined until Task 5 runs) — confirm the *only* new error is about `enemySprite`, not about `Enemy`/`enemyInit`/`enemyUpdate`/`enemyTakeDamage` or `Entity`

- [ ] **Step 4: Commit**

```bash
git add inc/entities/enemy.h src/entities/enemy.c
git commit -m "Fix broken Enemy stub: add include guard, real struct, implementations"
```

---

## Task 4: Add `Boss`

**Files:**
- Create: `inc/entities/boss.h`
- Create: `src/entities/boss.c`

**Interfaces:**
- Consumes: `Entity`, `entityApplyPhysics`, `entityTakeDamage` from Task 1 (`inc/core/entity.h`); `bossSprite` from Task 5 (`inc/assetLoader.h`, extern — same forward-reference pattern as Task 3)
- Produces: `Boss boss;` global instance; `void bossInit(void)`; `void bossUpdate(void)`; `void bossTakeDamage(u16 amount)` — consumed by Task 6 (main loop integration)

- [ ] **Step 1: Write `inc/entities/boss.h`**

```c
#ifndef BOSS_H
#define BOSS_H

#include <genesis.h>
#include "core/config.h"
#include "core/entity.h"

typedef enum {
    BOSS_STATE_IDLE,
    BOSS_STATE_RUNNING,
    BOSS_STATE_JUMPING,
    BOSS_STATE_FALLING,
    BOSS_STATE_HURT,
    BOSS_STATE_DEAD
} BossState;

typedef struct {
    Entity base;
    u8 currentPhase;   // reserved for future phase logic; always 0 in this plan
    u16 attackTimer;   // reserved for future attack-pattern logic; unused in this plan
} Boss;

extern Boss boss;

/**
 * @brief Initialize boss at a default position with default stats
 */
void bossInit(void);

/**
 * @brief Update boss physics and state each frame
 */
void bossUpdate(void);

/**
 * @brief Reduce boss health and transition to HURT/DEAD as appropriate
 * @param amount Damage amount
 */
void bossTakeDamage(u16 amount);

#endif // BOSS_H
```

- [ ] **Step 2: Write `src/entities/boss.c`**

```c
#include <genesis.h>
#include "entities/boss.h"
#include "assetLoader.h"
#include "core/config.h"
#include "core/entity.h"
#include "systems/physics.h"
#include "systems/collision.h"

Boss boss;

void bossInit(void) {
    boss.base.posX = FIX32(280);
    boss.base.posY = FIX32(100);
    boss.base.velX = FIX32(0);
    boss.base.velY = FIX32(0);

    boss.base.health = 200;
    boss.base.maxHealth = 200;

    boss.base.currentState = BOSS_STATE_IDLE;
    boss.base.onGround = FALSE;
    boss.base.facingRight = TRUE;

    boss.currentPhase = 0;
    boss.attackTimer = 0;

    boss.base.sprite = bossSprite;
}

void bossUpdate(void) {
    if (boss.base.currentState == BOSS_STATE_DEAD) {
        return;
    }

    entityApplyPhysics(&boss.base);
    entityKeepInBounds(&boss.base);

    if (boss.base.currentState != BOSS_STATE_HURT) {
        boss.base.currentState = boss.base.onGround ? BOSS_STATE_IDLE : BOSS_STATE_FALLING;
    }

    if (boss.base.sprite) {
        s16 spriteX = boss.base.posX >> 16;
        s16 spriteY = boss.base.posY >> 16;
        SPR_setPosition(boss.base.sprite, spriteX, spriteY);
        SPR_setHFlip(boss.base.sprite, !boss.base.facingRight);
    }
}

void bossTakeDamage(u16 amount) {
    bool died = entityTakeDamage(&boss.base, amount);
    boss.base.currentState = died ? BOSS_STATE_DEAD : BOSS_STATE_HURT;
}
```

- [ ] **Step 3: Build to verify it compiles**

Run: `cd "D:/RetroProjects/Genesis Projects/genesis-project-1" && GDK=D:/RetroProjects/SGDK "$GDK/bin/make" -f "$GDK/makefile.gen" -j1 release`
Expected: build fails with an undefined-reference/unresolved-symbol error for `bossSprite` (same forward-reference situation as Task 3's `enemySprite`) — confirm no errors reference `Boss`/`bossInit`/`bossUpdate`/`bossTakeDamage` or `Entity`

- [ ] **Step 4: Commit**

```bash
git add inc/entities/boss.h src/entities/boss.c
git commit -m "Add Boss entity with shared Entity base"
```

---

## Task 5: Placeholder sprites for Enemy and Boss

**Files:**
- Modify: `inc/assetLoader.h`
- Modify: `src/assetLoader.c`

**Interfaces:**
- Consumes: `pSprite` (from generated `res/resources.h`, existing resource — the only sprite resource in the project); `TILE_USER_INDEX`, `SPR_addSprite`, `PAL_setPalette`, `PAL3` (SGDK)
- Produces: `extern Sprite *enemySprite;`, `extern Sprite *bossSprite;`, `void loadEnemyAssets(void);`, `void loadBossAssets(void);` — consumed by Task 3 (`enemy.c`), Task 4 (`boss.c`), and this task's own `initializeAssets()`

This resolves the `enemySprite`/`bossSprite` undefined-reference errors from Tasks 3 and 4. No new art assets are added — enemy/boss reuse the existing `pSprite` sprite definition (`res/playerSprite.png`) on separate palette slots, per the spec's "no new art required" scope.

- [ ] **Step 1: Modify `inc/assetLoader.h`**

```c
#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#include <genesis.h>

// Declare the global variables that will be used across files.
// Use 'extern' to indicate that they are defined 
extern u16 ind;
extern int scrollBackground_offset;
extern int scrollForeground_offset;
extern Sprite *playerSprite;
extern Sprite *enemySprite;
extern Sprite *bossSprite;
 
// Declare the functions...
void initializeAssets();
void loadPlayerAssets();
void loadEnemyAssets();
void loadBossAssets();
void loadLevelAssets();
void updateBackgroundScroll();

#endif // ASSET_LOADER_H
```

- [ ] **Step 2: Modify `src/assetLoader.c`**

```c
#include <genesis.h>
#include <resources.h>
#include "entities/player.h"

void loadPlayerAssets();
void loadEnemyAssets();
void loadBossAssets();
void loadLevelAssets();
void updateBackgroundScroll();
void initializeAssets();

//Level Design Assets
u16 ind = TILE_USER_INDEX;
int scrollBackground_offset = 0;
int scrollForeground_offset = 0;
Sprite *playerSprite;
Sprite *enemySprite;
Sprite *bossSprite;


void initializeAssets()
{
    SPR_init();
    loadPlayerAssets();
    loadEnemyAssets();
    loadBossAssets();
    loadLevelAssets();
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
}

void loadPlayerAssets()
{
    PAL_setPalette(PAL2, pSprite.palette->data, DMA);
    playerSprite = SPR_addSprite(
                        &pSprite,
                        160,  // Just a reasonable default
                        180,  // playerInit() will override this anyway
                    TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
}

void loadEnemyAssets()
{
    // Placeholder: reuse player sprite art on PAL3 until enemy art exists
    PAL_setPalette(PAL3, pSprite.palette->data, DMA);
    enemySprite = SPR_addSprite(
                        &pSprite,
                        220,  // enemyInit() will override this anyway
                        100,
                    TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
}

void loadBossAssets()
{
    // Placeholder: reuse player sprite art on PAL3 until boss art exists
    PAL_setPalette(PAL3, pSprite.palette->data, DMA);
    bossSprite = SPR_addSprite(
                        &pSprite,
                        280,  // bossInit() will override this anyway
                        100,
                    TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
}

void loadLevelAssets()
{
    // Background B - Use VDP_drawImageEx for Image resources
    PAL_setPalette(PAL0, background.palette->data, DMA);
    VDP_drawImageEx(BG_B,
                    &background,
                    TILE_ATTR_FULL(PAL0,
                        FALSE,
                        FALSE,
                        FALSE,
                        ind),
                    0, 0,
                    FALSE,
                    TRUE); // <- This extra argument is for a bitmap, not for a tilemap
    ind += background.tileset->numTile;
    
    // Background A - Your original code was correct here
    PAL_setPalette(PAL1, foreground.palette->data, DMA);
    VDP_drawImageEx(BG_A,
                    &foreground,
                    TILE_ATTR_FULL(PAL1,
                        FALSE,
                        FALSE,
                        FALSE,
                        ind),
                    0, 0,
                    FALSE,
                    TRUE);
    ind +=foreground.tileset->numTile;
}

void updateBackgroundScroll()
{
    scrollBackground_offset -=1;
    scrollForeground_offset -=2;
    VDP_setHorizontalScroll(BG_A, scrollForeground_offset);
    VDP_setHorizontalScroll(BG_B, scrollBackground_offset);
}
```

- [ ] **Step 3: Build to verify it compiles**

Run: `cd "D:/RetroProjects/Genesis Projects/genesis-project-1" && GDK=D:/RetroProjects/SGDK "$GDK/bin/make" -f "$GDK/makefile.gen" -j1 release`
Expected: build succeeds with no errors — `enemySprite`/`bossSprite` are now defined, resolving Task 3/4's forward references. `enemy.c`/`boss.c` reference `enemyInit`/`bossInit`/etc. but nothing calls them yet (Task 6), so no new undefined-reference errors should appear; unused-function warnings are acceptable at this point if the compiler emits them (they'll disappear once Task 6 wires the calls in)

- [ ] **Step 4: Commit**

```bash
git add inc/assetLoader.h src/assetLoader.c
git commit -m "Add placeholder sprite loading for Enemy and Boss"
```

---

## Task 6: Wire Enemy and Boss into the main loop

**Files:**
- Modify: `src/core/game.c`

**Interfaces:**
- Consumes: `enemyInit`, `enemyUpdate` from Task 3 (`inc/entities/enemy.h`); `bossInit`, `bossUpdate` from Task 4 (`inc/entities/boss.h`)
- Produces: nothing new consumed by later tasks — this is the final integration point

- [ ] **Step 1: Modify `src/core/game.c`**

Add the two new includes at the top (after the existing `#include "entities/player.h"` line):

```c
#include "entities/enemy.h"
#include "entities/boss.h"
```

In `gameInit()`, add calls right after `playerInit();`:

```c
    // Initialize player
    playerInit();

    // Initialize enemy and boss
    enemyInit();
    bossInit();
```

In `gameUpdate()`'s `GAME_STATE_PLAYING` case, add calls right after `playerUpdate();`:

```c
        case GAME_STATE_PLAYING:
            // Main gameplay update
            inputUpdate();
            playerUpdate();
            enemyUpdate();
            bossUpdate();
            hudUpdate();
            hudRender();
            // Camera update will be called from main
            break;
```

The full resulting `src/core/game.c` should read:

```c
#include <genesis.h>
#include "core/game.h"
#include "core/config.h"
#include "systems/input.h"
#include "entities/player.h"
#include "entities/enemy.h"
#include "entities/boss.h"
#include "assetLoader.h"
#include "world/zone.h"
#include "ui/hud.h"
#include "gameplay/stats.h"

// Global game state
GameState currentGameState = GAME_STATE_TITLE;

void gameInit() {
    // Initialize SGDK systems
    SPR_init();
    
    // Initialize input system
    inputInit();
    
    // Initialize zone system
    zoneInit();
    
    // Initialize stats
    statsInit();
    
    // Initialize assets
    initializeAssets();
    
    // Initialize player
    playerInit();

    // Initialize enemy and boss
    enemyInit();
    bossInit();
    
    // Initialize HUD
    hudInit();
    
    // Start in playing state (skip title for now)
    gameChangeState(GAME_STATE_PLAYING);
}

void gameUpdate() {
    switch (currentGameState) {
        case GAME_STATE_TITLE:
            // Title screen logic (to be implemented)
            // For now, just transition to playing on button press
            if (JOY_readJoypad(JOY_1) & BUTTON_START) {
                gameChangeState(GAME_STATE_PLAYING);
            }
            break;
            
        case GAME_STATE_PLAYING:
            // Main gameplay update
            inputUpdate();
            playerUpdate();
            enemyUpdate();
            bossUpdate();
            hudUpdate();
            hudRender();
            // Camera update will be called from main
            break;
            
        case GAME_STATE_PAUSED:
            // Pause menu logic
            if (JOY_readJoypad(JOY_1) & BUTTON_START) {
                gameChangeState(GAME_STATE_PLAYING);
            }
            break;
            
        case GAME_STATE_GAME_OVER:
            // Game over screen logic
            break;
            
        case GAME_STATE_TRANSITION:
            // Zone transition logic
            break;
            
        default:
            break;
    }
}

void gameChangeState(GameState newState) {
    // GameState oldState = currentGameState;  // Reserved for future state transition logic
    currentGameState = newState;
    
    // Handle state entry logic
    switch (newState) {
        case GAME_STATE_TITLE:
            // Reset game
            break;
            
        case GAME_STATE_PLAYING:
            // Resume or start gameplay
            break;
            
        case GAME_STATE_PAUSED:
            // Pause display
            break;
            
        case GAME_STATE_GAME_OVER:
            // Game over display
            break;
            
        case GAME_STATE_TRANSITION:
            // Zone transition effect
            break;
            
        default:
            break;
    }
}

GameState gameGetState() {
    return currentGameState;
}
```

- [ ] **Step 2: Build to verify it compiles**

Run: `cd "D:/RetroProjects/Genesis Projects/genesis-project-1" && GDK=D:/RetroProjects/SGDK "$GDK/bin/make" -f "$GDK/makefile.gen" -j1 release`
Expected: build succeeds with no errors and produces `out/rom.bin`

- [ ] **Step 3: Manual emulator verification**

Run the produced ROM (`out/rom.bin`) in an emulator (Gens/BlastEm — `Gens.cfg` at the project root already points `Rom path` at `D:\RetroProjects\`).
Expected, per the spec's verification checklist:
- Player behaves identically to before the refactor (movement, jump, dash, parry, attack, state transitions)
- Enemy placeholder sprite appears near the right side of the screen, falls under gravity, stays within screen bounds
- Boss placeholder sprite appears further right, falls under gravity, stays within screen bounds

- [ ] **Step 4: Commit**

```bash
git add src/core/game.c
git commit -m "Wire Enemy and Boss into game init/update loop"
```

---

## Self-Review Notes

- **Spec coverage:** `Entity` base (Task 1), Player refactor (Task 2), Enemy fix (Task 3), Boss addition (Task 4), asset loading (Task 5), main-loop integration (Task 6), manual verification checklist (Task 6 Step 3) — all spec sections have a corresponding task.
- **Placeholder scan:** no TBD/TODO markers; every step has full code.
- **Type consistency checked:** `Entity`, `entityApplyPhysics`, `entityTakeDamage`, `entityKeepInBounds` signatures match between Task 1's declaration and every later task's usage. `enemySprite`/`bossSprite` names match between Task 5's declaration and Tasks 3/4's usage. `enemyInit`/`enemyUpdate`/`bossInit`/`bossUpdate` names match between Tasks 3/4's declarations and Task 6's usage.
