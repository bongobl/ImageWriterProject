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
extern "C" __declspec(dllexport) bool scene_updateScreenSpaceCameraControlRig(InstanceData instanceData, ScreenSpaceControls controls);

// scene API
extern "C" __declspec(dllexport) bool scene_setCameraTransform(InstanceData instanceData, PlainReply* pReply, Transform transform);
extern "C" __declspec(dllexport) bool scene_getCameraTransform(InstanceData instanceData, TransformReply* pReply);
extern "C" __declspec(dllexport) bool scene_addEntity(InstanceData instanceData, AddEntityReply* pReply, EntityData params);
extern "C" __declspec(dllexport) bool scene_getEntityData(InstanceData instanceData, EntityDataReply* pReply, uint64_t entityId);
extern "C" __declspec(dllexport) bool scene_removeEntity(InstanceData instanceData, PlainReply* pReply, uint64_t entityId);
extern "C" __declspec(dllexport) bool scene_removeAllEntities(InstanceData instanceData, PlainReply* pReply);

