#include <iostream>
#include <ImageWriter/CoreEntry.h>
#include <ImageWriter/Entity.h>
#include <algorithm>

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
	const Camera& camera = pCoreData->m_Camera;
	const float widthFromHeight = pCoreData->initialWindowWidth / pCoreData->initialWindowHeight;

	*pCameraTransform = camera.getTransform();

	return true;
}
extern "C" __declspec(dllexport) bool addEllipse(InstanceData instanceData, Transform transform, Color color)
{
	if (!instanceData.pCoreData) {
		std::cerr << "drawCircle: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on drawCircle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);
	pCoreData->m_Entities.addEllipse(transform, color);
	return true;
}

extern "C" __declspec(dllexport) bool addRectangle(InstanceData instanceData, Transform transform, Color color)
{
	if (!instanceData.pCoreData) {
		std::cerr << "addRectangle: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on addRectangle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);
	pCoreData->m_Entities.addRectangle(transform, color);
	return true;
}

extern "C" __declspec(dllexport) bool addTriangle(InstanceData instanceData, Transform transform, Color color, Vec2 point1, Vec2 point2, Vec2 point3)
{
	if (!instanceData.pCoreData) {
		std::cerr << "addTriangle: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on addTriangle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);
	pCoreData->m_Entities.addTriangle(transform, color, point1, point2, point3);
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

	pCoreData->m_Entities.destroyAllShapes();

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

	pCoreData->m_Camera.m_widthFromHeight = size.x / size.y;

	// Set screen from camera transform, it will never change
	pCoreData->screenFromCamera = sf::Transform()
		.translate(sf::Vector2f(pCoreData->initialWindowWidth / 2, pCoreData->initialWindowHeight / 2))
		.scale(sf::Vector2f(pCoreData->initialWindowWidth / 2, -pCoreData->initialWindowHeight / 2))
		.scale(sf::Vector2f((float)pCoreData->initialWindowHeight / pCoreData->initialWindowWidth, 1))
	;
	
	pCoreData->mousePosition = sf::Mouse::getPosition(window);

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

	// Update mouse tracking
	sf::Vector2i prevFrameMousePosition = pCoreData->mousePosition;
	pCoreData->mousePosition = sf::Mouse::getPosition(window);
	sf::Vector2f deltaMouseScreenSpace = (sf::Vector2f)(pCoreData->mousePosition - prevFrameMousePosition);

	
	Camera& camera = pCoreData->m_Camera;

	// Camera controls (and event loop)
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			window.close();

		if (const sf::Event::MouseWheelScrolled* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {

			float scrollDelta = mouseWheelScrolled->delta;
			camera.incrementScaleY(-scrollDelta);
		}

		if (const sf::Event::MouseButtonPressed* MouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {

			if (MouseButtonPressed->button == sf::Mouse::Button::Middle) {
				camera.setPosition(Vec2(0, 0));
				camera.setOrientation(0);
				camera.setScaleY(7);
			}
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		sf::Vector2f cameraDeltaWorldSpaceYFlipped = deltaMouseScreenSpace.rotatedBy(-sf::degrees(camera.getOrientation())) / pCoreData->pixelsPerWorldUnit;

		camera.move(Vec2(-cameraDeltaWorldSpaceYFlipped.x, cameraDeltaWorldSpaceYFlipped.y));
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
		camera.rotate(deltaMouseScreenSpace.x / 14);
	}

	// Test just to make sure window updates every frame
	//pCoreData->m_Entities.dummyUpdateShapes(deltaTime);

	// TODO: Move to a draw function
	// Update camera inverse transform for drawing
	pCoreData->pixelsPerWorldUnit = (pCoreData->initialWindowHeight / 2.0f) / camera.getScaleY();
	pCoreData->cameraFromWorld = sf::Transform()
		.scale(sf::Vector2f(1 / camera.getScaleY(), 1 / camera.getScaleY())) // inverse camera scale
		.rotate(-sf::degrees(camera.getOrientation())) // inverse camera orientation
		.translate(-sf::fromCore(camera.getPosition())) // inverse camera position
	;
	

	// Draw
	window.clear();
	pCoreData->m_Entities.drawShapes(window, pCoreData);
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

	pCoreData->m_Entities.destroyAllShapes();
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