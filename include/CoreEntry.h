#pragma once
#include <API.h>

extern "C" __declspec(dllexport) void initialize(InstanceData* pInstanceData);
extern "C" __declspec(dllexport) void setupImage(InstanceData instanceData, int width, int height);
extern "C" __declspec(dllexport) void drawCircle(InstanceData instanceData, int centerX, int centerY, int radius);
extern "C" __declspec(dllexport) void drawRectangle(InstanceData instanceData, int centerX, int centerY, int halfExtentX, int halfExtentY);
extern "C" __declspec(dllexport) void exportImage(InstanceData instanceData);

extern "C" __declspec(dllexport) void dispose(InstanceData* pInstanceData);