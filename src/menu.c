#include "menu.h"
#include "keybinds.h"
#include "scene.h"
#include <raylib.h>
#include <pspctrl.h>
#include <string.h>
#include <stdio.h>

extern KeyBindingSystem g_keybinds;

static void Menu_Action_Start(void);
static void Menu_Action_Options(void);
static void Menu_Action_Back(void);
static void Menu_Action_Save(void);
static void Menu_Action_Load(void);
static void Menu_Action_Keybindings(void);

static MenuItem mainMenuItems[] = {
    {"Start Game", Menu_Action_Start},
    {"Save Game", Menu_Action_Save},
    {"Load Game", Menu_Action_Load},
    {"Options", Menu_Action_Options}
};

static MenuItem optionsMenuItems[] = {
    {"Keybindings", Menu_Action_Keybindings},
    {"Back", Menu_Action_Back}
};

static MenuSystem* g_currentMenu = NULL;
extern ECSWorld g_world;

static char g_statusMessage[64] = {0};
static int g_statusFrames = 0;

static void Menu_ShowStatus(const char* message, int frames) {
    if (!message) return;
    strncpy(g_statusMessage, message, sizeof(g_statusMessage) - 1);
    g_statusMessage[sizeof(g_statusMessage) - 1] = '\0';
    g_statusFrames = frames;
}

void Menu_Init(MenuSystem* menu) {
    menu->currentMenu = MENU_NONE;
    menu->selectedItem = 0;
    menu->isActive = false;
    g_currentMenu = menu;
}

static void Menu_Action_Start(void) {
    if (g_currentMenu) {
        Scene_ResetToDefault(&g_world);
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

static void Menu_Action_Keybindings(void) {
    if (g_currentMenu) {
        g_currentMenu->currentMenu = MENU_KEYBINDINGS;
        g_currentMenu->selectedItem = 0;
    }
}

static void Menu_Action_Save(void) {
    Scene_Save(&g_world);
}

static void Menu_Action_Load(void) {
    if (Scene_GetPopulatedSaveCount() <= 0) {
        Menu_ShowStatus("No saves found!", 180);
        return;
    }

    if (!Scene_Load(&g_world)) {
        Menu_ShowStatus("Load failed", 180);
    }
}

void Menu_Update(MenuSystem* menu) {
    if (!menu->isActive) return;

    if (g_statusFrames > 0) {
        g_statusFrames--;
        if (g_statusFrames == 0) {
            g_statusMessage[0] = '\0';
        }
    }
    
    // Track button states for press detection (oldPad initialized to zero on first call)
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
    } else if (menu->currentMenu == MENU_KEYBINDINGS) {
        // No items for keybindings screen (display only)
        itemCount = 0;
        items = NULL;
    }
    
    // Navigation - detect button press (not held)
    unsigned int downButton = Keybinds_GetBinding(&g_keybinds, ACTION_MENU_DOWN);
    unsigned int upButton = Keybinds_GetBinding(&g_keybinds, ACTION_MENU_UP);
    unsigned int selectButton = Keybinds_GetBinding(&g_keybinds, ACTION_MENU_SELECT);
    unsigned int backButton = Keybinds_GetBinding(&g_keybinds, ACTION_MENU_BACK);
    
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
    
    // Back button - detect button press (not held)
    if ((pad.Buttons & backButton) && !(oldPad.Buttons & backButton)) {
        if (menu->currentMenu == MENU_MAIN) {
            // Close menu if at main menu
            Menu_Hide(menu);
        } else {
            // Go back to main menu from any other menu
            menu->currentMenu = MENU_MAIN;
            menu->selectedItem = 0;
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
    } else if (menu->currentMenu == MENU_KEYBINDINGS) {
        items = NULL;
        itemCount = 0;
        title = "KEYBINDINGS";
    }
    
    // Draw title
    int titleWidth = MeasureText(title, 30);
    DrawText(title, (screenWidth - titleWidth) / 2, 10, 30, WHITE);
    
    // Draw menu items or keybindings display
    if (menu->currentMenu == MENU_KEYBINDINGS) {
        // Display in-game keybindings (excluding menu actions)
        int startY = 55;
        int lineSpacing = 22;
        int currentY = startY;
        
        // Helper function to get button name
        const char* GetButtonName(unsigned int button) {
            switch(button) {
                case PSP_CTRL_UP: return "D-Pad Up";
                case PSP_CTRL_DOWN: return "D-Pad Down";
                case PSP_CTRL_LEFT: return "D-Pad Left";
                case PSP_CTRL_RIGHT: return "D-Pad Right";
                case PSP_CTRL_TRIANGLE: return "Triangle";
                case PSP_CTRL_CIRCLE: return "Circle";
                case PSP_CTRL_CROSS: return "Cross";
                case PSP_CTRL_SQUARE: return "Square";
                case PSP_CTRL_LTRIGGER: return "L Trigger";
                case PSP_CTRL_RTRIGGER: return "R Trigger";
                case PSP_CTRL_START: return "Start";
                case PSP_CTRL_SELECT: return "Select";
                default: return "Unknown";
            }
        }
        
        // Display in-game actions only
        ActionID inGameActions[] = {
            ACTION_MOVE_FORWARD,
            ACTION_MOVE_BACKWARD,
            ACTION_MOVE_LEFT,
            ACTION_MOVE_RIGHT,
            ACTION_MOVE_UP,
            ACTION_MOVE_DOWN,
            ACTION_TOGGLE_MENU
        };
        
        // First pass: calculate max width for centering
        int maxWidth = 0;
        for (int i = 0; i < 7; i++) {
            ActionID action = inGameActions[i];
            unsigned int button = Keybinds_GetBinding(&g_keybinds, action);
            const char* actionName = Keybinds_GetActionName(action);
            const char* buttonName = GetButtonName(button);
            
            char line[128];
            snprintf(line, sizeof(line), "%s: %s", actionName, buttonName);
            int lineWidth = MeasureText(line, 16);
            if (lineWidth > maxWidth) maxWidth = lineWidth;
        }
        
        // Second pass: draw centered
        currentY = startY;
        for (int i = 0; i < 7; i++) {
            ActionID action = inGameActions[i];
            unsigned int button = Keybinds_GetBinding(&g_keybinds, action);
            const char* actionName = Keybinds_GetActionName(action);
            const char* buttonName = GetButtonName(button);
            
            char line[128];
            snprintf(line, sizeof(line), "%s: %s", actionName, buttonName);
            int lineWidth = MeasureText(line, 16);
            int xPos = (screenWidth - lineWidth) / 2;
            DrawText(line, xPos, currentY, 16, WHITE);
            currentY += lineSpacing;
        }
    } else {
        // Draw menu items
        int startY = 55;
        int itemSpacing = 40;
        
        for (int i = 0; i < itemCount; i++) {
            Color color = (i == menu->selectedItem) ? YELLOW : WHITE;
            const char* prefix = (i == menu->selectedItem) ? "> " : "  ";
            char text[256];
            snprintf(text, sizeof(text), "%s%s", prefix, items[i].text);
            
            int textWidth = MeasureText(text, 20);
            DrawText(text, (screenWidth - textWidth) / 2, startY + i * itemSpacing, 20, color);
        }
    }
    
    // Draw controls help
    const char* helpText = "UP/DOWN: Navigate | X: Select | O: Back";
    int helpWidth = MeasureText(helpText, 15);
    DrawText(helpText, (screenWidth - helpWidth) / 2, screenHeight - 30, 15, LIGHTGRAY);

    if (g_statusMessage[0] != '\0') {
        int msgWidth = MeasureText(g_statusMessage, 18);
        DrawText(g_statusMessage, (screenWidth - msgWidth) / 2, screenHeight - 55, 18, YELLOW);
    }
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
