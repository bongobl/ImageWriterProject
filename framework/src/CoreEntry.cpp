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

	pCoreData->m_pScene->setCameraTransform(pReply, transform);
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

	pCoreData->m_pScene->getCameraTransform(pReply);
	return pReply->success;
}

extern "C" __declspec(dllexport) bool scene_addEntity(InstanceData instanceData, PlainReply* pReply, AddEntityParams params)
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

	pCoreData->m_pScene->addEntity(params);

	pReply->success = true;
	strcpy(pReply->message, "Entity added successfully");
	return pReply->success;
}

extern "C" __declspec(dllexport) bool scene_removeEntity(InstanceData instanceData, PlainReply* pReply, uint64_t id)
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

	if (!pCoreData->m_pScene->removeEntity(id)) {
		// NOTE: this assumes it failed due to id key not found, we shouldn't be making
		// assumptions on why it failed in the CoreEntry layer

		std::string errorMessage = "No entity with a given ID of " + std::to_string(id) + std::string(" found");
		pReply->success = false;
		strcpy(pReply->message, errorMessage.c_str());
		return false;
	}
	pReply->success = true;
	std::string successMessage = "Entity with id " + std::to_string(id) + std::string(" removed successfully");
	strcpy(pReply->message, successMessage.c_str());
	return pReply->success = true;
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

	pCoreData->m_pScene->removeAllEntities();

	pReply->success = true;
	strcpy(pReply->message, "All entities removed successfully");
	return pReply->success = true;
}