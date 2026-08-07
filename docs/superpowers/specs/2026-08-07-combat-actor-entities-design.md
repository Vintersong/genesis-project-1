# Combat Actor Entities: Shared Base, Enemy, Boss

**Date:** 2026-08-07
**Status:** Approved for implementation

## Purpose

The project currently has one working combat actor (`Player`, in `src/entities/player.c`) and one broken, empty stub (`inc/entities/enemy.h` / `src/entities/enemy.c` — missing include guard, empty struct, no implementation). This spec finishes `Enemy` and adds `Boss`, while introducing a shared `Entity` base struct so the three types (and future ones, see Forward Compatibility) don't duplicate position/velocity/health/state/collision/sprite fields and the physics/bounds logic that operates on them.

This spec is scoped to **combat actors** (things with a state machine and physics). Inventory items (Weapons, Armors, Consumables) are a separate concern and will get their own spec later.

## Out of Scope

- Weapons, Armors, Consumables (separate spec)
- Enemy AI/decision-making (patrol, chase, perception) — Enemy gets a minimal stub only
- Boss phase/attack-pattern logic — fields are reserved on the struct, but logic is a stub
- Combat/hit-detection wiring (player attack hitbox vs. enemy/boss hitbox, damage application, knockback, death/loot triggers)
- Multiple simultaneous enemies (array/pool of enemies) — single static instance only, matching the current `Player`/`Enemy` pattern
- Projectiles (see Forward Compatibility — the `Entity` base is designed to support them later, but no `Projectile` type is added in this spec)

## Architecture

### Shared `Entity` base struct

New files: `inc/core/entity.h`, `src/core/entity.c`.

```c
typedef struct {
    fix32 posX, posY, velX, velY;
    u16 health, maxHealth;
    u8 currentState;        // per-type enum (PlayerState/EnemyState/BossState) stored as u8, cast at use sites
    bool onGround, facingRight;
    CollisionBox hitbox;
    Sprite* sprite;
} Entity;
```

`Player`, `Enemy`, and `Boss` each embed this as their first member (`Entity base;`). Because it's the first member, a pointer to any of these structs is also a valid `Entity*` (standard C struct-layout guarantee), so shared helper functions can operate on any actor type without casting boilerplate at call sites beyond `&thing.base`.

Shared helper functions (`src/core/entity.c`):

- `void entityApplyPhysics(Entity* e)` — replaces the inlined gravity/friction/ground-check/integrate-velocity sequence currently duplicated in `playerUpdate()`. Internally calls the existing `applyGravity`, `applyFriction`, `integrateVelocity`, `checkGroundCollision` functions from `systems/physics.h` and `systems/collision.h` (unchanged — they already take raw `fix32*` pointers, not `Player*`, so no changes needed there).
- `bool entityTakeDamage(Entity* e, u16 amount)` — subtracts `amount` from `health`, clamps at 0, returns `TRUE` if health reached 0 (caller decides what "died" means for that type).
- `void entityKeepInBounds(Entity* e)` — replaces the inlined screen-clamp logic currently in `playerUpdate()`.

### `Player` refactor

`inc/entities/player.h`: `Player` struct changes from flat fields to:

```c
typedef struct {
    Entity base;
    // stamina, dash, double-jump, parry fields (unchanged names/types)
    u16 stamina, maxStamina;
    bool canDash, hasDashAbility;
    u16 dashCooldown, dashTimer;
    bool canDoubleJump, hasDoubleJumpAbility;
    u16 parryWindow;
    bool hasParryAbility;
} Player;
```

`PlayerState` enum is unchanged (it has members, like `PLAYER_STATE_PARRYING`, that don't apply to other actor types, so it stays a separate enum from `EnemyState`/`BossState` — only the storage slot in `Entity.currentState` is shared).

All field accesses in `src/entities/player.c` (e.g. `player.posX`) become `player.base.posX`. `playerUpdate()` is shortened by replacing its inlined physics/bounds-clamp blocks with calls to `entityApplyPhysics(&player.base)` and `entityKeepInBounds(&player.base)`. This is a mechanical refactor — no behavior change.

### `Enemy`

Fixes `inc/entities/enemy.h` (currently missing its `#ifndef` guard line, has an empty struct, and `enemyInit()` has no implementation).

```c
#ifndef ENEMY_H
#define ENEMY_H

#include <genesis.h>
#include "core/entity.h"
#include "core/config.h"

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

void enemyInit(void);
void enemyUpdate(void);
void enemyTakeDamage(u16 amount);

#endif // ENEMY_H
```

`enemyUpdate()` calls `entityApplyPhysics(&enemy.base)` and `entityKeepInBounds(&enemy.base)`. No decision-making/AI logic — enemy sits under gravity like a physics object, matching the "minimal stub" scope. `enemyTakeDamage()` wraps `entityTakeDamage(&enemy.base, amount)` and transitions `base.currentState` to `ENEMY_STATE_HURT` or `ENEMY_STATE_DEAD`.

### `Boss`

New files: `inc/entities/boss.h`, `src/entities/boss.c`.

```c
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
    u8 currentPhase;    // reserved for future phase logic; always 0 in this spec
    u16 attackTimer;    // reserved for future attack-pattern logic; unused in this spec
} Boss;

extern Boss boss;

void bossInit(void);
void bossUpdate(void);
void bossTakeDamage(u16 amount);
```

Same treatment as Enemy: `bossUpdate()` applies physics/bounds only, no phase or attack logic. `currentPhase`/`attackTimer` exist on the struct now specifically so a future "boss mechanics" spec can add behavior without changing the struct shape or touching `Player`/`Enemy` code.

### Asset loading & main loop integration

- `src/assetLoader.c`: add `loadEnemyAssets()` and `loadBossAssets()` (placeholder sprites, following the existing `loadPlayerAssets()` pattern), called from `initializeAssets()`.
- `src/main.c` / `src/core/game.c`: add `enemyInit()`/`bossInit()` alongside `playerInit()`, and `enemyUpdate()`/`bossUpdate()` alongside `playerUpdate()` in the frame loop.
- No Makefile changes — `src/core/entity.c` and `src/entities/boss.c` are covered by the existing wildcard `SRC`/`INC` patterns described in `PROJECT_INITIALIZATION_PROMPT.md`.

## Forward Compatibility (not built in this spec)

- **Projectiles**: will embed `Entity base;` the same way Player/Enemy/Boss do (e.g. `typedef struct { Entity base; u8 damage; u8 ownerType; } Projectile;`). The `Entity` struct's fields (position, velocity, collision box, no ability-specific state) were chosen to support this without rework.
- **Per-enemy-species attacks**: `Enemy` can grow an `attackPattern` field or reference a shared stats/behavior-template table later, without affecting `Player` or `Boss`.
- **Boss phases/attack patterns**: `currentPhase`/`attackTimer` are reserved for a future `bossHandlePhase()` that switches on phase — additive, no struct rework.
- **Multiple enemies**: converting the single static `Enemy enemy;` to `Enemy enemies[MAX_ENEMIES];` is a follow-up spec once spawning/zone triggers are designed.

## Verification

No unit test framework applies (SGDK/68000 target, no host-side test runner). Verification is a manual checklist:

- [ ] Project compiles via SGDK makefile with no errors or new warnings
- [ ] ROM boots in emulator (Gens/BlastEm)
- [ ] Player behaves identically to before the refactor (movement, jump, dash, parry, attack, state transitions)
- [ ] Enemy placeholder sprite appears, falls under gravity, stays in bounds
- [ ] Boss placeholder sprite appears, falls under gravity, stays in bounds
- [ ] `enemyTakeDamage()` / `bossTakeDamage()` transition state to HURT/DEAD when called manually (e.g. temporary debug call) since no combat wiring exists yet to trigger them in normal play
