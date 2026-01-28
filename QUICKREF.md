# PSP-ECS Quick Reference

## File Structure
```
PSP-ECS/
├── src/
│   ├── main.c       - Main application entry, game loop
│   ├── ecs.c        - ECS implementation
│   ├── menu.c       - Menu system
│   ├── keybinds.c   - Keybinding system  
│   ├── scene.c      - Scene management
│   └── camera.c     - Camera controls
├── include/
│   ├── ecs.h        - ECS header
│   ├── menu.h       - Menu header
│   ├── keybinds.h   - Keybindings header
│   ├── scene.h      - Scene header
│   └── camera.h     - Camera header
├── Makefile         - PSP build configuration
├── README.md        - Project overview
├── BUILD.md         - Build instructions
├── ARCHITECTURE.md  - Architecture documentation
└── CONTROLS.md      - Control reference
```

## Quick Build
```bash
make              # Build the project
make clean        # Clean build artifacts
```

## Key Features

### ✅ Entity Component System (ECS)
- Max 256 entities
- 4 component types: Transform, Renderable, Camera, Input
- Component-based architecture
- System-based updates

### ✅ Menu System
- Press START to toggle menu
- Main menu: Start Game, Options
- Options menu: Keybindings (future), Back
- Semi-transparent overlay

### ✅ Keybinding System
- 11 remappable actions
- Default PSP control scheme
- Press and hold detection
- Save/load support (stubs)

### ✅ Test Scene
- Gray background (RGB: 100, 100, 100)
- 20x20 white grid on ground
- 2x2x2 white cube at center
- Player-controllable camera
- Analog stick dead zone

## Quick Controls

| Action | Button |
|--------|--------|
| Move Camera | D-Pad |
| Rotate Camera | Analog Stick |
| Move Up | Triangle (△) |
| Move Down | Cross (✕) |
| Menu | START |
| Select | Cross (✕) |
| Back | Circle (○) |

## Code Examples

### Creating an Entity
```c
EntityID entity = ECS_CreateEntity(&world);
TransformComponent* transform = ECS_AddComponent(&world, entity, COMPONENT_TRANSFORM);
RenderableComponent* renderable = ECS_AddComponent(&world, entity, COMPONENT_RENDERABLE);
```

### Checking Button State
```c
SceCtrlData pad;
sceCtrlReadBufferPositive(&pad, 1);

if (Keybinds_IsActionDown(&keybinds, ACTION_MOVE_FORWARD, &pad)) {
    // Button is held down
}
```

### Detecting Button Press
```c
static SceCtrlData oldPad = {0};
SceCtrlData pad;
sceCtrlReadBufferPositive(&pad, 1);

unsigned int button = Keybinds_GetBinding(&keybinds, ACTION_JUMP);
if ((pad.Buttons & button) && !(oldPad.Buttons & button)) {
    // Button was just pressed
}

oldPad = pad;
```

## Important Constants

```c
#define MAX_ENTITIES 256
#define MAX_COMPONENTS 8
#define COMPONENT_COUNT 4
```

## Component Types

| ID | Type | Description |
|----|------|-------------|
| 0 | COMPONENT_TRANSFORM | Position, rotation, scale |
| 1 | COMPONENT_RENDERABLE | Visual representation |
| 2 | COMPONENT_CAMERA | Camera properties |
| 3 | COMPONENT_INPUT | Input handling flag |

## Renderable Types

| Type | Description |
|------|-------------|
| RENDERABLE_CUBE | 3D cube with wireframe |
| RENDERABLE_SPHERE | 3D sphere (not implemented) |
| RENDERABLE_PLANE | 3D plane (not implemented) |
| RENDERABLE_GRID | Ground grid (20x20) |

## Menu States

| State | Description |
|-------|-------------|
| MENU_NONE | No menu (game active) |
| MENU_MAIN | Main menu |
| MENU_OPTIONS | Options menu |

## Action IDs

```c
ACTION_MOVE_FORWARD    // D-Pad Up
ACTION_MOVE_BACKWARD   // D-Pad Down
ACTION_MOVE_LEFT       // D-Pad Left
ACTION_MOVE_RIGHT      // D-Pad Right
ACTION_MOVE_UP         // Triangle
ACTION_MOVE_DOWN       // Cross
ACTION_MENU_UP         // D-Pad Up
ACTION_MENU_DOWN       // D-Pad Down
ACTION_MENU_SELECT     // Cross
ACTION_MENU_BACK       // Circle
ACTION_TOGGLE_MENU     // START
```

## Common Tasks

### Add a New Component Type
1. Add to `ComponentType` enum in `include/ecs.h`
2. Create component struct in `include/ecs.h`
3. Add case in `ECS_AddComponent()` in `src/ecs.c`
4. Update `COMPONENT_COUNT`

### Add a New Renderable Type
1. Add to `RenderableType` enum in `include/ecs.h`
2. Add case in `System_Render()` in `src/ecs.c`

### Add a New Action
1. Add to `ActionID` enum in `include/keybinds.h`
2. Add name to `actionNames[]` in `src/keybinds.c`
3. Set default binding in `Keybinds_Init()`
4. Update `ACTION_COUNT`

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Build error: psp-gcc not found | Install PSP toolchain |
| Build error: raylib.h not found | Install raylib4Psp |
| Black screen on launch | Check LIBS in Makefile |
| Controls not working | Check keybinds in keybinds.c |
| Camera drifting | Dead zone is 10 units (adjust if needed) |

## Performance Notes

- Fixed 256 entity limit
- Linear entity iteration (O(n))
- Components heap-allocated
- No dynamic array resizing
- Suitable for PSP's limited RAM

## Security

✅ No unsafe string functions (strcpy, strcat, sprintf, gets)
✅ Safe snprintf with sizeof()
✅ Bounds checking on entity access
✅ NULL checks on component access
✅ malloc failure handling

## Next Steps

See TODO section in README.md for planned features:
- Visual keybinding configuration
- Save/load keybindings
- Additional renderables
- Lighting system
- Audio system

## Resources

- [PSP Dev Wiki](http://ps2dev.org/psp/)
- [raylib Docs](https://www.raylib.com/)
- [simple_ecs](https://github.com/raylib-extras/simple_ecs)
- [raylib4Psp](https://github.com/raylib4Consoles/raylib4Psp)
