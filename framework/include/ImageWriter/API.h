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

// App lifecycle
extern "C" __declspec(dllexport) bool CreateImageWriterInstance(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) bool DestroyImageWriterInstance(HImageWriterInstance* instance);

// AI API
extern "C" __declspec(dllexport) bool GetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform* pCameraTransform);
extern "C" __declspec(dllexport) bool AddEllipse(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color);
extern "C" __declspec(dllexport) bool AddRectangle(HImageWriterInstance instance, char* pStatusMessage, Transform transform, Color color);
extern "C" __declspec(dllexport) bool AddTriangle(HImageWriterInstance instance, char* pStatusMessage, Vec2 point1, Vec2 point2, Vec2 point3, Transform transform, Color color);
extern "C" __declspec(dllexport) bool ClearImage(HImageWriterInstance instance, char* pStatusMessage);

// Window lifecycle
extern "C" __declspec(dllexport) bool InitRenderWindow(HImageWriterInstance instance, int64_t windowHandle);
extern "C" __declspec(dllexport) bool UpdateRenderWindow(HImageWriterInstance instance, float deltaTime);
extern "C" __declspec(dllexport) bool DisposeRenderWindow(HImageWriterInstance instance);
extern "C" __declspec(dllexport) bool IsIsolatedRenderWindowOpen(HImageWriterInstance instance);

