# PSP-ECS
Testing out raylib4psp under an ECS programming paradigm

## Overview
This is a basic raylib project for the PlayStation Portable (PSP) that demonstrates:
- Entity Component System (ECS) architecture
- Menu system with start and options screens
- Re-mappable keybinding system
- 3D test scene with player-controllable camera

## Features

### Entity Component System (ECS)
- Component-based architecture inspired by [simple_ecs](https://github.com/raylib-extras/simple_ecs)
- Support for Transform, Renderable, Camera, and Input components
- Flexible entity management with up to 256 entities
- System-based updates (Input, Camera, Render)

### Menu System
- Press START to toggle the main menu
- Main menu with "Start Game" and "Options"
- Options menu for future keybinding configuration
- Fully navigable with PSP controls

### Keybinding System
- Re-mappable controls for all game actions
- Default PSP control scheme:
  - D-Pad: Camera movement (Forward/Back/Left/Right)
  - Triangle: Move camera up
  - Cross (X): Move camera down / Menu select
  - Circle: Menu back
  - START: Toggle menu
  - Analog stick: Camera rotation
- Extensible for future key remapping UI

### Test Scene
- Gray background (RGB: 100, 100, 100)
- White grid on the ground (20x20)
- White cube at the center (2x2x2 units)
- Directional lighting (via raylib's default lighting)
- Player-controllable camera using ECS

## Project Structure
```
PSP-ECS/
├── src/
│   ├── main.c          # Main application entry point
│   ├── ecs.c           # ECS implementation
│   ├── menu.c          # Menu system
│   ├── keybinds.c      # Keybinding system
│   ├── scene.c         # Scene management
│   └── camera.c        # Camera controls
├── include/
│   ├── ecs.h           # ECS header
│   ├── menu.h          # Menu header
│   ├── keybinds.h      # Keybindings header
│   ├── scene.h         # Scene header
│   └── camera.h        # Camera header
├── assets/             # (Future) Asset directory
├── Makefile            # PSP build configuration
└── README.md           # This file
```

## Building

### Prerequisites
- [PSP Toolchain](https://github.com/pspdev/psptoolchain)
- [raylib4Psp](https://github.com/raylib4Consoles/raylib4Psp)

### Build Instructions
1. Install the PSP toolchain and set up the environment
2. Install raylib4Psp to your PSP SDK
3. Clone this repository:
   ```bash
   git clone https://github.com/astearon/PSP-ECS.git
   cd PSP-ECS
   ```
4. Build the project:
   ```bash
   make
   ```
5. The output will be `EBOOT.PBP` in the project directory
6. Copy `EBOOT.PBP` to `ms0:/PSP/GAME/PSP-ECS/` on your PSP

## Controls

### In-Game
- **D-Pad Up**: Move camera forward
- **D-Pad Down**: Move camera backward
- **D-Pad Left**: Move camera left
- **D-Pad Right**: Move camera right
- **Triangle**: Move camera up
- **Cross (X)**: Move camera down
- **Analog Stick**: Rotate camera
- **START**: Toggle menu

### Menu
- **D-Pad Up/Down**: Navigate menu
- **Cross (X)**: Select menu item
- **Circle**: Go back

## Architecture Details

### ECS Implementation
The ECS system is built with three core concepts:

1. **Entities**: Simple integer IDs that represent game objects
2. **Components**: Data containers that hold specific attributes
   - TransformComponent: Position, rotation, scale
   - RenderableComponent: Visual representation
   - CameraComponent: Camera properties and controls
   - InputComponent: Input handling flag
3. **Systems**: Logic that operates on entities with specific components
   - System_Render: Renders all entities with Transform and Renderable
   - Camera_UpdateControls: Updates camera based on input

### Menu System
- State-based menu management (MENU_NONE, MENU_MAIN, MENU_OPTIONS)
- MenuItem structure with text and action callbacks
- Semi-transparent overlay during menu display

### Keybinding System
- ActionID enum for all game actions
- KeyBinding structure maps actions to PSP buttons
- Support for both pressed and held button states
- Future: Save/load keybindings to memory stick

## Future Enhancements
- [ ] Visual keybinding configuration in options menu
- [ ] Save/load keybindings to memory stick
- [ ] Additional renderable types (sphere, plane)
- [ ] Lighting system configuration
- [ ] Multiple test scenes
- [ ] Entity spawning/destruction at runtime
- [ ] Collision detection
- [ ] Audio system

## License
This project is open source and available under the MIT License.

## Credits
- Built with [raylib4Psp](https://github.com/raylib4Consoles/raylib4Psp)
- ECS architecture inspired by [simple_ecs](https://github.com/raylib-extras/simple_ecs)
- Developed for the PSP homebrew community
