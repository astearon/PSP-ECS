#ifndef ECS_H
#define ECS_H

#include <raylib.h>
#include <stdbool.h>

// Maximum entities and components
#define MAX_ENTITIES 256
// MAX_COMPONENTS reserved for future expansion, currently using COMPONENT_COUNT
#define MAX_COMPONENTS 8

// Component type IDs
typedef enum {
    COMPONENT_TRANSFORM = 0,
    COMPONENT_RENDERABLE = 1,
    COMPONENT_CAMERA = 2,
    COMPONENT_INPUT = 3,
    COMPONENT_COUNT
} ComponentType;

// Entity ID type
typedef int EntityID;

// Transform Component
typedef struct {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
} TransformComponent;

// Renderable Component
typedef enum {
    RENDERABLE_CUBE,
    RENDERABLE_SPHERE,
    RENDERABLE_PLANE,
    RENDERABLE_GRID
} RenderableType;

typedef struct {
    RenderableType type;
    Color color;
    Vector3 size;
} RenderableComponent;

// Camera Component
typedef struct {
    Camera3D camera;
    float moveSpeed;
    float lookSpeed;
} CameraComponent;

// Input Component
typedef struct {
    bool active;
} InputComponent;

// Entity structure
typedef struct {
    bool active;
    unsigned int componentMask;
    void* components[COMPONENT_COUNT];
} Entity;

// ECS World
typedef struct {
    Entity entities[MAX_ENTITIES];
    int entityCount;
} ECSWorld;

// ECS functions
void ECS_Init(ECSWorld* world);
EntityID ECS_CreateEntity(ECSWorld* world);
void ECS_DestroyEntity(ECSWorld* world, EntityID id);
void* ECS_AddComponent(ECSWorld* world, EntityID id, ComponentType type);
void* ECS_GetComponent(ECSWorld* world, EntityID id, ComponentType type);
bool ECS_HasComponent(ECSWorld* world, EntityID id, ComponentType type);
void ECS_RemoveComponent(ECSWorld* world, EntityID id, ComponentType type);
void ECS_Cleanup(ECSWorld* world);

// System functions
void System_Render(ECSWorld* world);

#endif // ECS_H
