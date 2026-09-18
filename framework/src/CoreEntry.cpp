#include <iostream>
#include <ImageWriter/CoreEntry.h>

extern "C" __declspec(dllexport) bool initialize(InstanceData* pInstanceData)
{
	if (!pInstanceData) {
		std::cerr << "initialize: pInstanceData was null" << std::endl;
		return false;
	}
	pInstanceData->pCoreData = new CoreData();

	return true;
}

extern "C" __declspec(dllexport) bool getCameraTransform(InstanceData instanceData, Transform* pCameraTransform)
{
	if (!instanceData.pCoreData) {
		std::cerr << "getCameraTransform: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on getCameraTransform()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);
	*pCameraTransform = {
		.positionX = 0,
		.positionY = 0,
		.scaleX = pCoreData->initialWindowWidth / (2 * pCoreData->screenFromWorldScaleFactor),
		.scaleY = pCoreData->initialWindowHeight / (2 * pCoreData->screenFromWorldScaleFactor),
		.angle = 0
	};

	return true;
}
extern "C" __declspec(dllexport) bool drawCircle(InstanceData instanceData, float posX, float posY, float radius)
{
	if (!instanceData.pCoreData) {
		std::cerr << "drawCircle: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on drawCircle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);


	float screenRadius = pCoreData->screenFromWorldScaleFactor * radius;
	sf::CircleShape* pCircleShape = new sf::CircleShape(screenRadius);
	pCircleShape->setFillColor(sf::Color::Green);
	pCircleShape->setOrigin(sf::Vector2f(screenRadius, screenRadius));
	pCircleShape->setPosition(pCoreData->screenFromWorld * sf::Vector2f(posX, posY));

	pCoreData->m_Shapes.addShape(pCircleShape);

	return true;
}

extern "C" __declspec(dllexport) bool drawRectangle(InstanceData instanceData, float posX, float posY, float halfExtentX, float halfExtentY)
{
	if (!instanceData.pCoreData) {
		std::cerr << "drawRectangle: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on drawRectangle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	sf::Vector2f screenExtents = pCoreData->screenFromWorldScaleFactor * sf::Vector2f(halfExtentX * 2, halfExtentY * 2);
	sf::RectangleShape* pRectShape = new sf::RectangleShape(screenExtents);
	pRectShape->setFillColor(sf::Color::Magenta);
	pRectShape->setOrigin(screenExtents / 2.0f);
	pRectShape->setPosition(pCoreData->screenFromWorld * sf::Vector2f(posX, posY));

	pCoreData->m_Shapes.addShape(pRectShape);

	return true;
}

extern "C" __declspec(dllexport) bool clearImage(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "clearImage: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on clearImage()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	pCoreData->m_Shapes.destroyAllShapes();

	return true;
}
extern "C" __declspec(dllexport) bool initRenderWindow(InstanceData instanceData, int64_t windowHandle)
{
	if (!instanceData.pCoreData) {
		std::cerr << "initRenderWindow: instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	if (windowHandle) {
		pCoreData->pWindow = new sf::RenderWindow((HWND)windowHandle);
		
	}
	else {
		pCoreData->pWindow = new sf::RenderWindow(sf::VideoMode({ 1920, 1080 }), "Image Writer App");
	}
	

	sf::RenderWindow& window = *pCoreData->pWindow;
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(120);

	// read window initial size
	sf::Vector2u size = window.getSize();
	pCoreData->initialWindowWidth = size.x;
	pCoreData->initialWindowHeight = size.y;

	pCoreData->screenFromWorldScaleFactor = (pCoreData->initialWindowHeight / 2.0f) * pCoreData->worldZoomOutFactor;
	pCoreData->screenFromWorld = sf::Transform()
		.translate(sf::Vector2f(pCoreData->initialWindowWidth / 2, pCoreData->initialWindowHeight / 2))
		.scale(sf::Vector2f(pCoreData->initialWindowWidth / 2, -pCoreData->initialWindowHeight / 2))
		.scale(sf::Vector2f((float)pCoreData->initialWindowHeight / pCoreData->initialWindowWidth, 1))
		.scale(sf::Vector2f(pCoreData->worldZoomOutFactor, pCoreData->worldZoomOutFactor));

	// The thread that initializes this window may not be the one that renders to it
	// deactivate OpenGL context for this thread
	(void)window.setActive(false);


	return true;
}
extern "C" __declspec(dllexport) bool updateRenderWindow(InstanceData instanceData, float deltaTime)
{
	if (!instanceData.pCoreData) {
		std::cerr << "updateRenderWindow: instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	sf::RenderWindow& window = *pCoreData->pWindow;

	// activate OpenGL context for on thread for drawing
	(void)window.setActive(true);

	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			window.close();

	}
	
	// Test just to make sure window updates every frame
	//pCoreData->m_Shapes.dummyUpdateShapes(deltaTime);

	window.clear();
	pCoreData->m_Shapes.drawShapes(window);
	window.display();

	return true;
}
extern "C" __declspec(dllexport) bool disposeRenderWindow(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "disposeRenderWindow: instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	pCoreData->m_Shapes.destroyAllShapes();
	delete pCoreData->pWindow;
	pCoreData->pWindow = nullptr;

	return true;
}

extern "C" __declspec(dllexport) bool isIsolatedRenderWindowOpen(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "temp_IsRenderWindowOpen: instanceData.pCoreData was null" << std::endl;
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	sf::RenderWindow& window = *pCoreData->pWindow;
	return window.isOpen();

}
extern "C" __declspec(dllexport) bool dispose(InstanceData* pInstanceData)
{
	if (!pInstanceData) {
		std::cerr << "dispose: pInstanceData was null" << std::endl;
		return false;
	}

	if (!pInstanceData->pCoreData) {
		std::cerr << "dispose: pInstanceData->pCoreData was null" << std::endl;
		return false;
	}

	delete static_cast<CoreData*>(pInstanceData->pCoreData);
	pInstanceData->pCoreData = nullptr;
	return true;
}