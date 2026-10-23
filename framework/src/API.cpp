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

    PfnCore_Scene_RemoveEntity pfnCore_Scene_RemoveEntity = (PfnCore_Scene_RemoveEntity)GetProcAddress(hDll, "scene_removeEntity");

    if (pfnCore_Scene_RemoveEntity == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_removeEntity could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_removeAllEntities pfnCore_Scene_removeAllEntities = (PfnCore_Scene_removeAllEntities)GetProcAddress(hDll, "scene_removeAllEntities");

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
        .pfnCore_Scene_RemoveEntity = pfnCore_Scene_RemoveEntity,
        .pfnCore_Scene_removeAllEntities = pfnCore_Scene_removeAllEntities,
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

extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddEntity(HImageWriterInstance instance, AddEntityReply* pReply, AddEntityParams params)
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

extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveEntity(HImageWriterInstance instance, PlainReply* pReply, uint64_t id)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_RemoveEntity: instance.pData was null\n");
        pReply->success = false;
        strcpy(pReply->message, "App did not call ImageWriter_Scene_RemoveEntity() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    return pInstanceData->pfnCore_Scene_RemoveEntity(*pInstanceData, pReply, id);
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
    return pInstanceData->pfnCore_Scene_removeAllEntities(*pInstanceData, pReply);
}