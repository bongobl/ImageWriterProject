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

typedef ImageWriter HImageWriterInstance;

// App lifecycle
extern "C" __declspec(dllexport) bool CreateImageWriterInstance(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) bool DestroyImageWriterInstance(HImageWriterInstance* instance);

// AI API
extern "C" __declspec(dllexport) bool GetCameraTransform(HImageWriterInstance instance, char* pStatusMessage, Transform* pCameraTransform);
extern "C" __declspec(dllexport) bool AddEllipse(HImageWriterInstance instance, char* pStatusMessage, const Transform* pTransform);
extern "C" __declspec(dllexport) bool AddRectangle(HImageWriterInstance instance, char* pStatusMessage, const Transform* pCameraTransform);
extern "C" __declspec(dllexport) bool ClearImage(HImageWriterInstance instance, char* pStatusMessage);

// Window lifecycle
extern "C" __declspec(dllexport) bool InitRenderWindow(HImageWriterInstance instance, int64_t windowHandle);
extern "C" __declspec(dllexport) bool UpdateRenderWindow(HImageWriterInstance instance, float deltaTime);
extern "C" __declspec(dllexport) bool DisposeRenderWindow(HImageWriterInstance instance);
extern "C" __declspec(dllexport) bool IsIsolatedRenderWindowOpen(HImageWriterInstance instance);

