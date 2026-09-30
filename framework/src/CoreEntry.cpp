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

extern "C" __declspec(dllexport) bool scene_initialize(InstanceData instanceData, int64_t windowHandle)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_initialize(): instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::unique_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);
	pCoreData->m_pScene = new Scene(windowHandle);

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

	return true;
}

extern "C" __declspec(dllexport) bool scene_getCameraTransform(InstanceData instanceData, Transform* pCameraTransform)
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

	return true;
}

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
	sf::Vector2f deltaMouseScreenSpace(delta.positionX, delta.positionY);

	// Move
	sf::Vector2f cameraDeltaWorldSpaceYFlipped = deltaMouseScreenSpace.rotatedBy(-sf::degrees(camera.getOrientation())) / pCoreData->m_pScene->pixelsPerWorldUnit;
	camera.move(Vec2(-cameraDeltaWorldSpaceYFlipped.x, cameraDeltaWorldSpaceYFlipped.y));

	// Rotate
	camera.rotate(delta.angle);

	// Scale
	camera.incrementScaleY(delta.scaleY);

	return true;
}
extern "C" __declspec(dllexport) bool scene_addEllipse(InstanceData instanceData, Transform transform, Color color)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_addEllipse(): instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on scene_addEllipse()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_addEllipse(): pCoreData->m_pScene was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework: calling scene_addEllipse() on non-existent scene");
		return false;
	}

	pCoreData->m_pScene->m_Entities.addEllipse(transform, color);
	return true;
}

extern "C" __declspec(dllexport) bool scene_addRectangle(InstanceData instanceData, Transform transform, Color color)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_addRectangle(): instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on scene_addRectangle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_addRectangle(): pCoreData->m_pScene was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework: calling scene_addRectangle() on non-existent scene");
		return false;
	}

	pCoreData->m_pScene->m_Entities.addRectangle(transform, color);
	return true;
}

extern "C" __declspec(dllexport) bool scene_addTriangle(InstanceData instanceData, Transform transform, Color color, Vec2 point1, Vec2 point2, Vec2 point3)
{
	if (!instanceData.pCoreData) {
		std::cerr << "Core entry scene_addTriangle(): instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on scene_addTriangle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	std::shared_lock<std::shared_mutex> lock(pCoreData->m_SceneMutex);

	if (!pCoreData->m_pScene) {
		std::cerr << "Core entry scene_addTriangle(): pCoreData->m_pScene was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework: calling scene_addTriangle() on non-existent scene");
		return false;
	}

	pCoreData->m_pScene->m_Entities.addTriangle(transform, color, point1, point2, point3);
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