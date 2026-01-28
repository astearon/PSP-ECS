#include "scene.h"
#include <pspdisplay.h>
#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <psputility.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SAVE_GAME_NAME "PSP-ECS"
#define SAVE_SLOT_NAME "DATA00"
#define SAVE_FILE_NAME "SAVE.BIN"
#define SAVE_TITLE "PSP-ECS Demo"
#define SAVE_DETAIL "ECS scene state"
#define LOG_FILE_NAME "psp-ecs-log.txt"
#define SAVE_SLOT_COUNT 10

static char g_saveSlotList[SAVE_SLOT_COUNT + 1][20] = {
    "DATA00",
    "DATA01",
    "DATA02",
    "DATA03",
    "DATA04",
    "DATA05",
    "DATA06",
    "DATA07",
    "DATA08",
    "DATA09",
    ""
};

static const char* Scene_GetSaveMount(void) {
    static char mount[5] = "ms0:";
    char cwd[256];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if (strncmp(cwd, "ef0:", 4) == 0) {
            strncpy(mount, "ef0:", sizeof(mount));
            return mount;
        }
        if (strncmp(cwd, "ms0:", 4) == 0) {
            strncpy(mount, "ms0:", sizeof(mount));
            return mount;
        }
    }

    return mount;
}

static void Scene_BuildSaveRoot(char* outPath, size_t outSize) {
    const char* mount = Scene_GetSaveMount();
    snprintf(outPath, outSize, "%s/PSP/SAVEDATA", mount);
}

static void Scene_BuildLogPath(char* outPath, size_t outSize) {
    char root[64];
    Scene_BuildSaveRoot(root, sizeof(root));
    snprintf(outPath, outSize, "%s/PSP-ECS/%s", root, LOG_FILE_NAME);
}

static void Scene_EnsureLogDir(void) {
    char root[64];
    Scene_BuildSaveRoot(root, sizeof(root));
    sceIoMkdir(root, 0777);

    char dirPath[96];
    snprintf(dirPath, sizeof(dirPath), "%s/PSP-ECS", root);
    sceIoMkdir(dirPath, 0777);
}

static int Scene_BuildPopulatedSaveList(char list[][20], int maxCount) {
    int count = 0;
    for (int i = 0; i < SAVE_SLOT_COUNT && count < maxCount; i++) {
        char path[128];
        char root[64];
        Scene_BuildSaveRoot(root, sizeof(root));
        snprintf(path, sizeof(path), "%s/%s%s/%s", root, SAVE_GAME_NAME, g_saveSlotList[i], SAVE_FILE_NAME);
        SceIoStat stat;
        if (sceIoGetstat(path, &stat) >= 0) {
            strncpy(list[count], g_saveSlotList[i], sizeof(list[count]) - 1);
            list[count][sizeof(list[count]) - 1] = '\0';
            count++;
        }
    }

    if (count < maxCount) {
        list[count][0] = '\0';
    } else if (maxCount > 0) {
        list[maxCount - 1][0] = '\0';
    }

    return count;
}

static void Scene_Log(const char* message) {
    Scene_EnsureLogDir();
    char logPath[128];
    Scene_BuildLogPath(logPath, sizeof(logPath));
    SceUID fd = sceIoOpen(logPath, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);
    if (fd < 0) return;
    sceIoWrite(fd, message, strlen(message));
    sceIoWrite(fd, "\n", 1);
    sceIoClose(fd);
}

typedef struct {
    unsigned int componentMask;
    TransformComponent transform;
    RenderableComponent renderable;
    CameraComponent camera;
    InputComponent input;
} SceneEntitySave;

typedef struct {
    int activeCount;
    SceneEntitySave entities[MAX_ENTITIES];
} SceneSaveData;

static void Scene_InitSavedataParams(SceUtilitySavedataParam* params) {
    memset(params, 0, sizeof(*params));
    params->base.size = sizeof(*params);
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &params->base.language);
    #if defined(PSP_UTILITY_SWAP_XO)
    params->base.buttonSwap = PSP_UTILITY_SWAP_XO;
    #elif defined(PSP_UTILITY_SWAP_CIRCLE_CROSS)
    params->base.buttonSwap = PSP_UTILITY_SWAP_CIRCLE_CROSS;
    #else
    params->base.buttonSwap = 0;
    #endif
    params->base.graphicsThread = 0x11;
    params->base.accessThread = 0x13;
    params->base.fontThread = 0x12;
    params->base.soundThread = 0x10;
}

static bool Scene_RunSavedata(SceUtilitySavedataParam* params) {
    Scene_Log("Savedata: init start");
    int initResult = sceUtilitySavedataInitStart(params);
    if (initResult < 0) {
        char logLine[128];
        snprintf(logLine, sizeof(logLine), "Savedata: init failed (%d)", initResult);
        Scene_Log(logLine);
        return false;
    }

    int lastStatus = -1;

    while (1) {
        int status = sceUtilitySavedataGetStatus();
        if (status != lastStatus) {
            char logLine[128];
            snprintf(logLine, sizeof(logLine), "Savedata: status %d", status);
            Scene_Log(logLine);
            lastStatus = status;
        }

        if (status == PSP_UTILITY_DIALOG_NONE) {
            Scene_Log("Savedata: dialog none (done)");
            break;
        }

        if (status == PSP_UTILITY_DIALOG_QUIT || status == PSP_UTILITY_DIALOG_FINISHED) {
            Scene_Log("Savedata: shutdown start");
            sceUtilitySavedataShutdownStart();
        } else if (status == PSP_UTILITY_DIALOG_VISIBLE || status == PSP_UTILITY_DIALOG_INIT) {
            sceUtilitySavedataUpdate(1);
        }

        sceDisplayWaitVblankStart();
        sceKernelDelayThread(10000);
    }

    return true;
}

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
    
    // Create ground plane entity
    EntityID groundEntity = ECS_CreateEntity(world);
    TransformComponent* groundTransform = (TransformComponent*)ECS_AddComponent(world, groundEntity, COMPONENT_TRANSFORM);
    RenderableComponent* groundRenderable = (RenderableComponent*)ECS_AddComponent(world, groundEntity, COMPONENT_RENDERABLE);
    
    if (groundTransform) {
        groundTransform->position = (Vector3){0.0f, 0.0f, 0.0f};
        groundTransform->rotation = (Vector3){0.0f, 0.0f, 0.0f};
        groundTransform->scale = (Vector3){1.0f, 1.0f, 1.0f};
    }
    
    if (groundRenderable) {
        groundRenderable->type = RENDERABLE_PLANE;
        groundRenderable->color = (Color){50, 50, 50, 255};
        groundRenderable->size = (Vector3){50.0f, 1.0f, 50.0f};
    }
}

void Scene_ResetToDefault(ECSWorld* world) {
    if (!world) return;
    ECS_Cleanup(world);
    ECS_Init(world);
    Scene_CreateTestScene(world);
}

int Scene_GetPopulatedSaveCount(void) {
    char list[SAVE_SLOT_COUNT + 1][20];
    return Scene_BuildPopulatedSaveList(list, SAVE_SLOT_COUNT + 1);
}

bool Scene_Save(ECSWorld* world) {
    if (!world) return false;

    Scene_Log("Scene_Save: start");

    SceneSaveData* saveData = (SceneSaveData*)malloc(sizeof(SceneSaveData));
    if (!saveData) return false;
    memset(saveData, 0, sizeof(SceneSaveData));

    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!world->entities[i].active) continue;

        SceneEntitySave* entry = &saveData->entities[saveData->activeCount++];
        entry->componentMask = world->entities[i].componentMask;

        if (entry->componentMask & (1 << COMPONENT_TRANSFORM)) {
            TransformComponent* transform = (TransformComponent*)ECS_GetComponent(world, i, COMPONENT_TRANSFORM);
            if (transform) entry->transform = *transform;
        }

        if (entry->componentMask & (1 << COMPONENT_RENDERABLE)) {
            RenderableComponent* renderable = (RenderableComponent*)ECS_GetComponent(world, i, COMPONENT_RENDERABLE);
            if (renderable) entry->renderable = *renderable;
        }

        if (entry->componentMask & (1 << COMPONENT_CAMERA)) {
            CameraComponent* camera = (CameraComponent*)ECS_GetComponent(world, i, COMPONENT_CAMERA);
            if (camera) entry->camera = *camera;
        }

        if (entry->componentMask & (1 << COMPONENT_INPUT)) {
            InputComponent* input = (InputComponent*)ECS_GetComponent(world, i, COMPONENT_INPUT);
            if (input) entry->input = *input;
        }
    }

    SceUtilitySavedataParam params;
    Scene_InitSavedataParams(&params);
    params.mode = PSP_UTILITY_SAVEDATA_LISTSAVE;
    params.overwrite = 0;
    strncpy(params.gameName, SAVE_GAME_NAME, sizeof(params.gameName) - 1);
    params.saveName[0] = '\0';
    strncpy(params.fileName, SAVE_FILE_NAME, sizeof(params.fileName) - 1);
    params.msFree = 0;
    params.focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST;
    params.saveNameList = g_saveSlotList;
    strncpy(params.sfoParam.title, SAVE_TITLE, sizeof(params.sfoParam.title) - 1);
    strncpy(params.sfoParam.savedataTitle, SAVE_TITLE, sizeof(params.sfoParam.savedataTitle) - 1);
    strncpy(params.sfoParam.detail, SAVE_DETAIL, sizeof(params.sfoParam.detail) - 1);
    params.dataBuf = (void*)saveData;
    params.dataSize = sizeof(SceneSaveData);
    params.dataBufSize = sizeof(SceneSaveData);

    bool result = Scene_RunSavedata(&params);
    Scene_Log(result ? "Scene_Save: success" : "Scene_Save: failed");
    free(saveData);
    return result;
}

bool Scene_Load(ECSWorld* world) {
    if (!world) return false;

    Scene_Log("Scene_Load: start");

    char populatedList[SAVE_SLOT_COUNT + 1][20];
    int populatedCount = Scene_BuildPopulatedSaveList(populatedList, SAVE_SLOT_COUNT + 1);
    if (populatedCount <= 0) {
        Scene_Log("Scene_Load: no saves found");
        return false;
    }

    SceneSaveData* saveData = (SceneSaveData*)malloc(sizeof(SceneSaveData));
    if (!saveData) return false;
    memset(saveData, 0, sizeof(SceneSaveData));

    SceUtilitySavedataParam params;
    Scene_InitSavedataParams(&params);
    params.mode = PSP_UTILITY_SAVEDATA_LISTLOAD;
    strncpy(params.gameName, SAVE_GAME_NAME, sizeof(params.gameName) - 1);
    params.saveName[0] = '\0';
    strncpy(params.fileName, SAVE_FILE_NAME, sizeof(params.fileName) - 1);
    params.focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST;
    params.saveNameList = populatedList;
    params.dataBuf = (void*)saveData;
    params.dataSize = sizeof(SceneSaveData);
    params.dataBufSize = sizeof(SceneSaveData);

    bool result = Scene_RunSavedata(&params);
    if (!result) {
        Scene_Log("Scene_Load: savedata failed");
        free(saveData);
        return false;
    }

    ECS_Cleanup(world);
    ECS_Init(world);

    for (int i = 0; i < saveData->activeCount; i++) {
        SceneEntitySave* entry = &saveData->entities[i];
        EntityID id = ECS_CreateEntity(world);
        if (id < 0) {
            free(saveData);
            return false;
        }

        if (entry->componentMask & (1 << COMPONENT_TRANSFORM)) {
            TransformComponent* transform = (TransformComponent*)ECS_AddComponent(world, id, COMPONENT_TRANSFORM);
            if (transform) *transform = entry->transform;
        }

        if (entry->componentMask & (1 << COMPONENT_RENDERABLE)) {
            RenderableComponent* renderable = (RenderableComponent*)ECS_AddComponent(world, id, COMPONENT_RENDERABLE);
            if (renderable) *renderable = entry->renderable;
        }

        if (entry->componentMask & (1 << COMPONENT_CAMERA)) {
            CameraComponent* camera = (CameraComponent*)ECS_AddComponent(world, id, COMPONENT_CAMERA);
            if (camera) *camera = entry->camera;
        }

        if (entry->componentMask & (1 << COMPONENT_INPUT)) {
            InputComponent* input = (InputComponent*)ECS_AddComponent(world, id, COMPONENT_INPUT);
            if (input) *input = entry->input;
        }
    }

    free(saveData);
    Scene_Log("Scene_Load: success");
    return true;
}
