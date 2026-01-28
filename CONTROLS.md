# PSP-ECS Controls Reference

This document provides a complete reference of all controls in PSP-ECS.

## Default Control Scheme

### In-Game Controls

#### Camera Movement
- **D-Pad Up**: Move camera forward
- **D-Pad Down**: Move camera backward
- **D-Pad Left**: Move camera left (strafe)
- **D-Pad Right**: Move camera right (strafe)
- **Triangle (△)**: Move camera up
- **Cross (✕)**: Move camera down

#### Camera Rotation
- **Left Analog Stick**: Rotate camera
  - Left/Right: Horizontal rotation (yaw)
  - Up/Down: Vertical rotation (pitch)

#### System Controls
- **START**: Toggle main menu

### Menu Controls

#### Navigation
- **D-Pad Up**: Move selection up
- **D-Pad Down**: Move selection down

#### Actions
- **Cross (✕)**: Select current menu item
- **Circle (○)**: Go back / Cancel (in submenus)
- **START**: Close menu

## Control Remapping

The keybinding system supports remapping controls. Currently, defaults are set in code, but the system is designed for future UI-based remapping.

### Remappable Actions

| Action ID | Default Button | Description |
|-----------|----------------|-------------|
| ACTION_MOVE_FORWARD | D-Pad Up | Move camera forward |
| ACTION_MOVE_BACKWARD | D-Pad Down | Move camera backward |
| ACTION_MOVE_LEFT | D-Pad Left | Move camera left |
| ACTION_MOVE_RIGHT | D-Pad Right | Move camera right |
| ACTION_MOVE_UP | Triangle | Move camera up |
| ACTION_MOVE_DOWN | Cross | Move camera down |
| ACTION_MENU_UP | D-Pad Up | Navigate menu up |
| ACTION_MENU_DOWN | D-Pad Down | Navigate menu down |
| ACTION_MENU_SELECT | Cross | Select menu item |
| ACTION_MENU_BACK | Circle | Go back in menu |
| ACTION_TOGGLE_MENU | START | Open/close menu |

## PSP Button Reference

For developers and advanced users:

### Standard Buttons
- `PSP_CTRL_TRIANGLE` (△) - Triangle button
- `PSP_CTRL_CIRCLE` (○) - Circle button
- `PSP_CTRL_CROSS` (✕) - Cross button
- `PSP_CTRL_SQUARE` (□) - Square button

### D-Pad
- `PSP_CTRL_UP` - D-Pad Up
- `PSP_CTRL_DOWN` - D-Pad Down
- `PSP_CTRL_LEFT` - D-Pad Left
- `PSP_CTRL_RIGHT` - D-Pad Right

### Shoulder Buttons
- `PSP_CTRL_LTRIGGER` - Left shoulder button (L)
- `PSP_CTRL_RTRIGGER` - Right shoulder button (R)

### System Buttons
- `PSP_CTRL_START` - START button
- `PSP_CTRL_SELECT` - SELECT button

### Analog
- `pad.Lx` - Left analog X axis (0-255, center at 128)
- `pad.Ly` - Left analog Y axis (0-255, center at 128)

## Tips and Tricks

### Camera Control
1. **Smooth Movement**: Hold buttons for continuous movement
2. **Precise Rotation**: Use small analog stick movements for fine camera adjustments
3. **Quick Look Around**: Push analog stick to edges for faster rotation

### Menu Navigation
1. **Quick Access**: Press START at any time to access menu
2. **Fast Navigation**: Hold D-Pad Up/Down for rapid menu scrolling (will repeat)
3. **Back Navigation**: Use Circle to quickly back out of menus

## Future Controls

Planned for future versions:

### Additional Camera Modes
- Free-look mode (hold button to decouple camera from movement)
- Fixed camera follow
- Orbit camera around center point

### Additional Actions
- Action button for interaction with objects
- Quick-select menu (SELECT button)
- Debug mode toggle
- Screenshot capture

## Programming Your Own Controls

For developers extending PSP-ECS:

### Adding a New Action
1. Add to `ActionID` enum in `include/keybinds.h`:
   ```c
   typedef enum {
       // ... existing actions
       ACTION_YOUR_NEW_ACTION,
       ACTION_COUNT
   } ActionID;
   ```

2. Add name to `actionNames` array in `src/keybinds.c`:
   ```c
   static const char* actionNames[ACTION_COUNT] = {
       // ... existing names
       "Your New Action"
   };
   ```

3. Set default binding in `Keybinds_Init()`:
   ```c
   system->bindings[ACTION_YOUR_NEW_ACTION].button = PSP_CTRL_SQUARE;
   system->bindings[ACTION_YOUR_NEW_ACTION].name = actionNames[ACTION_YOUR_NEW_ACTION];
   ```

4. Check for the action in your game code:
   ```c
   if (Keybinds_IsActionHeld(&g_keybinds, ACTION_YOUR_NEW_ACTION, &pad)) {
       // Your action logic here
   }
   ```

### Button State Detection

Two methods for checking button state:

#### Held State (Continuous)
```c
if (Keybinds_IsActionHeld(&keybinds, ACTION_MOVE_FORWARD, &pad)) {
    // This triggers every frame while button is held
    MoveForward();
}
```

#### Pressed State (Single Fire)
```c
// Save previous pad state
static SceCtrlData oldPad = {0};
// ... read current pad state

unsigned int button = Keybinds_GetBinding(&keybinds, ACTION_JUMP);
if ((pad.Buttons & button) && !(oldPad.Buttons & button)) {
    // This triggers only once when button is first pressed
    Jump();
}

oldPad = pad; // Save for next frame
```

## Accessibility

### Current Accessibility Features
- Single-button actions (no complex button combinations required)
- Visual feedback for selected menu items
- Clear on-screen button prompts

### Future Accessibility
- Adjustable analog sensitivity
- Toggle vs hold options for movement
- Colorblind-friendly UI options
- Larger UI text options

## Related Files
- `include/keybinds.h` - Keybinding system header
- `src/keybinds.c` - Keybinding implementation
- `src/camera.c` - Camera control implementation
- `src/menu.c` - Menu control implementation
