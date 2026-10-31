# Handoff notes (2026-10-31)

Branch: `development`. Last commit: `a1dfa15`. Builds clean (no errors or warnings) with `$GDK/bin/make -f $GDK/makefile.gen -j1 release` (`GDK=D:\RetroProjects\SGDK`). **Not yet run/played**, so everything below is untested in the emulator.

## Done
- `fix32` -> int conversions use `F32_toInt` everywhere (fix32 is 22.10, not 16.16).
- Ground collision is tile-based: `checkGroundCollision(posX, posY)` probes the pixel row below the feet (`posY + PLAYER_HITBOX_SIZE`) at both bottom corners via `tilemapIsSolidAtPixel`. `entityApplyPhysics` snaps the feet to the tile top on landing. The old hardcoded `GROUND_Y` is gone.
- `entityKeepInBounds` clamps to the map width (`TILEMAP_WIDTH_TILES * TILE_DIMENSION`), not the screen.
- `camera.c` clamps use `FIX32(MAP_WIDTH - PLAYER_WIDTH)` (and HEIGHT).
- Sprites are drawn in screen space: `entitySyncSprite()` (`src/core/entity.c`) subtracts the camera and applies the flip. It is called from `main.c` right after `mainCamera()` so sprites use the final camera for the frame. The per-entity sprite code was removed from the player/enemy/boss update functions.
- Camera smoothing works: eases 1/4 of the remaining distance per frame (min 1px). The parallax scroll for BG_B is now a signed `s16` (it was a `u8` that wrapped).

## First thing to do next session
1. Run `out/rom.bin`. Walk the whole 640px level and check: sprite aligned with the ground, landing on platforms, double jump / dash pits, camera follows without jitter, background parallax doesn't jump.
2. If the camera feels laggy, change `>> 2` to `>> 1` in `src/camera.c`.
3. The HUD X/Y debug text shows world coordinates; decide if that's what you want.

## Still open
- No horizontal (wall) collision, only the floor. Ceiling collision is also missing.
- `enemyUpdate` / `bossUpdate` are near-duplicates: extract a shared state helper.
- Enemy/Boss never leave HURT once hit (deliberately deferred to the combat task), `facingRight` never changes for them, `hitbox`, `currentPhase` and `attackTimer` are unused.
- Collision grid is hardcoded in `src/world/tilemap.c` (`buildPlaceholderCollisionGrid`) and must be kept in sync by hand with `res/level.png`.

## Art / level pipeline (researched, nothing set up yet)
- Aseprite is compiled at `D:\Graphical Software\Design Software\Aseprite\aseprite.exe`. [pixel-mcp](https://pkg.go.dev/github.com/willibrandon/pixel-mcp) can drive it (palette snapping, spritesheet export) if wanted.
- Python 3.13 + Pillow are installed, so tilesets and placeholder art can be generated as code with a fixed 16-colour palette.
- Hardware limits to design around: 4bpp, 16 colours per palette (index 0 transparent), 4 palettes, sprites built from 8x8 tiles (max 32x32 per hardware sprite).
- `playerSprite.png` is currently 48x48 (declared 6x6 tiles in `resources.res`) while the hitbox is 16x16. Frame size and hitbox are separate decisions still to make: frame size, first animations (idle/run/jump/fall/dash), art style.
- Recommended level workflow: Tiled with a graphics layer, a collision layer and an object layer (spawns, doors), plus a small Python exporter that generates the map/tileset and a `const u8 collision[]` C file so graphics and collision can't drift. LDtk is the alternative.

## Next ideas
- Basic enemy AI (patrol, chase, contact damage) on the existing state machine.
- Hook up hitboxes / combat (see `docs/superpowers/plans/2026-08-07-combat-actor-entities.md`).
- Design reference: ability-gated progression, inverted map, RPG layer. Describe your own game's mechanics rather than copying another game's rooms or bosses.
