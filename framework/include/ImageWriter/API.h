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
extern "C" __declspec(dllexport) bool ImageWriter_Scene_SetCameraTransform(HImageWriterInstance instance, PlainReply* pReply, Transform transform);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_GetCameraTransform(HImageWriterInstance instance, TransformReply* pReply);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddEntity(HImageWriterInstance instance, AddEntityReply* pReply, EntityData params);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_GetEntityData(HImageWriterInstance instance, EntityDataReply* pReply, uint64_t entityId);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveEntity(HImageWriterInstance instance, PlainReply* pReply, uint64_t entityId);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveAllEntities(HImageWriterInstance instance, PlainReply* pReply);