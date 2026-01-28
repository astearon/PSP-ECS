#include <raylib.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspkernel.h>
#include "ecs.h"
#include "menu.h"
#include "keybinds.h"
#include "scene.h"
#include "camera.h"

PSP_MODULE_INFO("PSP-ECS", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

// Global systems
KeyBindingSystem g_keybinds;
MenuSystem g_menu;
ECSWorld g_world;

// Exit callback
int running = 1;

int exit_callback(int arg1, int arg2, void *common) {
    running = 0;
    return 0;
}

int CallbackThread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int SetupCallbacks(void) {
    int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}

void UpdateCamera(float deltaTime) {
    // Find camera entity and update it
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!g_world.entities[i].active) continue;
        
        if (ECS_HasComponent(&g_world, i, COMPONENT_CAMERA) && 
            ECS_HasComponent(&g_world, i, COMPONENT_INPUT)) {
            
            CameraComponent* camera = (CameraComponent*)ECS_GetComponent(&g_world, i, COMPONENT_CAMERA);
            if (camera) {
                Camera_UpdateControls(camera, &g_keybinds, deltaTime);
            }
        }
    }
}

void RenderScene(void) {
    // Find camera entity
    CameraComponent* activeCamera = NULL;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!g_world.entities[i].active) continue;
        
        if (ECS_HasComponent(&g_world, i, COMPONENT_CAMERA)) {
            activeCamera = (CameraComponent*)ECS_GetComponent(&g_world, i, COMPONENT_CAMERA);
            break;
        }
    }
    
    if (activeCamera) {
        BeginMode3D(activeCamera->camera);
        
        // Render all entities
        System_Render(&g_world);
        
        EndMode3D();
    }
}

int main(void) {
    SetupCallbacks();
    
    // Initialize PSP controller
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    
    // Initialize Raylib
    const int screenWidth = 480;
    const int screenHeight = 272;
    
    InitWindow(screenWidth, screenHeight, "PSP-ECS Demo");
    SetTargetFPS(60);
    
    // Initialize systems
    Keybinds_Init(&g_keybinds);
    Menu_Init(&g_menu);
    Scene_Init(&g_world);
    Scene_CreateTestScene(&g_world);
    
    // Main game loop
    while (running && !WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Input handling
        SceCtrlData pad;
        sceCtrlReadBufferPositive(&pad, 1);
        
        // Toggle menu with START button
        static bool startWasPressed = false;
        if (Keybinds_IsActionPressed(&g_keybinds, ACTION_TOGGLE_MENU, &pad)) {
            if (!startWasPressed) {
                if (Menu_IsActive(&g_menu)) {
                    Menu_Hide(&g_menu);
                } else {
                    Menu_Show(&g_menu, MENU_MAIN);
                }
                startWasPressed = true;
            }
        } else {
            startWasPressed = false;
        }
        
        // Update
        if (Menu_IsActive(&g_menu)) {
            Menu_Update(&g_menu);
        } else {
            UpdateCamera(deltaTime);
        }
        
        // Render
        BeginDrawing();
        
        ClearBackground((Color){100, 100, 100, 255}); // Gray background
        
        if (!Menu_IsActive(&g_menu)) {
            RenderScene();
            
            // Draw HUD
            DrawText("PSP-ECS Demo", 10, 10, 20, WHITE);
            DrawFPS(screenWidth - 80, 10);
            DrawText("Press START for menu", 10, screenHeight - 30, 15, LIGHTGRAY);
        } else {
            // Still render the scene in background but darker
            RenderScene();
        }
        
        // Render menu on top
        Menu_Render(&g_menu);
        
        EndDrawing();
    }
    
    // Cleanup
    CloseWindow();
    
    sceKernelExitGame();
    return 0;
}
