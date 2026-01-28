#include "keybinds.h"
#include <string.h>
#include <stdio.h>

static const char* actionNames[ACTION_COUNT] = {
    "Move Forward",
    "Move Backward",
    "Move Left",
    "Move Right",
    "Move Up",
    "Move Down",
    "Menu Up",
    "Menu Down",
    "Menu Select",
    "Menu Back",
    "Toggle Menu"
};

void Keybinds_Init(KeyBindingSystem* system) {
    // Set default keybindings
    system->bindings[ACTION_MOVE_FORWARD].button = PSP_CTRL_UP;
    system->bindings[ACTION_MOVE_FORWARD].name = actionNames[ACTION_MOVE_FORWARD];
    
    system->bindings[ACTION_MOVE_BACKWARD].button = PSP_CTRL_DOWN;
    system->bindings[ACTION_MOVE_BACKWARD].name = actionNames[ACTION_MOVE_BACKWARD];
    
    system->bindings[ACTION_MOVE_LEFT].button = PSP_CTRL_LEFT;
    system->bindings[ACTION_MOVE_LEFT].name = actionNames[ACTION_MOVE_LEFT];
    
    system->bindings[ACTION_MOVE_RIGHT].button = PSP_CTRL_RIGHT;
    system->bindings[ACTION_MOVE_RIGHT].name = actionNames[ACTION_MOVE_RIGHT];
    
    system->bindings[ACTION_MOVE_UP].button = PSP_CTRL_LTRIGGER;
    system->bindings[ACTION_MOVE_UP].name = actionNames[ACTION_MOVE_UP];
    
    system->bindings[ACTION_MOVE_DOWN].button = PSP_CTRL_RTRIGGER;
    system->bindings[ACTION_MOVE_DOWN].name = actionNames[ACTION_MOVE_DOWN];
    
    system->bindings[ACTION_MENU_UP].button = PSP_CTRL_UP;
    system->bindings[ACTION_MENU_UP].name = actionNames[ACTION_MENU_UP];
    
    system->bindings[ACTION_MENU_DOWN].button = PSP_CTRL_DOWN;
    system->bindings[ACTION_MENU_DOWN].name = actionNames[ACTION_MENU_DOWN];
    
    system->bindings[ACTION_MENU_SELECT].button = PSP_CTRL_CROSS;
    system->bindings[ACTION_MENU_SELECT].name = actionNames[ACTION_MENU_SELECT];
    
    system->bindings[ACTION_MENU_BACK].button = PSP_CTRL_CIRCLE;
    system->bindings[ACTION_MENU_BACK].name = actionNames[ACTION_MENU_BACK];
    
    system->bindings[ACTION_TOGGLE_MENU].button = PSP_CTRL_START;
    system->bindings[ACTION_TOGGLE_MENU].name = actionNames[ACTION_TOGGLE_MENU];
}

void Keybinds_SetBinding(KeyBindingSystem* system, ActionID action, unsigned int button) {
    if (action < 0 || action >= ACTION_COUNT) return;
    system->bindings[action].button = button;
}

unsigned int Keybinds_GetBinding(KeyBindingSystem* system, ActionID action) {
    if (action < 0 || action >= ACTION_COUNT) return 0;
    return system->bindings[action].button;
}

bool Keybinds_IsActionDown(KeyBindingSystem* system, ActionID action, SceCtrlData* pad) {
    if (action < 0 || action >= ACTION_COUNT) return false;
    
    unsigned int button = system->bindings[action].button;
    return (pad->Buttons & button) != 0;
}

bool Keybinds_IsActionHeld(KeyBindingSystem* system, ActionID action, SceCtrlData* pad) {
    // Alias for IsActionDown - both check if button is currently held
    return Keybinds_IsActionDown(system, action, pad);
}

const char* Keybinds_GetActionName(ActionID action) {
    if (action < 0 || action >= ACTION_COUNT) return "";
    return actionNames[action];
}

void Keybinds_Save(KeyBindingSystem* system) {
    // TODO: Implement save to file
    // For PSP, this would save to ms0:/PSP/SAVEDATA/
}

void Keybinds_Load(KeyBindingSystem* system) {
    // TODO: Implement load from file
    // For PSP, this would load from ms0:/PSP/SAVEDATA/
}
