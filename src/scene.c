#include "scene.h"

void Scene_Init(ECSWorld* world) {
    ECS_Init(world);
}

void Scene_CreateTestScene(ECSWorld* world) {
    // Create camera entity
    EntityID cameraEntity = ECS_CreateEntity(world);
    CameraComponent* camera = (CameraComponent*)ECS_AddComponent(world, cameraEntity, COMPONENT_CAMERA);
    ECS_AddComponent(world, cameraEntity, COMPONENT_INPUT);
    
    if (camera) {
        camera->camera.position = (Vector3){10.0f, 10.0f, 10.0f};
        camera->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
        camera->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
        camera->camera.fovy = 45.0f;
        camera->camera.projection = CAMERA_PERSPECTIVE;
        camera->moveSpeed = 5.0f;
        camera->lookSpeed = 2.0f;
    }
    
    // Create cube entity at center
    EntityID cubeEntity = ECS_CreateEntity(world);
    TransformComponent* cubeTransform = (TransformComponent*)ECS_AddComponent(world, cubeEntity, COMPONENT_TRANSFORM);
    RenderableComponent* cubeRenderable = (RenderableComponent*)ECS_AddComponent(world, cubeEntity, COMPONENT_RENDERABLE);
    
    if (cubeTransform) {
        cubeTransform->position = (Vector3){0.0f, 1.0f, 0.0f};
        cubeTransform->rotation = (Vector3){0.0f, 0.0f, 0.0f};
        cubeTransform->scale = (Vector3){1.0f, 1.0f, 1.0f};
    }
    
    if (cubeRenderable) {
        cubeRenderable->type = RENDERABLE_CUBE;
        cubeRenderable->color = WHITE;
        cubeRenderable->size = (Vector3){2.0f, 2.0f, 2.0f};
    }
    
    // Create grid entity
    EntityID gridEntity = ECS_CreateEntity(world);
    TransformComponent* gridTransform = (TransformComponent*)ECS_AddComponent(world, gridEntity, COMPONENT_TRANSFORM);
    RenderableComponent* gridRenderable = (RenderableComponent*)ECS_AddComponent(world, gridEntity, COMPONENT_RENDERABLE);
    
    if (gridTransform) {
        gridTransform->position = (Vector3){0.0f, 0.0f, 0.0f};
        gridTransform->rotation = (Vector3){0.0f, 0.0f, 0.0f};
        gridTransform->scale = (Vector3){1.0f, 1.0f, 1.0f};
    }
    
    if (gridRenderable) {
        gridRenderable->type = RENDERABLE_GRID;
        gridRenderable->color = WHITE;
        gridRenderable->size = (Vector3){1.0f, 1.0f, 1.0f};
    }
}
