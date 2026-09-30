#pragma once
#include <stdio.h>
#include <cstdint>

struct ImageWriter
{
    void* pData;
};

struct Transform {
    float positionX;
    float positionY;
    float scaleX;
    float scaleY;
    float angle;
};

struct Color {
    float red = 0;
    float green = 0;
    float blue = 0;
    float alpha = 0;
};

struct Vec2 {
    float x;
    float y;
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
extern "C" __declspec(dllexport) bool ImageWriter_Scene_GetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform* pCameraTransform);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_TEMP_MoveCameraLocalSpace(HImageWriterInstance instance, char* pStatusMessage, Transform delta);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddEllipse(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddRectangle(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_AddTriangle(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color, Vec2 point1, Vec2 point2, Vec2 point3);
extern "C" __declspec(dllexport) bool ImageWriter_Scene_RemoveAllEntities(HImageWriterInstance instance, char* pStatusMessage);



