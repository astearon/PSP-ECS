#include "menu.h"
#include "keybinds.h"
#include <raylib.h>
#include <pspctrl.h>
#include <string.h>

extern KeyBindingSystem g_keybinds;

static void Menu_Action_Start(void);
static void Menu_Action_Options(void);
static void Menu_Action_Back(void);

static MenuItem mainMenuItems[] = {
    {"Start Game", Menu_Action_Start},
    {"Options", Menu_Action_Options}
};

static MenuItem optionsMenuItems[] = {
    {"Keybindings", NULL},
    {"Back", Menu_Action_Back}
};

static MenuSystem* g_currentMenu = NULL;

void Menu_Init(MenuSystem* menu) {
    menu->currentMenu = MENU_NONE;
    menu->selectedItem = 0;
    menu->isActive = false;
    g_currentMenu = menu;
}

static void Menu_Action_Start(void) {
    if (g_currentMenu) {
        Menu_Hide(g_currentMenu);
    }
}

static void Menu_Action_Options(void) {
    if (g_currentMenu) {
        g_currentMenu->currentMenu = MENU_OPTIONS;
        g_currentMenu->selectedItem = 0;
    }
}

static void Menu_Action_Back(void) {
    if (g_currentMenu) {
        g_currentMenu->currentMenu = MENU_MAIN;
        g_currentMenu->selectedItem = 0;
    }
}

void Menu_Update(MenuSystem* menu) {
    if (!menu->isActive) return;
    
    static SceCtrlData pad;
    static SceCtrlData oldPad = {0};
    
    oldPad = pad;
    sceCtrlReadBufferPositive(&pad, 1);
    
    int itemCount = 0;
    MenuItem* items = NULL;
    
    if (menu->currentMenu == MENU_MAIN) {
        items = mainMenuItems;
        itemCount = sizeof(mainMenuItems) / sizeof(MenuItem);
    } else if (menu->currentMenu == MENU_OPTIONS) {
        items = optionsMenuItems;
        itemCount = sizeof(optionsMenuItems) / sizeof(MenuItem);
    }
    
    // Navigation - detect button press (not held)
    unsigned int downButton = Keybinds_GetBinding(&g_keybinds, ACTION_MENU_DOWN);
    unsigned int upButton = Keybinds_GetBinding(&g_keybinds, ACTION_MENU_UP);
    unsigned int selectButton = Keybinds_GetBinding(&g_keybinds, ACTION_MENU_SELECT);
    
    if ((pad.Buttons & downButton) && !(oldPad.Buttons & downButton)) {
        menu->selectedItem = (menu->selectedItem + 1) % itemCount;
    }
    
    if ((pad.Buttons & upButton) && !(oldPad.Buttons & upButton)) {
        menu->selectedItem = (menu->selectedItem - 1 + itemCount) % itemCount;
    }
    
    // Selection - detect button press (not held)
    if ((pad.Buttons & selectButton) && !(oldPad.Buttons & selectButton)) {
        if (items && items[menu->selectedItem].action) {
            items[menu->selectedItem].action();
        }
    }
}

void Menu_Render(MenuSystem* menu) {
    if (!menu->isActive) return;
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 180});
    
    MenuItem* items = NULL;
    int itemCount = 0;
    const char* title = "";
    
    if (menu->currentMenu == MENU_MAIN) {
        items = mainMenuItems;
        itemCount = sizeof(mainMenuItems) / sizeof(MenuItem);
        title = "MAIN MENU";
    } else if (menu->currentMenu == MENU_OPTIONS) {
        items = optionsMenuItems;
        itemCount = sizeof(optionsMenuItems) / sizeof(MenuItem);
        title = "OPTIONS";
    }
    
    // Draw title
    int titleWidth = MeasureText(title, 30);
    DrawText(title, (screenWidth - titleWidth) / 2, 50, 30, WHITE);
    
    // Draw menu items
    int startY = 120;
    int itemSpacing = 40;
    
    for (int i = 0; i < itemCount; i++) {
        Color color = (i == menu->selectedItem) ? YELLOW : WHITE;
        const char* prefix = (i == menu->selectedItem) ? "> " : "  ";
        char text[256];
        snprintf(text, sizeof(text), "%s%s", prefix, items[i].text);
        
        int textWidth = MeasureText(text, 20);
        DrawText(text, (screenWidth - textWidth) / 2, startY + i * itemSpacing, 20, color);
    }
    
    // Draw controls help
    DrawText("UP/DOWN: Navigate | X: Select", 10, screenHeight - 30, 15, LIGHTGRAY);
}

void Menu_Show(MenuSystem* menu, MenuState state) {
    menu->currentMenu = state;
    menu->selectedItem = 0;
    menu->isActive = true;
}

void Menu_Hide(MenuSystem* menu) {
    menu->isActive = false;
}

bool Menu_IsActive(MenuSystem* menu) {
    return menu->isActive;
}
