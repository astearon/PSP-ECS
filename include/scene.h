#ifndef SCENE_H
#define SCENE_H

#include "ecs.h"

// Scene management
void Scene_Init(ECSWorld* world);
void Scene_CreateTestScene(ECSWorld* world);
bool Scene_Save(ECSWorld* world);
bool Scene_Load(ECSWorld* world);
void Scene_ResetToDefault(ECSWorld* world);
int Scene_GetPopulatedSaveCount(void);

#endif // SCENE_H
