#pragma once
#include <windows.h>
#include <cstdint>
#include <ImageWriter/API.h>

// Included by API.cpp, No constructs allowed here


struct InstanceData;

// instance
typedef bool (*PfnCore_Instance_Initialize)(InstanceData*);
typedef bool (*PfnCore_Instance_Dispose)(InstanceData*);

// scene lifecycle
typedef bool (*PfnCore_Scene_Initialize)(InstanceData, int64_t, RenderConfig);
typedef bool (*PfnCore_Scene_Dispose)(InstanceData);
typedef bool (*PfnCore_Scene_UpdateFrame)(InstanceData, float);
typedef bool (*PfnCore_Scene_isSelfManagedRenderWindowOpen)(InstanceData);
typedef bool (*PfnCore_Scene_updateRenderConfig)(InstanceData, RenderConfig);


// scene API
typedef bool (*PfnCore_Scene_SetCameraTransform)(InstanceData, Transform);
typedef bool (*PfnCore_Scene_GetCameraTransform)(InstanceData, Transform*, float*);
typedef bool (*PfnCore_Scene_TEMP_MoveCameraLocalSpace)(InstanceData, Transform);
typedef bool (*PfnCore_Scene_AddEntity)(InstanceData, AddEntityParams);
typedef bool (*PfnCore_Scene_RemoveEntity)(InstanceData, uint16_t);
typedef bool (*PfnCore_Scene_removeAllEntities)(InstanceData);



struct InstanceData
{
    HMODULE hDll;

    PfnCore_Instance_Initialize pfnCore_Instance_Initialize;
    PfnCore_Instance_Dispose pfnCore_Instance_Dispose;

    PfnCore_Scene_Initialize pfnCore_Scene_Initialize;
    PfnCore_Scene_Dispose pfnCore_Scene_Dispose;
    PfnCore_Scene_UpdateFrame pfnCore_Scene_UpdateFrame;
    PfnCore_Scene_isSelfManagedRenderWindowOpen pfnCore_Scene_isSelfManagedRenderWindowOpen;
    PfnCore_Scene_updateRenderConfig pfnCore_Scene_updateRenderConfig;

    PfnCore_Scene_SetCameraTransform pfnCore_Scene_SetCameraTransform;
    PfnCore_Scene_GetCameraTransform pfnCore_Scene_GetCameraTransform;
    PfnCore_Scene_TEMP_MoveCameraLocalSpace pfnCore_Scene_TEMP_MoveCameraLocalSpace;
    PfnCore_Scene_AddEntity pfnCore_Scene_AddEntity;
    PfnCore_Scene_RemoveEntity pfnCore_Scene_RemoveEntity;
    PfnCore_Scene_removeAllEntities pfnCore_Scene_removeAllEntities;

    void* pCoreData;
    char* pPublicStatusMessage;
};