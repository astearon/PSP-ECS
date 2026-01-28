#ifndef MENU_H
#define MENU_H

#include <stdbool.h>

// Menu states
typedef enum {
    MENU_NONE,
    MENU_MAIN,
    MENU_OPTIONS
} MenuState;

// Menu item structure
typedef struct {
    const char* text;
    void (*action)(void);
} MenuItem;

// Menu system
typedef struct {
    MenuState currentMenu;
    int selectedItem;
    bool isActive;
} MenuSystem;

// Menu functions
void Menu_Init(MenuSystem* menu);
void Menu_Update(MenuSystem* menu);
void Menu_Render(MenuSystem* menu);
void Menu_Show(MenuSystem* menu, MenuState state);
void Menu_Hide(MenuSystem* menu);
bool Menu_IsActive(MenuSystem* menu);

#endif // MENU_H
