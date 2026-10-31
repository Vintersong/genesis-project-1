# Handoff notes (2026-10-31)

Branch: `fix/fix32-conversion` (2 commits on top of `master`/default). Built and tested: **NO** (no SGDK toolchain in the cloud session).

## Done
- Replaced every `fixVal >> 16` with `F32_toInt(fixVal)` in `collision.c`, `player.c`, `enemy.c`, `boss.c`, and `hud.c` (was `fix32ToInt`, renamed for consistency with `camera.c`).
- Reason: SGDK `fix32` is 22.10 (10 fractional bits), confirmed against SGDK master `inc/types.h`. `>> 16` put sprites at ~1/64 of their real position and made the ground check at y=180 almost never trigger.

## First thing to do on desktop
1. Build: `$GDK/bin/make -f $GDK/makefile.gen -j1 release`, run `out/rom.bin`.
2. Check the player/enemy/boss sprites now sit where the HUD's X/Y debug text says, and that the player lands on the ground at y=180.
3. Check `FIX32_FRAC_BITS` in your installed SGDK `types.h` is 10.

## Still open
- `src/camera.c`: `FIX32(MAP_WIDTH) - PLAYER_WIDTH` subtracts an unscaled int from a fix32. Should be `FIX32(PLAYER_WIDTH)` (same for HEIGHT). `MAP_WIDTH`/`MAP_HEIGHT` are placeholders (100), so the player gets clamped to x<=100px.
- `checkGroundCollision(posY)` treats posY as the sprite top, not the feet, so entities sink into the floor by sprite height. It takes only Y, so no tile collision yet.
- `entityKeepInBounds` clamps to the 320px screen, but the camera works with a larger map.
- `enemyUpdate` / `bossUpdate` are near-duplicates: extract a shared sprite-sync + state helper.
- Enemy/Boss never leave HURT once hit (deliberately deferred to the combat task), `facingRight` never changes, `hitbox`, `currentPhase` and `attackTimer` are unused.

## Next ideas
- Basic enemy AI (patrol, chase, contact damage) on the existing state machine.
- Hook up hitboxes / combat (see `docs/superpowers/plans/2026-08-07-combat-actor-entities.md`).
- Design reference: Castlevania SotN as inspiration for systems (ability-gated progression, inverted map, RPG layer). Note: Claude Code has a rule against recreating a specific existing game, so describe your own game's mechanics rather than "copy SotN's rooms/bosses".
