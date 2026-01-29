#include "camera.h"
#include <pspctrl.h>
#include <raymath.h>
#include <stdlib.h>

void Camera_UpdateControls(CameraComponent* camera, KeyBindingSystem* keybinds, float deltaTime) {
    if (!camera) return;
    
    SceCtrlData pad;
    sceCtrlReadBufferPositive(&pad, 1);
    
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera->camera.target, camera->camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera->camera.up));
    Vector3 movement = {0, 0, 0};
    
    // Movement controls
    if (Keybinds_IsActionHeld(keybinds, ACTION_MOVE_FORWARD, &pad)) {
        movement = Vector3Add(movement, Vector3Scale(forward, camera->moveSpeed * deltaTime));
    }
    
    if (Keybinds_IsActionHeld(keybinds, ACTION_MOVE_BACKWARD, &pad)) {
        movement = Vector3Add(movement, Vector3Scale(forward, -camera->moveSpeed * deltaTime));
    }
    
    if (Keybinds_IsActionHeld(keybinds, ACTION_MOVE_LEFT, &pad)) {
        movement = Vector3Add(movement, Vector3Scale(right, -camera->moveSpeed * deltaTime));
    }
    
    if (Keybinds_IsActionHeld(keybinds, ACTION_MOVE_RIGHT, &pad)) {
        movement = Vector3Add(movement, Vector3Scale(right, camera->moveSpeed * deltaTime));
    }
    
    if (Keybinds_IsActionHeld(keybinds, ACTION_MOVE_UP, &pad)) {
        movement = Vector3Add(movement, Vector3Scale(camera->camera.up, -camera->moveSpeed * deltaTime));
    }
    
    if (Keybinds_IsActionHeld(keybinds, ACTION_MOVE_DOWN, &pad)) {
        movement = Vector3Add(movement, Vector3Scale(camera->camera.up, camera->moveSpeed * deltaTime));
    }
    
    // Apply movement
    camera->camera.position = Vector3Add(camera->camera.position, movement);
    camera->camera.target = Vector3Add(camera->camera.target, movement);
    
    // Analog stick for camera rotation
    // Apply dead zone to prevent drift (analog center is 128, accept 118-138 as neutral)
    const int ANALOG_DEAD_ZONE = 10;
    int analogX = pad.Lx - 128;
    int analogY = pad.Ly - 128;
    
    if (abs(analogX) > ANALOG_DEAD_ZONE || abs(analogY) > ANALOG_DEAD_ZONE) {
        float rotX = (-analogX / 128.0f) * camera->lookSpeed * deltaTime;
        float rotY = (-analogY / 128.0f) * camera->lookSpeed * deltaTime;
        
        // Update pitch and clamp to ±88 degrees
        const float MAX_PITCH = 88.0f;
        const float RAD_TO_DEG = 57.29577951f;
        const float DEG_TO_RAD = 0.01745329251f;
        
        camera->pitch += rotY * RAD_TO_DEG;
        if (camera->pitch > MAX_PITCH) camera->pitch = MAX_PITCH;
        if (camera->pitch < -MAX_PITCH) camera->pitch = -MAX_PITCH;
        
        // Calculate new rotation amount respecting the clamp
        rotY = (camera->pitch - (camera->pitch - rotY * RAD_TO_DEG)) * DEG_TO_RAD;
        
        // Update target based on rotation
        Vector3 direction = Vector3Subtract(camera->camera.target, camera->camera.position);
        float distance = Vector3Length(direction);
        
        // Horizontal rotation (around Y axis)
        Matrix rotationY = MatrixRotateY(rotX);
        direction = Vector3Transform(direction, rotationY);
        
        // Vertical rotation (around right vector)
        Matrix rotationX = MatrixRotate(right, rotY);
        direction = Vector3Transform(direction, rotationX);
        
        direction = Vector3Normalize(direction);
        camera->camera.target = Vector3Add(camera->camera.position, Vector3Scale(direction, distance));
    }
}
