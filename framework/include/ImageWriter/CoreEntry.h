#pragma once
#include <ImageWriter/API.h>
#include <ImageWriter/CoreAndAPIShared.h>
#include <ImageWriter/CoreCommon.h>
#include <cstdint>

// instance
extern "C" __declspec(dllexport) bool instance_initialize(InstanceData* pInstanceData);
extern "C" __declspec(dllexport) bool instance_dispose(InstanceData* pInstanceData);

// scene lifecycle
extern "C" __declspec(dllexport) bool scene_initialize(InstanceData instanceData, int64_t windowHandle, RenderConfig renderConfig);
extern "C" __declspec(dllexport) bool scene_dispose(InstanceData instanceData);
extern "C" __declspec(dllexport) bool scene_updateFrame(InstanceData instanceData, float deltaTime);
extern "C" __declspec(dllexport) bool scene_isSelfManagedRenderWindowOpen(InstanceData instanceData);
extern "C" __declspec(dllexport) bool scene_updateRenderConfig(InstanceData instanceData, RenderConfig renderConfig);

// scene API
extern "C" __declspec(dllexport) bool scene_setCameraTransform(InstanceData instanceData, Transform transform);
extern "C" __declspec(dllexport) bool scene_getCameraTransform(InstanceData instanceData, Transform* pCameraTransform, float* pWidthFromheight);
extern "C" __declspec(dllexport) bool scene_TEMP_moveCameraLocalSpace(InstanceData instanceData, Transform delta);
extern "C" __declspec(dllexport) bool scene_addEntity(InstanceData instanceData, AddEntityParams params);
extern "C" __declspec(dllexport) bool scene_removeEntity(InstanceData instanceData, uint64_t id);
extern "C" __declspec(dllexport) bool scene_removeAllEntities(InstanceData instanceData);

