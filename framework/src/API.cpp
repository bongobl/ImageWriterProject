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

    PfnCore_Scene_isSelfManagedRenderWindowOpen pfnCore_Scene_isSelfManagedRenderWindowOpen = (PfnCore_Scene_isSelfManagedRenderWindowOpen)GetProcAddress(hDll, "scene_isSelfManagedRenderWindowOpen");

    if (pfnCore_Scene_isSelfManagedRenderWindowOpen == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_isSelfManagedRenderWindowOpen could not load\n");
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

    PfnCore_Scene_TEMP_MoveCameraLocalSpace pfnCore_Scene_TEMP_MoveCameraLocalSpace = (PfnCore_Scene_TEMP_MoveCameraLocalSpace)GetProcAddress(hDll, "scene_TEMP_moveCameraLocalSpace");

    if (pfnCore_Scene_TEMP_MoveCameraLocalSpace == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_TEMP_moveCameraLocalSpace could not load\n");
        FreeLibrary(hDll);
        return false;
    }


    PfnCore_Scene_AddEllipse pfnCore_Scene_AddEllipse = (PfnCore_Scene_AddEllipse)GetProcAddress(hDll, "scene_addEllipse");

    if (pfnCore_Scene_AddEllipse == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_addEllipse could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_AddRectangle pfnCore_Scene_AddRectangle = (PfnCore_Scene_AddRectangle)GetProcAddress(hDll, "scene_addRectangle");

    if (pfnCore_Scene_AddRectangle == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_addRectangle could not load\n");
        FreeLibrary(hDll);
        return false;
    }

    PfnCore_Scene_AddTriangle pfnCore_Scene_AddTriangle = (PfnCore_Scene_AddTriangle)GetProcAddress(hDll, "scene_addTriangle");

    if (pfnCore_Scene_AddTriangle == NULL) {
        fprintf(stderr, "ImageWriter_Instance_Intialize: CoreEntry function scene_addTriangle could not load\n");
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
        .pfnCore_Scene_isSelfManagedRenderWindowOpen = pfnCore_Scene_isSelfManagedRenderWindowOpen,

        .pfnCore_Scene_SetCameraTransform = pfnCore_Scene_SetCameraTransform,
        .pfnCore_Scene_GetCameraTransform = pfnCore_Scene_GetCameraTransform,
        .pfnCore_Scene_TEMP_MoveCameraLocalSpace = pfnCore_Scene_TEMP_MoveCameraLocalSpace,
        .pfnCore_Scene_AddEllipse = pfnCore_Scene_AddEllipse,
        .pfnCore_Scene_AddRectangle = pfnCore_Scene_AddRectangle,
        .pfnCore_Scene_AddTriangle = pfnCore_Scene_AddTriangle,
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

extern "C" __declspec(dllexport) bool ImageWriter_Scene_Init(HImageWriterInstance instance, int64_t windowHandle)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_Init: instance.pData was null\n");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;

    return pInstanceData->pfnCore_Scene_Initialize(*pInstanceData, windowHandle);
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

    return pInstanceData->pfnCore_Scene_isSelfManagedRenderWindowOpen(*pInstanceData);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_SetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform transform)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_SetCameraTransform: instance.pData was null\n");
        strcpy(pStatusMessage, "App did not call ImageWriter_Scene_SetCameraTransform() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCore_Scene_SetCameraTransform(*pInstanceData, transform);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_GetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform* pCameraTransform)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_GetCameraTransform: instance.pData was null\n");
        strcpy(pStatusMessage, "App did not call ImageWriter_Scene_GetCameraTransform() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCore_Scene_GetCameraTransform(*pInstanceData, pCameraTransform);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_TEMP_MoveCameraLocalSpace(HImageWriterInstance instance, char* pStatusMessage, Transform delta)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_TEMP_MoveCameraLocalSpace: instance.pData was null\n");
        strcpy(pStatusMessage, "App did not call ImageWriter_Scene_TEMP_MoveCameraLocalSpace() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCore_Scene_TEMP_MoveCameraLocalSpace(*pInstanceData, delta);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddEllipse(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color, Vec2 halfExtents)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_AddEllipse: instance.pData was null\n");
        strcpy(pStatusMessage, "App did not call ImageWriter_Scene_AddEllipse() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCore_Scene_AddEllipse(*pInstanceData, transform, color, halfExtents);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddRectangle(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color, Vec2 halfExtents)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_AddRectangle: instance.pData was null\n");
        strcpy(pStatusMessage, "App did not call ImageWriter_Scene_AddRectangle() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCore_Scene_AddRectangle(*pInstanceData, transform, color, halfExtents);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddTriangle(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color, Vec2 point1, Vec2 point2, Vec2 point3)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_AddTriangle: instance.pData was null\n");
        strcpy(pStatusMessage, "App did not call ImageWriter_Scene_AddTriangle() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCore_Scene_AddTriangle(*pInstanceData, transform, color, point1, point2, point3);
}

extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveAllEntities(HImageWriterInstance instance, char* pStatusMessage)
{
    if (!instance.pData) {
        fprintf(stderr, "ImageWriter_Scene_RemoveAllEntities: instance.pData was null\n");
        strcpy(pStatusMessage, "App did not call ImageWriter_Scene_RemoveAllEntities() from its underlying framework correctly");
        return false;
    }

    InstanceData* pInstanceData = (InstanceData*)instance.pData;
    pInstanceData->pPublicStatusMessage = pStatusMessage;
    return pInstanceData->pfnCore_Scene_removeAllEntities(*pInstanceData);
}