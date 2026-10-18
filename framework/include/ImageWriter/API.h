#pragma once
#include <stdio.h>
#include <cstdint>
#include <ImageWriter/Common.h>

struct Framework
{
    void* pData;
};

enum GridViewMode {
    GRID_VIEW_MODE_NONE = 0,
    GRID_VIEW_MODE_FOREGROUND = 1,
    GRID_VIEW_MODE_BACKGROUND = 2,
};

struct RenderConfig {
    int gridViewMode;
};

typedef Framework HImageWriterInstance;

// instance
extern "C" __declspec(dllexport) bool ImageWriter_Instance_Intialize(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) bool ImageWriter_Instance_Dispose(HImageWriterInstance* pInstance);

// scene lifecycle
extern "C" __declspec(dllexport) bool ImageWriter_Scene_Init(HImageWriterInstance instance, int64_t windowHandle, RenderConfig renderConfig);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_Dispose(HImageWriterInstance instance);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_UpdateFrame(HImageWriterInstance instance, float deltaTime);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_IsSelfManagedRenderWindowOpen(HImageWriterInstance instance);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_UpdateRenderConfig(HImageWriterInstance instance, RenderConfig renderConfig);

// scene API
extern "C" __declspec(dllexport) bool ImageWriter_Scene_SetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform transform);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_GetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform* pCameraTransform, float* pWidthFromheight);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_TEMP_MoveCameraLocalSpace(HImageWriterInstance instance, char* pStatusMessage, Transform delta);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddEntity(HImageWriterInstance instance, char* pStatusMessage, AddEntityParams params);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveEntity(HImageWriterInstance instance, char* pStatusMessage, uint64_t id);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveAllEntities(HImageWriterInstance instance, char* pStatusMessage);



