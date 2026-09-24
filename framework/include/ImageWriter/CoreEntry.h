#pragma once
#include <ImageWriter/API.h>
#include <ImageWriter/CoreAndAPIShared.h>
#include <ImageWriter/CoreCommon.h>
#include <cstdint>

extern "C" __declspec(dllexport) bool initialize(InstanceData* pInstanceData);

extern "C" __declspec(dllexport) bool getCameraTransform(InstanceData instanceData, Transform* pCameraTransform);
extern "C" __declspec(dllexport) bool addEllipse(InstanceData instanceData, Transform transform, Color color);
extern "C" __declspec(dllexport) bool addRectangle(InstanceData instanceData, Transform transform, Color color);
extern "C" __declspec(dllexport) bool addTriangle(InstanceData instanceData, Transform transform, Color color, Vec2 point1, Vec2 point2, Vec2 point3);
extern "C" __declspec(dllexport) bool clearImage(InstanceData instanceData);

extern "C" __declspec(dllexport) bool initRenderWindow(InstanceData instanceData, int64_t windowHandle);
extern "C" __declspec(dllexport) bool updateRenderWindow(InstanceData instanceData, float deltaTime);
extern "C" __declspec(dllexport) bool disposeRenderWindow(InstanceData instanceData);
extern "C" __declspec(dllexport) bool isIsolatedRenderWindowOpen(InstanceData instanceData);

extern "C" __declspec(dllexport) bool dispose(InstanceData* pInstanceData);
