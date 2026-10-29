#include <iostream>
#include <ImageWriter/CoreEntry.h>
#include <ImageWriter/Scene.h>
#include <algorithm>

extern "C" __declspec(dllexport) bool instance_initialize(InstanceData* pInstanceData)
{
	if (!pInstanceData) {
		std::cerr << "Core entry instance_initialize(): pInstanceData was null" << std::endl;
		return false;
	}
	pInstanceData->pCoreData = new CoreData();

	return true;
}

extern "C" __declspec(dllexport) bool instance_dispose(InstanceData* pInstanceData)
{
	if (!pInstanceData) {
		std::cerr << "Core entry instance_dispose(): pInstanceData was null" << std::endl;
		return false;
	}

	if (!pInstanceData->pCoreData) {
		std::cerr << "Core entry instance_dispose(): pInstanceData->pCoreData was null" << std::endl;
		return false;
	}

	delete static_cast<CoreData*>(pInstanceData->pCoreData);
	pInstanceData->pCoreData = nullptr;
	return true;
}

extern "C" __declspec(dllexport) bool scene_initialize(InstanceData instanceData, int64_t windowHandle, RenderConfig renderConfig)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_initialize(): instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::unique_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);
	pCoreData->m_pScene = new Scene(windowHandle, renderConfig);

	return true;
}

extern "C" __declspec(dllexport) bool scene_dispose(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_dispose(): instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::unique_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);
	delete pCoreData->m_pScene;
	pCoreData->m_pScene = nullptr;
	return true;
}

extern "C" __declspec(dllexport) bool scene_updateFrame(InstanceData instanceData, float deltaTime)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_updateFrame(): instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_updateFrame(): pCoreData->m_pScene was null" << std::endl;
		return false;
	}

	pCoreData->m_pScene->updateState(deltaTime);
	pCoreData->m_pScene->render();
	return true;
}

extern "C" __declspec(dllexport) bool scene_isSelfManagedRenderWindowOpen(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_isSelfManagedRenderWindowOpen(): instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_isSelfManagedRenderWindowOpen(): pCoreData->m_pScene was null" << std::endl;
		return false;
	}
	return pCoreData->m_pScene->isSelfManagedRenderWindowOpen();

}

extern "C" __declspec(dllexport) bool scene_updateRenderConfig(InstanceData instanceData, RenderConfig renderConfig)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_isSelfManagedRenderWindowOpen(): instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_isSelfManagedRenderWindowOpen(): pCoreData->m_pScene was null" << std::endl;
		return false;
	}

	pCoreData->m_pScene->updateRenderConfig(renderConfig);
	return true;
}

extern "C" __declspec(dllexport) bool scene_updateScreenSpaceCameraControlRig(InstanceData instanceData, ScreenSpaceControls controls)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_TEMP_moveCameraLocalSpace(): instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_TEMP_moveCameraLocalSpace(): pCoreData->m_pScene was null" << std::endl;
		return false;
	}

	pCoreData->m_pScene->updateScreenSpaceCameraControlRig(controls);
	return true;
}

extern "C" __declspec(dllexport) bool scene_setCameraTransform(InstanceData instanceData, PlainReply* pReply, Transform transform)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_setCameraTransform(): instanceData.pCoreData was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework on scene_setCameraTransform()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_setCameraTransform(): pCoreData->m_pScene was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework: calling scene_setCameraTransform() on non-existent scene");
		return false;
	}

	Camera* pCamera = pCoreData->m_pScene->getCamera();
	pCamera->API_setTransform(pReply, transform);
	return pReply->success;
}

extern "C" __declspec(dllexport) bool scene_getCameraTransform(InstanceData instanceData, TransformReply* pReply)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_getCameraTransform(): instanceData.pCoreData was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework on scene_getCameraTransform()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_getCameraTransform(): pCoreData->m_pScene was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework: calling scene_getCameraTransform() on non-existent scene");
		return false;
	}

	const Camera* pCamera = pCoreData->m_pScene->getCamera();
	pCamera->API_getTransform(pReply);
	return pReply->success;
}

extern "C" __declspec(dllexport) bool scene_addEntity(InstanceData instanceData, AddEntityReply* pReply, EntityData params)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_addEntity(): instanceData.pCoreData was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework on scene_addEntity()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_addEntity(): pCoreData->m_pScene was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework: calling scene_addEntity() on non-existent scene");
		return false;
	}

	EntityList* pEntityList = pCoreData->m_pScene->getEntityList();
	pEntityList->API_addEntity(pReply, params);
	return pReply->success;
}

extern "C" __declspec(dllexport) bool scene_getEntityData(InstanceData instanceData, EntityDataReply* pReply, uint64_t entityId)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_removeEntity(): instanceData.pCoreData was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework on scene_removeEntity()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_removeEntity(): pCoreData->m_pScene was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework: calling scene_removeEntity() on non-existent scene");
		return false;
	}

	EntityList* pEntityList = pCoreData->m_pScene->getEntityList();
	pEntityList->API_getEntityData(pReply, entityId);
	return pReply->success;
}

extern "C" __declspec(dllexport) bool scene_removeEntity(InstanceData instanceData, PlainReply* pReply, uint64_t entityId)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_removeEntity(): instanceData.pCoreData was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework on scene_removeEntity()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_removeEntity(): pCoreData->m_pScene was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework: calling scene_removeEntity() on non-existent scene");
		return false;
	}

	EntityList* pEntityList = pCoreData->m_pScene->getEntityList();
	pEntityList->API_removeEntity(pReply, entityId);
	return pReply->success;
}

extern "C" __declspec(dllexport) bool scene_removeAllEntities(InstanceData instanceData, PlainReply* pReply)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_removeAllEntities(): instanceData.pCoreData was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework on scene_removeAllEntities()");
		return false;
	}
	
	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_removeAllEntities(): pCoreData->m_pScene was null" << std::endl;
		pReply->success = false;
		strcpy(pReply->message, "Internal failure in framework: calling scene_removeAllEntities() on non-existent scene");
		return false;
	}

	EntityList* pEntityList = pCoreData->m_pScene->getEntityList();
	pEntityList->removeAllEntities();

	pReply->success = true;
	strcpy(pReply->message, "All entities removed successfully");
	return pReply->success = true;
}