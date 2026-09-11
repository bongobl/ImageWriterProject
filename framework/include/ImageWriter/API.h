#pragma once
#include <stdio.h>

struct HImageWriterInstance
{
    void* pData;
};
extern "C" __declspec(dllexport) bool CreateImageWriterInstance(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) bool DrawCircle(HImageWriterInstance instance, char* pStatusMessage, int centerX, int centerY, int radius);
extern "C" __declspec(dllexport) bool DrawRectangle(HImageWriterInstance instance, char* pStatusMessage, int centerX, int centerY, int halfExtentX, int halfExtentY);

// TODO: remove status message params from window functions since they aren't part of the MCP call path
extern "C" __declspec(dllexport) bool InitRenderWindow(HImageWriterInstance instance, char* pStatusMessage);
extern "C" __declspec(dllexport) bool UpdateRenderWindow(HImageWriterInstance instance, char* pStatusMessage, float deltaTime);
extern "C" __declspec(dllexport) bool DisposeRenderWindow(HImageWriterInstance instance, char* pStatusMessage);
extern "C" __declspec(dllexport) bool TEMP_IsRenderWindowOpen(HImageWriterInstance instance, char* pStatusMessage);

extern "C" __declspec(dllexport) bool DestroyImageWriterInstance(HImageWriterInstance* instance);