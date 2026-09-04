#pragma once
#include <API.h>

extern "C" __declspec(dllexport) bool initialize(InstanceData* pInstanceData);
extern "C" __declspec(dllexport) bool setupImage(InstanceData instanceData, int width, int height);
extern "C" __declspec(dllexport) bool drawCircle(InstanceData instanceData, int centerX, int centerY, int radius);
extern "C" __declspec(dllexport) bool drawRectangle(InstanceData instanceData, int centerX, int centerY, int halfExtentX, int halfExtentY);
extern "C" __declspec(dllexport) bool exportImage(InstanceData instanceData, const char* pImageName);

extern "C" __declspec(dllexport) bool dispose(InstanceData* pInstanceData);