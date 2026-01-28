# PSP-ECS Architecture

This document describes the architecture and design patterns used in PSP-ECS.

## Overview

PSP-ECS implements a data-oriented Entity Component System (ECS) architecture, which separates data (Components) from behavior (Systems) and identity (Entities).

## Core Concepts

### Entities

An **Entity** is simply an ID (integer) that represents a game object. Entities don't contain any data or logic themselves.

```c
typedef int EntityID;
```

Each entity has:
- An `active` flag indicating if it's in use
- A `componentMask` bitmask showing which components it has
- An array of component pointers

### Components

**Components** are pure data structures that hold specific attributes:

#### TransformComponent
Holds position, rotation, and scale in 3D space.
```c
typedef struct {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
} TransformComponent;
```

#### RenderableComponent
Defines how an entity should be rendered.
```c
typedef struct {
    RenderableType type;  // CUBE, SPHERE, PLANE, GRID
    Color color;
    Vector3 size;
} RenderableComponent;
```

#### CameraComponent
Contains camera properties and control settings.
```c
typedef struct {
    Camera3D camera;      // raylib camera
    float moveSpeed;
    float lookSpeed;
} CameraComponent;
```

#### InputComponent
Marks an entity as input-controllable.
```c
typedef struct {
    bool active;
} InputComponent;
```

### Systems

**Systems** contain the logic that operates on entities with specific component combinations.

#### System_Render()
Renders all entities that have both Transform and Renderable components:
- Finds entities with the required components
- Extracts their component data
- Renders based on the renderable type

#### Camera_UpdateControls()
Updates camera position and orientation based on input:
- Reads PSP controller input
- Applies movement in camera-relative directions
- Rotates camera based on analog stick

## ECS World

The `ECSWorld` is a container that manages all entities and their components:

```c
typedef struct {
    Entity entities[MAX_ENTITIES];  // Fixed-size entity array
    int entityCount;                // Current active entity count
} ECSWorld;
```

### Entity Management

#### Creating Entities
```c
EntityID id = ECS_CreateEntity(world);
```
- Finds first inactive entity slot
- Marks it as active
- Returns the entity ID

#### Adding Components
```c
TransformComponent* transform = ECS_AddComponent(world, id, COMPONENT_TRANSFORM);
```
- Allocates memory for the component
- Initializes with default values
- Stores pointer in entity's component array
- Updates component mask

#### Destroying Entities
```c
ECS_DestroyEntity(world, id);
```
- Frees all component memory
- Marks entity as inactive
- Clears component mask

## Component Mask System

Each component type has a unique bit position:
```c
typedef enum {
    COMPONENT_TRANSFORM = 0,   // Bit 0
    COMPONENT_RENDERABLE = 1,  // Bit 1
    COMPONENT_CAMERA = 2,      // Bit 2
    COMPONENT_INPUT = 3,       // Bit 3
    COMPONENT_COUNT
} ComponentType;
```

The component mask is a bitmask where each bit represents the presence of a component:
- Entity with Transform and Renderable: `0011` (binary) = 3
- Entity with Camera and Input: `1100` (binary) = 12

This allows fast checks:
```c
bool hasTransform = (entity->componentMask & (1 << COMPONENT_TRANSFORM)) != 0;
```

## Memory Management

### Component Memory
- Components are heap-allocated when added to an entity
- Memory is freed when component is removed or entity is destroyed
- Each component type has its own size

### Entity Storage
- Fixed-size array of MAX_ENTITIES (256)
- No dynamic allocation for entity array itself
- O(n) search for free slots on entity creation

## Scene Management

The scene system initializes and populates the ECS world:

```c
void Scene_CreateTestScene(ECSWorld* world) {
    // Create camera entity
    EntityID camera = ECS_CreateEntity(world);
    ECS_AddComponent(world, camera, COMPONENT_CAMERA);
    ECS_AddComponent(world, camera, COMPONENT_INPUT);
    
    // Create cube entity
    EntityID cube = ECS_CreateEntity(world);
    ECS_AddComponent(world, cube, COMPONENT_TRANSFORM);
    ECS_AddComponent(world, cube, COMPONENT_RENDERABLE);
    
    // Create grid entity
    EntityID grid = ECS_CreateEntity(world);
    ECS_AddComponent(world, grid, COMPONENT_TRANSFORM);
    ECS_AddComponent(world, grid, COMPONENT_RENDERABLE);
}
```

## Menu System

The menu system is state-based:

```c
typedef enum {
    MENU_NONE,
    MENU_MAIN,
    MENU_OPTIONS
} MenuState;
```

### Menu Flow
1. Game starts with `MENU_NONE` (menu inactive)
2. Press START → `MENU_MAIN` (show main menu)
3. Select "Start Game" → `MENU_NONE` (hide menu)
4. Select "Options" → `MENU_OPTIONS` (show options menu)
5. Select "Back" → `MENU_MAIN` (return to main menu)

### Menu Items
Each menu item has text and an action callback:
```c
typedef struct {
    const char* text;
    void (*action)(void);
} MenuItem;
```

## Keybinding System

The keybinding system maps game actions to PSP buttons:

### Action Definitions
```c
typedef enum {
    ACTION_MOVE_FORWARD,
    ACTION_MOVE_BACKWARD,
    ACTION_TOGGLE_MENU,
    // ... more actions
} ActionID;
```

### Default Bindings
- Movement: D-Pad
- Menu navigation: D-Pad Up/Down
- Menu select: Cross (X)
- Menu back: Circle (O)
- Toggle menu: START

### Future: Remappable Bindings
The system supports runtime rebinding:
```c
Keybinds_SetBinding(&keybinds, ACTION_MOVE_FORWARD, PSP_CTRL_TRIANGLE);
```

Bindings can be saved/loaded (stub functions exist for future implementation).

## Main Loop

The main loop follows this structure:

```c
while (running) {
    // 1. Input
    ReadControllerInput();
    HandleMenuToggle();
    
    // 2. Update
    if (MenuActive) {
        Menu_Update();
    } else {
        UpdateCamera();
        // ... other systems
    }
    
    // 3. Render
    BeginDrawing();
        ClearBackground();
        RenderScene();
        Menu_Render();  // Overlay
    EndDrawing();
}
```

## Design Patterns

### Component Pattern
Components are pure data, no behavior. This separates data from logic.

### System Pattern
Systems iterate over entities with specific component combinations and apply logic.

### Composition over Inheritance
Entities gain functionality by composing components, not through inheritance.

### Cache-Friendly Design
- Components stored separately, not in entity structs
- Systems can iterate over component arrays (future optimization)

## Performance Considerations

### Current Implementation
- Simple linear search for entity iteration
- Components stored as pointers (indirection cost)
- Maximum 256 entities

### Future Optimizations
- Sort entities by component mask for cache locality
- Pack components in arrays by type
- Use archetype-based storage
- Implement entity pools

## Extension Points

### Adding New Components
1. Define component type in `ComponentType` enum
2. Create component struct
3. Add case in `ECS_AddComponent()` for initialization
4. Update `COMPONENT_COUNT`

### Adding New Systems
1. Create system function taking `ECSWorld*`
2. Iterate entities checking for required components
3. Call from main loop at appropriate time

### Adding New Renderables
1. Add type to `RenderableType` enum
2. Add case in `System_Render()` switch statement
3. Implement rendering code

## References

This ECS implementation is inspired by:
- [simple_ecs](https://github.com/raylib-extras/simple_ecs) by raylib-extras
- Data-Oriented Design principles
- Mike Acton's talks on DOD
