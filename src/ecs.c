#include "ecs.h"
#include <stdlib.h>
#include <string.h>

void ECS_Init(ECSWorld* world) {
    memset(world, 0, sizeof(ECSWorld));
    world->entityCount = 0;
}

EntityID ECS_CreateEntity(ECSWorld* world) {
    if (world->entityCount >= MAX_ENTITIES) {
        return -1;
    }
    
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!world->entities[i].active) {
            world->entities[i].active = true;
            world->entities[i].componentMask = 0;
            memset(world->entities[i].components, 0, sizeof(void*) * COMPONENT_COUNT);
            world->entityCount++;
            return i;
        }
    }
    
    return -1;
}

void ECS_DestroyEntity(ECSWorld* world, EntityID id) {
    if (id < 0 || id >= MAX_ENTITIES || !world->entities[id].active) {
        return;
    }
    
    // Free all components
    for (int i = 0; i < COMPONENT_COUNT; i++) {
        if (world->entities[id].components[i] != NULL) {
            free(world->entities[id].components[i]);
            world->entities[id].components[i] = NULL;
        }
    }
    
    world->entities[id].active = false;
    world->entities[id].componentMask = 0;
    world->entityCount--;
}

void* ECS_AddComponent(ECSWorld* world, EntityID id, ComponentType type) {
    if (id < 0 || id >= MAX_ENTITIES || !world->entities[id].active) {
        return NULL;
    }
    
    if (world->entities[id].components[type] != NULL) {
        return world->entities[id].components[type];
    }
    
    void* component = NULL;
    size_t size = 0;
    
    switch (type) {
        case COMPONENT_TRANSFORM:
            size = sizeof(TransformComponent);
            component = malloc(size);
            if (component) {
                TransformComponent* transform = (TransformComponent*)component;
                transform->position = (Vector3){0, 0, 0};
                transform->rotation = (Vector3){0, 0, 0};
                transform->scale = (Vector3){1, 1, 1};
            }
            break;
        case COMPONENT_RENDERABLE:
            size = sizeof(RenderableComponent);
            component = malloc(size);
            if (component) {
                RenderableComponent* renderable = (RenderableComponent*)component;
                renderable->type = RENDERABLE_CUBE;
                renderable->color = WHITE;
                renderable->size = (Vector3){1, 1, 1};
            }
            break;
        case COMPONENT_CAMERA:
            size = sizeof(CameraComponent);
            component = malloc(size);
            if (component) {
                CameraComponent* camera = (CameraComponent*)component;
                camera->camera.position = (Vector3){10.0f, 10.0f, 10.0f};
                camera->camera.target = (Vector3){0.0f, 0.0f, 0.0f};
                camera->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
                camera->camera.fovy = 45.0f;
                camera->camera.projection = CAMERA_PERSPECTIVE;
                camera->moveSpeed = 5.0f;
                camera->lookSpeed = 2.0f;
            }
            break;
        case COMPONENT_INPUT:
            size = sizeof(InputComponent);
            component = malloc(size);
            if (component) {
                InputComponent* input = (InputComponent*)component;
                input->active = true;
            }
            break;
        default:
            return NULL;
    }
    
    if (component) {
        world->entities[id].components[type] = component;
        world->entities[id].componentMask |= (1 << type);
    }
    
    return component;
}

void* ECS_GetComponent(ECSWorld* world, EntityID id, ComponentType type) {
    if (id < 0 || id >= MAX_ENTITIES || !world->entities[id].active) {
        return NULL;
    }
    
    return world->entities[id].components[type];
}

bool ECS_HasComponent(ECSWorld* world, EntityID id, ComponentType type) {
    if (id < 0 || id >= MAX_ENTITIES || !world->entities[id].active) {
        return false;
    }
    
    return (world->entities[id].componentMask & (1 << type)) != 0;
}

void ECS_RemoveComponent(ECSWorld* world, EntityID id, ComponentType type) {
    if (id < 0 || id >= MAX_ENTITIES || !world->entities[id].active) {
        return;
    }
    
    if (world->entities[id].components[type] != NULL) {
        free(world->entities[id].components[type]);
        world->entities[id].components[type] = NULL;
        world->entities[id].componentMask &= ~(1 << type);
    }
}

void System_Render(ECSWorld* world) {
    // Render all entities with transform and renderable components
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!world->entities[i].active) continue;
        
        if (ECS_HasComponent(world, i, COMPONENT_TRANSFORM) && 
            ECS_HasComponent(world, i, COMPONENT_RENDERABLE)) {
            
            TransformComponent* transform = (TransformComponent*)ECS_GetComponent(world, i, COMPONENT_TRANSFORM);
            RenderableComponent* renderable = (RenderableComponent*)ECS_GetComponent(world, i, COMPONENT_RENDERABLE);
            
            if (transform && renderable) {
                switch (renderable->type) {
                    case RENDERABLE_CUBE:
                        DrawCube(transform->position, 
                                renderable->size.x, 
                                renderable->size.y, 
                                renderable->size.z, 
                                renderable->color);
                        DrawCubeWires(transform->position, 
                                     renderable->size.x, 
                                     renderable->size.y, 
                                     renderable->size.z, 
                                     BLACK);
                        break;
                    case RENDERABLE_GRID:
                        DrawGrid(20, 1.0f);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
