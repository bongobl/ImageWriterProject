#pragma once
#include <ImageWriter/API.h>
#include <ImageWriter/CoreAndAPIShared.h>
#include <ImageWriter/CoreCommon.h>
#include <cstdint>

// instance
extern "C" __declspec(dllexport) bool instance_initialize(InstanceData* pInstanceData);
extern "C" __declspec(dllexport) bool instance_dispose(InstanceData* pInstanceData);

// scene lifecycle
extern "C" __declspec(dllexport) bool scene_initialize(InstanceData instanceData, int64_t windowHandle);
extern "C" __declspec(dllexport) bool scene_dispose(InstanceData instanceData);
extern "C" __declspec(dllexport) bool scene_updateFrame(InstanceData instanceData, float deltaTime);
extern "C" __declspec(dllexport) bool scene_isSelfManagedRenderWindowOpen(InstanceData instanceData);

// scene API
extern "C" __declspec(dllexport) bool scene_getCameraTransform(InstanceData instanceData, Transform* pCameraTransform);
extern "C" __declspec(dllexport) bool scene_addEllipse(InstanceData instanceData, Transform transform, Color color);
extern "C" __declspec(dllexport) bool scene_addRectangle(InstanceData instanceData, Transform transform, Color color);
extern "C" __declspec(dllexport) bool scene_addTriangle(InstanceData instanceData, Transform transform, Color color, Vec2 point1, Vec2 point2, Vec2 point3);
extern "C" __declspec(dllexport) bool scene_removeAllEntities(InstanceData instanceData);

