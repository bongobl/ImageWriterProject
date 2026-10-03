#pragma once
#include <stdio.h>
#include <cstdint>

struct Vec2 {
    float x;
    float y;
};

struct ImageWriter
{
    void* pData;
};

// TODO: don't let scale go below or equal to 0, 
// rename angle to orientation and flop order with scale
struct Transform {
    Vec2 position;
    float scale;
    float angle;
};

struct Color {
    float red = 0;
    float green = 0;
    float blue = 0;
    float alpha = 0;
};

typedef ImageWriter HImageWriterInstance;

// instance
extern "C" __declspec(dllexport) bool ImageWriter_Instance_Intialize(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) bool ImageWriter_Instance_Dispose(HImageWriterInstance* pInstance);

// scene lifecycle
extern "C" __declspec(dllexport) bool ImageWriter_Scene_Init(HImageWriterInstance instance, int64_t windowHandle);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_Dispose(HImageWriterInstance instance);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_UpdateFrame(HImageWriterInstance instance, float deltaTime);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_IsSelfManagedRenderWindowOpen(HImageWriterInstance instance);

// scene API
extern "C" __declspec(dllexport) bool ImageWriter_Scene_SetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform transform);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_GetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform* pCameraTransform, float* pWidthFromheight);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_TEMP_MoveCameraLocalSpace(HImageWriterInstance instance, char* pStatusMessage, Transform delta);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddEllipse(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color, Vec2 halfExtents);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddRectangle(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color, Vec2 halfExtents);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddTriangle(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color, Vec2 point1, Vec2 point2, Vec2 point3);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveAllEntities(HImageWriterInstance instance, char* pStatusMessage);



