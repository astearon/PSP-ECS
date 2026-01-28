#ifndef KEYBINDS_H
#define KEYBINDS_H

#include <pspctrl.h>

// Action IDs
typedef enum {
    ACTION_MOVE_FORWARD,
    ACTION_MOVE_BACKWARD,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_MOVE_UP,
    ACTION_MOVE_DOWN,
    ACTION_MENU_UP,
    ACTION_MENU_DOWN,
    ACTION_MENU_SELECT,
    ACTION_MENU_BACK,
    ACTION_TOGGLE_MENU,
    ACTION_COUNT
} ActionID;

// Keybinding structure
typedef struct {
    unsigned int button;
    const char* name;
} KeyBinding;

// Keybinding system
typedef struct {
    KeyBinding bindings[ACTION_COUNT];
} KeyBindingSystem;

// Keybinding functions
void Keybinds_Init(KeyBindingSystem* system);
void Keybinds_SetBinding(KeyBindingSystem* system, ActionID action, unsigned int button);
unsigned int Keybinds_GetBinding(KeyBindingSystem* system, ActionID action);
bool Keybinds_IsActionPressed(KeyBindingSystem* system, ActionID action, SceCtrlData* pad);
bool Keybinds_IsActionHeld(KeyBindingSystem* system, ActionID action, SceCtrlData* pad);
const char* Keybinds_GetActionName(ActionID action);
void Keybinds_Save(KeyBindingSystem* system);
void Keybinds_Load(KeyBindingSystem* system);

#endif // KEYBINDS_H
