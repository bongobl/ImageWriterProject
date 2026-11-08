#include <ImageWriter/API.h>
#include <ImageWriter/CoreAndAPIShared.h>

// Note: It is important that this file remains free of any C++ constructs or else GNU won't
// be able to build it into a dll that python can load

extern "C" __declspec(dllexport) bool ImageWriter_Instance_Intialize(HImageWriterInstance* pInstance)
{
    if(!pInstance){
        fprintf(stderr, "ImageWriter_Instance_Intialize: pInstance was null\n");
        return false;
    }

    HMODULE hDll = LoadLibrary("ImageWriterCore");

    if(!hDll){
        fprintf(stderr, "ImageWriter_Instance_Intialize: ImageWriterCore library could not load\n");
        return false;
    }
    
    PfnCore_Instance_Initialize pfnCore_Instance_Initialize = (PfnCore_Instance_Initialize)GetProcAddress(hDll, "instance_initialize");

    if(pfnCore_Instance_Initialize == NULL){
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function instance_initialize could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Instance_Dispose pfnCore_Instance_Dispose = (PfnCore_Instance_Dispose)GetProcAddress(hDll, "instance_dispose");

    if (pfnCore_Instance_Dispose == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function instance_dispose could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_Initialize pfnCore_Scene_Initialize = (PfnCore_Scene_Initialize)GetProcAddress(hDll, "scene_initialize");

    if (pfnCore_Scene_Initialize == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_initialize could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_Dispose pfnCore_Scene_Dispose = (PfnCore_Scene_Dispose)GetProcAddress(hDll, "scene_dispose");

    if (pfnCore_Scene_Dispose == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_dispose could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_UpdateFrame pfnCore_Scene_UpdateFrame = (PfnCore_Scene_UpdateFrame)GetProcAddress(hDll, "scene_updateFrame");

    if (pfnCore_Scene_UpdateFrame == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_updateFrame could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_IsSelfManagedRenderWindowOpen pfnCore_Scene_IsSelfManagedRenderWindowOpen = (PfnCore_Scene_IsSelfManagedRenderWindowOpen)GetProcAddress(hDll, "scene_isSelfManagedRenderWindowOpen");

    if (pfnCore_Scene_IsSelfManagedRenderWindowOpen == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_isSelfManagedRenderWindowOpen could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_UpdateRenderConfig pfnCore_Scene_UpdateRenderConfig = (PfnCore_Scene_UpdateRenderConfig)GetProcAddress(hDll, "scene_updateRenderConfig");

    if (pfnCore_Scene_UpdateRenderConfig == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_updateRenderConfig could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_SetCameraTransform pfnCore_Scene_SetCameraTransform = (PfnCore_Scene_SetCameraTransform)GetProcAddress(hDll, "scene_setCameraTransform");

    if (pfnCore_Scene_SetCameraTransform == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_getCameraTransform could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_GetCameraTransform pfnCore_Scene_GetCameraTransform = (PfnCore_Scene_GetCameraTransform)GetProcAddress(hDll, "scene_getCameraTransform");

    if (pfnCore_Scene_GetCameraTransform == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_getCameraTransform could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_UpdateScreenSpaceCameraControlRig pfnCore_Scene_UpdateScreenSpaceCameraControlRig = (PfnCore_Scene_UpdateScreenSpaceCameraControlRig)GetProcAddress(hDll, "scene_updateScreenSpaceCameraControlRig");

    if (pfnCore_Scene_UpdateScreenSpaceCameraControlRig == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_updateScreenSpaceCameraControlRig could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_AddEntity pfnCore_Scene_AddEntity = (PfnCore_Scene_AddEntity)GetProcAddress(hDll, "scene_addEntity");

    if (pfnCore_Scene_AddEntity == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_addEntity could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_ModifyEntity pfnCore_Scene_ModifyEntity = (PfnCore_Scene_ModifyEntity)GetProcAddress(hDll, "scene_modifyEntity");

    if (pfnCore_Scene_AddEntity == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_modifyEntity could not load\n");
        FreeLibrary(hDll);
        return false;
    }
    
    PfnCore_Scene_GetEntityData pfnCore_Scene_GetEntityData = (PfnCore_Scene_GetEntityData)GetProcAddress(hDll, "scene_getEntityData");

    if (pfnCore_Scene_GetEntityData == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_getEntityData could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_RemoveEntity pfnCore_Scene_RemoveEntity = (PfnCore_Scene_RemoveEntity)GetProcAddress(hDll, "scene_removeEntity");

    if (pfnCore_Scene_RemoveEntity == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_removeEntity could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_RemoveAllEntities pfnCore_Scene_removeAllEntities = (PfnCore_Scene_RemoveAllEntities)GetProcAddress(hDll, "scene_removeAllEntities");

    if (pfnCore_Scene_removeAllEntities == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_removeAllEntities could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    pInstance->pData = (InstanceData*)malloc(sizeof(InstanceData));
    InstanceData* pInstanceData = (InstanceData*)pInstance->pData;

    *pInstanceData = {
        .hDll = hDll,
        .pfnCore_Instance_Initialize = pfnCore_Instance_Initialize,
        .pfnCore_Instance_Dispose = pfnCore_Instance_Dispose,

        .pfnCore_Scene_Initialize = pfnCore_Scene_Initialize,
        .pfnCore_Scene_Dispose = pfnCore_Scene_Dispose,
        .pfnCore_Scene_UpdateFrame = pfnCore_Scene_UpdateFrame,
        .pfnCore_Scene_IsSelfManagedRenderWindowOpen = pfnCore_Scene_IsSelfManagedRenderWindowOpen,
        .pfnCore_Scene_UpdateRenderConfig = pfnCore_Scene_UpdateRenderConfig,

        .pfnCore_Scene_SetCameraTransform = pfnCore_Scene_SetCameraTransform,
        .pfnCore_Scene_GetCameraTransform = pfnCore_Scene_GetCameraTransform,
        .pfnCore_Scene_UpdateScreenSpaceCameraControlRig = pfnCore_Scene_UpdateScreenSpaceCameraControlRig,
        .pfnCore_Scene_AddEntity = pfnCore_Scene_AddEntity,
        .pfnCore_Scene_ModifyEntity = pfnCore_Scene_ModifyEntity,
        .pfnCore_Scene_GetEntityData = pfnCore_Scene_GetEntityData,
        .pfnCore_Scene_RemoveEntity = pfnCore_Scene_RemoveEntity,
        .pfnCore_Scene_RemoveAllEntities = pfnCore_Scene_removeAllEntities,
    };

    // initialize core
    return pInstanceData->pfnCore_Instance_Initialize(pInstanceData);
}

extern "C" __declspec(dllexport) bool ImageWriter_Instance_Dispose(HImageWriterInstance* pInstance)
{
    if (!pInstance || !pInstance->pData) {
        fprintf(stderr, "ImageWriter_Instance_Dispose: instance or instance->pData was null\n");
        return false;
    }
    InstanceData* pInstanceData = (InstanceData*)pInstance->pData;

    bool coreDisposeResult = pInstanceData->pfnCore_Instance_Dispose(pInstanceData);

    FreeLibrary(pInstanceData->hDll);

    free(pInstance->pData);
    pInstance->pData = nullptr;

    return coreDisposeResult;
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_Init(HImageWriterInstance instance, int64_t windowHandle, RenderConfig renderConfig)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_Init: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    return pInstanceData->pfnCore_Scene_Initialize(*pInstanceData, windowHandle, renderConfig);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_Dispose(HImageWriterInstance instance)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_Dispose: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    return pInstanceData->pfnCore_Scene_Dispose(*pInstanceData);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_UpdateFrame(HImageWriterInstance instance, float deltaTime)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_UpdateFrame: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    return pInstanceData->pfnCore_Scene_UpdateFrame(*pInstanceData, deltaTime);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_IsSelfManagedRenderWindowOpen(HImageWriterInstance instance)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_IsSelfManagedRenderWindowOpen: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    return pInstanceData->pfnCore_Scene_IsSelfManagedRenderWindowOpen(*pInstanceData);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_UpdateRenderConfig(HImageWriterInstance instance, RenderConfig renderConfig)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_UpdateRenderConfig: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_UpdateRenderConfig(*pInstanceData, renderConfig);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_SetCameraTransform(HImageWriterInstance instance, PlainReply* pReply, Transform transform)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_SetCameraTransform: instance.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_SetCameraTransform() from its underlying framework correctly");
        return false;
    }
    
    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_SetCameraTransform(*pInstanceData, pReply, transform);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_GetCameraTransform(HImageWriterInstance instance, TransformReply* pReply)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_GetCameraTransform: instance.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_GetCameraTransform() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_GetCameraTransform(*pInstanceData, pReply);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_UpdateScreenSpaceCameraControlRig(HImageWriterInstance instance, ScreenSpaceControls controls)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_UpdateScreenSpaceCameraControlRig: instance.pData was null\n");
        return false;
    }
    
    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_UpdateScreenSpaceCameraControlRig(*pInstanceData, controls);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddEntity(HImageWriterInstance instance, AddEntityReply* pReply, EntityData params)
{
    if(!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_AddEntity: instance.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_AddEntity() from its underlying framework correctly");
        return false;
    }
    
    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_AddEntity(*pInstanceData, pReply, params);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_ModifyEntity(HImageWriterInstance instance, PlainReply* pReply, EntityData params)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_ModifyEntity: instance.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_ModifyEntity() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_ModifyEntity(*pInstanceData, pReply, params);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_GetEntityData(HImageWriterInstance instance, EntityDataReply* pReply, uint64_t entityId)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_GetEntityData: instance.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_GetEntityData() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_GetEntityData(*pInstanceData, pReply, entityId);
}
extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveEntity(HImageWriterInstance instance, PlainReply* pReply, uint64_t entityId)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_RemoveEntity: instance.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_RemoveEntity() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_RemoveEntity(*pInstanceData, pReply, entityId);
}
extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveAllEntities(HImageWriterInstance instance, PlainReply* pReply)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_RemoveAllEntities: instance.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_RemoveAllEntities() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_RemoveAllEntities(*pInstanceData, pReply);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_MakeSnapshot(HImageWriterInstance instance, PlainReply* pReply, HSnapshot* pSnapshot)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_GetSnapshot: instance.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_GetSnapshot() from its underlying framework correctly");
        return false;
    }

    if (!pSnapshot) {
        fprintf(stderr, "ImageWriter_Scene_GetSnapshot: pSnapshot was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_GetSnapshot() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    pSnapshot->pData = (SnapshotData*)malloc(sizeof(SnapshotData));
    SnapshotData* pSnapshotData = (SnapshotData*)pSnapshot->pData;

    // Fill with camera data
    TransformReply cameraTransformReply;
    pInstanceData->pfnCore_Scene_GetCameraTransform(*pInstanceData, &cameraTransformReply);
    pSnapshotData->cameraTransform = cameraTransformReply.transform;
    pSnapshotData->cameraWidthFromHeight = cameraTransformReply.cameraWidthFromHeight;

    // Fill with entity data
    pSnapshotData->numEntities = 4;
    pSnapshotData->pEntitiesData = (EntityData*)malloc(pSnapshotData->numEntities * sizeof(EntityData));

    pSnapshotData->pEntitiesData[0] = {
        .entityType = ENTITY_TYPE_ELLIPSE,
        .transform = { .position = { .x = 4.6f, .y = 3.2f }, .orientation = 4.6f, .scale = 8.32f },
        .color = { .red = 0.2f, .green = 0.3f, .blue = 0.8f, .alpha = 1.0f },
        .zOrder = 5,
        .ellipse = { .halfExtents = { 2.7, 3.2 } },
        .entityIdToModify = 27,
    };

    pSnapshotData->pEntitiesData[1] = {
        .entityType = ENTITY_TYPE_RECTANGLE,
        .transform = { .position = { .x = 7.4f, .y = 1.3f }, .orientation = 3.7f, .scale = 9.6f },
        .color = { .red = 0.0f, .green = 0.4f, .blue = 0.3f, .alpha = 0.5f },
        .zOrder = 45,
        .rectangle = { .halfExtents = { 2.8f, 6.4f } },
        .entityIdToModify = 64,
    };

    pSnapshotData->pEntitiesData[2] = {
        .entityType = ENTITY_TYPE_TRIANGLE,
        .transform = { .position = { .x = 10.5f, .y = 11.1f }, .orientation = 9.5f, .scale = 13.8f },
        .color = { .red = 0.3f, .green = 0.5f, .blue = 0.8f, .alpha = 0.2f },
        .zOrder = 32,
        .triangle = {
            .point1 = { 3.9, 7.8 },
            .point2 = { 1.8, 8.7 },
            .point3 = { 9.8, 5.4 },
        },
        .entityIdToModify = 375,
    };

    pSnapshotData->pEntitiesData[3] = {
        .entityType = ENTITY_TYPE_ELLIPSE,
        .transform = { .position = { .x = 12.56f, .y = 90.83f }, .orientation = 44.67f, .scale = 83.65f },
        .color = { .red = 0.73f, .green = 0.84f, .blue = 0.92f, .alpha = 1.0f },
        .zOrder = 12,
        .ellipse = { .halfExtents = { 83.76f, 93.27f } },
        .entityIdToModify = 89,
    };

    return true;
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_DisposeSnapshot(HSnapshot* pSnapshot, PlainReply* pReply)
{

    if (!pSnapshot || !pSnapshot->pData) {
        fprintf(stderr, "ImageWriter_Scene_DisposeSnapshot: either pSnapshot or pSnapshot->pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_DisposeSnapshot() from its underlying framework correctly");
        return false;
    }
    
    SnapshotData* pSnapshotData = (SnapshotData*)pSnapshot->pData;

    free(pSnapshotData);
    pSnapshot->pData = nullptr;

    return true;
}

extern "C" __declspec(dllexport) bool ImageWriter_Snapshot_GetNumEntities(HSnapshot snapshot, PlainReply* pReply, uint64_t* pNumEntities)
{
    if (!snapshot.pData) {
        fprintf(stderr, "ImageWriter_Snapshot_GetNumEntities: snapshot.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Snapshot_GetNumEntities() from its underlying framework correctly");
        return false;
    }

    SnapshotData* pSnapshotData = (SnapshotData*)snapshot.pData;
    *pNumEntities = pSnapshotData->numEntities;
    return true;
}

extern "C" __declspec(dllexport) bool ImageWriter_Snapshot_ReadData(
    HSnapshot snapshot, PlainReply* pReply, Transform* pCameraTransform,
    float* pCameraWidthFromHeight, uint64_t* pNumEntities, EntityData* pEntitiesData)
{
    if (!snapshot.pData) {
        fprintf(stderr, "ImageWriter_Snapshot_ReadData: snapshot.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Snapshot_ReadData() from its underlying framework correctly");
        return false;
    }

    SnapshotData* pSnapshotData = (SnapshotData*)snapshot.pData;

    *pCameraTransform = pSnapshotData->cameraTransform;
    *pCameraWidthFromHeight = pSnapshotData->cameraWidthFromHeight;
    *pNumEntities = pSnapshotData->numEntities;
    memcpy(pEntitiesData, pSnapshotData->pEntitiesData, pSnapshotData->numEntities * sizeof(EntityData));
    return true;
}