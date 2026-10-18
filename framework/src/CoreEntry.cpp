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

extern "C" __declspec(dllexport) bool scene_setCameraTransform(InstanceData instanceData, Transform transform)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_setCameraTransform(): instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on scene_setCameraTransform()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_setCameraTransform(): pCoreData->m_pScene was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework: calling scene_setCameraTransform() on non-existent scene");
		return false;
	}

	Camera& camera = pCoreData->m_pScene->m_Camera;
	camera.setTransform(transform);

	// Validate camera transform
	std::stringstream warningsStream;
	camera.validate(&warningsStream);

	// If there are any warnings, overwrite default success message
	if (!warningsStream.str().empty()) {
		strcpy(instanceData.pPublicStatusMessage, warningsStream.str().c_str());
	}
	
	return true;
}

extern "C" __declspec(dllexport) bool scene_getCameraTransform(InstanceData instanceData, Transform* pCameraTransform, float* pWidthFromheight)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_getCameraTransform(): instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on scene_getCameraTransform()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_getCameraTransform(): pCoreData->m_pScene was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework: calling scene_getCameraTransform() on non-existent scene");
		return false;
	}

	const Camera& camera = pCoreData->m_pScene->m_Camera;
	*pCameraTransform = camera.getTransform();

	// Note: a bit sloppy here accessing it without a mutex but it will never change
	*pWidthFromheight = camera.m_widthFromHeight; 
	return true;
}

// NOTE: for the mouse control rig in python, we are currently using the delta Transform to store screen space
// translations (num pixels rather than world units), Maybe instead of fixing this we can just keep it as is
// and have it as a scene lifecycle function rather than an MCP one
extern "C" __declspec(dllexport) bool scene_TEMP_moveCameraLocalSpace(InstanceData instanceData, Transform delta)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_TEMP_moveCameraLocalSpace(): instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on scene_TEMP_moveCameraLocalSpace()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_TEMP_moveCameraLocalSpace(): pCoreData->m_pScene was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework: calling scene_TEMP_moveCameraLocalSpace() on non-existent scene");
		return false;
	}

	Scene& scene = *pCoreData->m_pScene;
	Camera& camera = scene.m_Camera;
	sf::Vector2f deltaMouseScreenSpace = sf::fromCore(delta.position);

	const Transform& cameraTransform = camera.getTransform();

	// Move
	sf::Vector2f cameraDeltaWorldSpaceYFlipped = deltaMouseScreenSpace.rotatedBy(-sf::degrees(cameraTransform.orientation)) / camera.getComputedState().pixelsPerWorldUnit;
	camera.move(Vec2(-cameraDeltaWorldSpaceYFlipped.x, cameraDeltaWorldSpaceYFlipped.y));

	// Rotate
	camera.rotate(delta.orientation);

	// Scale
	camera.incrementScale(delta.scale);

	// Validate camera transform
	std::stringstream warningsStream;
	camera.validate(&warningsStream);

	// If there are any warnings, overwrite default success message
	if (!warningsStream.str().empty()) {
		strcpy(instanceData.pPublicStatusMessage, warningsStream.str().c_str());
	}

	return true;
}

extern "C" __declspec(dllexport) bool scene_addEntity(InstanceData instanceData, AddEntityParams params)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_addEntity(): instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on scene_addEntity()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_addEntity(): pCoreData->m_pScene was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework: calling scene_addEntity() on non-existent scene");
		return false;
	}

	pCoreData->m_pScene->addEntity(params);

	return true;
}

extern "C" __declspec(dllexport) bool scene_removeEntity(InstanceData instanceData, uint64_t id)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_removeEntity(): instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on scene_removeEntity()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_removeEntity(): pCoreData->m_pScene was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework: calling scene_removeEntity() on non-existent scene");
		return false;
	}

	if (!pCoreData->m_pScene->removeEntity(id)) {
		// NOTE: this assumes it failed due to id key not found, we shouldn't be making
		// assumptions on why it failed in the CoreEntry layer

		std::string errorMessage = "No entity with a given ID of " + std::to_string(id) + std::string(" found");
		strcpy(instanceData.pPublicStatusMessage, errorMessage.c_str());
		return false;
	}

	return true;
}

extern "C" __declspec(dllexport) bool scene_removeAllEntities(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_removeAllEntities(): instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on scene_removeAllEntities()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_removeAllEntities(): pCoreData->m_pScene was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework: calling scene_removeAllEntities() on non-existent scene");
		return false;
	}

	pCoreData->m_pScene->m_Entities.destroyAllShapes();

	return true;
}