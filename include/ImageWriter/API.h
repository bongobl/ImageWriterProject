#pragma once
#include <stdio.h>

struct HImageWriterInstance
{
    void* pData;
};
extern "C" __declspec(dllexport) bool CreateImageWriterInstance(HImageWriterInstance* pInstance);
extern "C" __declspec(dllexport) bool SetupImage(HImageWriterInstance instance, int width, int height);
extern "C" __declspec(dllexport) bool DrawCircle(HImageWriterInstance instance, int centerX, int centerY, int radius);
extern "C" __declspec(dllexport) bool DrawRectangle(HImageWriterInstance instance, int centerX, int centerY, int halfExtentX, int halfExtentY);
extern "C" __declspec(dllexport) bool ExportImage(HImageWriterInstance instance, const char* pImageName);
extern "C" __declspec(dllexport) bool DestroyImageWriterInstance(HImageWriterInstance* instance);