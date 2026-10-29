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
typedef bool (*PfnCore_Scene_IsSelfManagedRenderWindowOpen)(InstanceData);
typedef bool (*PfnCore_Scene_UpdateRenderConfig)(InstanceData, RenderConfig);
typedef bool (*PfnCore_Scene_UpdateScreenSpaceCameraControlRig)(InstanceData, ScreenSpaceControls);

// scene API
typedef bool (*PfnCore_Scene_SetCameraTransform)(InstanceData, PlainReply*, Transform);
typedef bool (*PfnCore_Scene_GetCameraTransform)(InstanceData, TransformReply*);
typedef bool (*PfnCore_Scene_AddEntity)(InstanceData, AddEntityReply*, EntityData);
typedef bool (*PfnCore_Scene_GetEntityData)(InstanceData, EntityDataReply*, int64_t);
typedef bool (*PfnCore_Scene_RemoveEntity)(InstanceData, PlainReply*, uint16_t);
typedef bool (*PfnCore_Scene_removeAllEntities)(InstanceData, PlainReply*);



struct InstanceData
{
    HMODULE hDll;

    PfnCore_Instance_Initialize pfnCore_Instance_Initialize;
    PfnCore_Instance_Dispose pfnCore_Instance_Dispose;

    PfnCore_Scene_Initialize pfnCore_Scene_Initialize;
    PfnCore_Scene_Dispose pfnCore_Scene_Dispose;
    PfnCore_Scene_UpdateFrame pfnCore_Scene_UpdateFrame;
    PfnCore_Scene_IsSelfManagedRenderWindowOpen pfnCore_Scene_IsSelfManagedRenderWindowOpen;
    PfnCore_Scene_UpdateRenderConfig pfnCore_Scene_UpdateRenderConfig;

    PfnCore_Scene_SetCameraTransform pfnCore_Scene_SetCameraTransform;
    PfnCore_Scene_GetCameraTransform pfnCore_Scene_GetCameraTransform;
    PfnCore_Scene_UpdateScreenSpaceCameraControlRig pfnCore_Scene_UpdateScreenSpaceCameraControlRig;
    PfnCore_Scene_AddEntity pfnCore_Scene_AddEntity;
    PfnCore_Scene_GetEntityData pfnCore_Scene_GetEntityData;
    PfnCore_Scene_RemoveEntity pfnCore_Scene_RemoveEntity;
    PfnCore_Scene_removeAllEntities pfnCore_Scene_removeAllEntities;

    void* pCoreData;
};