#pragma once
#include <ImageWriter/API.h>
#include <ImageWriter/CoreAndAPIShared.h>
#include <ImageWriter/CoreCommon.h>

extern "C" __declspec(dllexport) bool initialize(InstanceData* pInstanceData);
extern "C" __declspec(dllexport) bool setupImage(InstanceData instanceData, int width, int height);
extern "C" __declspec(dllexport) bool drawCircle(InstanceData instanceData, int centerX, int centerY, int radius);
extern "C" __declspec(dllexport) bool drawRectangle(InstanceData instanceData, int centerX, int centerY, int halfExtentX, int halfExtentY);
extern "C" __declspec(dllexport) bool exportImage(InstanceData instanceData, const char* pImageName);

extern "C" __declspec(dllexport) bool initRenderWindow(InstanceData instanceData);
extern "C" __declspec(dllexport) bool updateRenderWindow(InstanceData instanceData);
extern "C" __declspec(dllexport) bool disposeRenderWindow(InstanceData instanceData);

extern "C" __declspec(dllexport) bool dispose(InstanceData* pInstanceData);
