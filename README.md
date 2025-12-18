# Genesis Metroidvania

A Metroidvania-style platformer for Sega Genesis/Mega Drive, built with SGDK.

## About

This is a work-in-progress action platformer featuring unlockable abilities, smooth physics, and modular zone-based design.

**Core Features:**
- Player controller with state machine
- Fixed-point physics (sub-pixel precision)
- Unlockable abilities: dash, double jump, parry
- Camera system with smooth following
- Health and stamina management

## Status

🚧 **Early Development** - Core systems implemented, gameplay content in progress

## Building

Requires [SGDK](https://github.com/Stephane-D/SGDK) installed.

```bash
export GDK=D:/RetroProjects/SGDK
$GDK/bin/make -f $GDK/makefile.gen -j1 release
```

ROM output: `out/rom.bin`

## Controls

- **D-Pad**: Move
- **B**: Jump / Double Jump
- **A**: Dash (unlockable)
- **C**: Parry (unlockable)

## Tech

- Language: C
- Target: Sega Genesis/Mega Drive (68000)
- Framework: SGDK
- Architecture: Modular systems (physics, collision, animation, input)

## Credits

- **SGDK**: Stephane Dallongeville
- **Development**: Vintersong

---

*Built with SGDK for Sega Genesis/Mega Drive*
