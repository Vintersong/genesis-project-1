# Genesis Metroidvania Project

A Metroidvania-style action platformer for the Sega Genesis/Mega Drive, built with SGDK (Sega Genesis Development Kit).

## 🎮 Features

### Player Mechanics
- **Smooth Movement**: Fixed-point physics with sub-pixel precision
- **State Machine**: Comprehensive player states (idle, running, jumping, falling, dashing, attacking, parrying)
- **Unlockable Abilities**:
  - Dash with cooldown system
  - Double jump
  - Parry mechanic with precise timing window

### Systems
- **Physics Engine**: Custom gravity, friction, and velocity integration
- **Collision Detection**: Ground detection and boundary checking
- **Camera System**: Smooth following camera with dead zones
- **Animation System**: Frame-based sprite animation with loop support
- **Input Handling**: Button press events and continuous D-pad input
- **HUD**: Health and stamina display
- **Stats System**: Player health and stamina management

### Architecture
- **Modular Design**: Separated into core, systems, entities, gameplay, UI, and world modules
- **Zone-Based Levels**: Multiple themed zones (CPU, GPU, RAM, Storage, Hub, BIOS)
- **Game State Manager**: Title, playing, paused, game over, and transition states

## 🛠️ Building

### Prerequisites
- [SGDK](https://github.com/Stephane-D/SGDK) installed at `D:/RetroProjects/SGDK`
- GCC toolchain (included with SGDK)

### Compile
```bash
# Set SGDK environment variable
export GDK=D:/RetroProjects/SGDK

# Clean build
$GDK/bin/make -f $GDK/makefile.gen clean

# Build release ROM
$GDK/bin/make -f $GDK/makefile.gen -j1 release
```

The compiled ROM will be at `out/rom.bin`.

## 🕹️ Controls

- **D-Pad**: Move left/right
- **B Button**: Jump (press again in air for double jump when unlocked)
- **A Button**: Dash (when unlocked)
- **C Button**: Parry (when unlocked)
- **X/Y/Z**: Attack (6-button controller)

## 📁 Project Structure

```
├── inc/                    # Header files
│   ├── core/              # Core game systems
│   ├── entities/          # Player and entity definitions
│   ├── gameplay/          # Gameplay mechanics
│   ├── systems/           # Reusable systems (physics, collision, etc.)
│   ├── ui/                # HUD and UI elements
│   └── world/             # Level/zone definitions
├── src/                   # Source files (mirrors inc/ structure)
├── res/                   # Resources (sprites, tilemaps, music)
└── out/                   # Build output (ignored by git)
```

## 🎯 Development Status

**Current Phase**: Foundation & Core Systems

### ✅ Completed
- [x] Player controller with state machine
- [x] Physics system (gravity, friction, velocity)
- [x] Basic collision detection
- [x] Camera system with smooth following
- [x] Animation framework
- [x] Input handling (button events + continuous)
- [x] Ability system framework (dash, double jump, parry)
- [x] Stats system (health, stamina)
- [x] HUD rendering
- [x] Game state management

### 🚧 In Progress
- [ ] Level geometry collision
- [ ] Tilemap integration
- [ ] Animation frame switching
- [ ] Enemy AI
- [ ] Combat system
- [ ] Ability unlock progression
- [ ] Save/load system

### 📋 Planned
- [ ] Multiple zone implementations
- [ ] Boss battles
- [ ] Power-ups and collectibles
- [ ] Sound effects and music
- [ ] Title screen and menus
- [ ] Particle effects

## 🧰 Technical Details

- **Language**: C (ANSI C with SGDK extensions)
- **Target Hardware**: Sega Genesis/Mega Drive (Motorola 68000)
- **Fixed-Point Math**: 16.16 format for smooth sub-pixel movement
- **Memory Management**: ROM-based definitions, RAM for runtime state
- **Frame Rate**: 60 FPS (NTSC) / 50 FPS (PAL)

## 📝 Design Philosophy

This project emphasizes:
- **Metroidvania Progression**: Abilities unlock new areas and movement options
- **Tight Controls**: Precise input handling with state validation
- **Modular Architecture**: Each system is self-contained and reusable
- **Performance**: Optimized for 16-bit hardware constraints

## 🤝 Contributing

This is a personal learning project, but feedback and suggestions are welcome! Feel free to open issues or discuss ideas.

## 📄 License

[Add your license here - MIT, GPL, etc.]

## 🙏 Credits

- **SGDK**: Stephane Dallongeville's Sega Genesis Development Kit
- **Development**: [Your Name/Handle]

---

*Built with SGDK for the Sega Genesis/Mega Drive*
