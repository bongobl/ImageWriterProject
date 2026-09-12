#pragma once
#include <stdio.h>
#include <cstdint>

struct ImageWriter
{
    void* pData;
};

typedef ImageWriter HImageWriterInstance;
// App lifecycle
extern "C" __declspec(dllexport) bool CreateImageWriterInstance(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) bool DestroyImageWriterInstance(HImageWriterInstance* instance);

// AI API
extern "C" __declspec(dllexport) bool DrawCircle(HImageWriterInstance instance, char* pStatusMessage, int centerX, int centerY, int radius);
extern "C" __declspec(dllexport) bool DrawRectangle(HImageWriterInstance instance, char* pStatusMessage, int centerX, int centerY, int halfExtentX, int halfExtentY);

// Window lifecycle
extern "C" __declspec(dllexport) bool InitRenderWindow(HImageWriterInstance instance, int64_t windowHandle);
extern "C" __declspec(dllexport) bool UpdateRenderWindow(HImageWriterInstance instance, float deltaTime);
extern "C" __declspec(dllexport) bool DisposeRenderWindow(HImageWriterInstance instance);
extern "C" __declspec(dllexport) bool IsIsolatedRenderWindowOpen(HImageWriterInstance instance);

