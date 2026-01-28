#ifndef CAMERA_H
#define CAMERA_H

#include "ecs.h"
#include "keybinds.h"

// Camera control functions
void Camera_UpdateControls(CameraComponent* camera, KeyBindingSystem* keybinds, float deltaTime);

#endif // CAMERA_H
