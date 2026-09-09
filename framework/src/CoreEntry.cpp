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
extern "C" __declspec(dllexport) bool setupImage(InstanceData instanceData, int width, int height)
{
	if (!instanceData.pCoreData) {
		std::cerr << "setupImage: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on setupImage()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	Image& image = pCoreData->image;

	if (image.getMode() != Image::NONE) {
		image.dispose();
	}
	image.setAsWrite(width, height);
	image.setSolidColor(Pixel(0, 0, 0));

	pCoreData->setupCalled = true;
	return true;
}

extern "C" __declspec(dllexport) bool drawCircle(InstanceData instanceData, int centerX, int centerY, int radius)
{
	if (!instanceData.pCoreData) {
		std::cerr << "drawCircle: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on drawCircle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	if (!pCoreData->setupCalled) {
		std::cerr << "drawCircle: an image must be created via setupImage() before drawing is allowed" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Must set up an image before trying to draw a circle");
		return false;
	}


	sf::CircleShape* pCircleShape = new sf::CircleShape(radius);
	pCircleShape->setFillColor(sf::Color::Green);
	pCircleShape->setOrigin(sf::Vector2f(radius, radius));
	pCircleShape->setPosition(sf::Vector2f(centerX, centerY));

	pCoreData->m_Shapes.addShape(pCircleShape);


	Image& image = pCoreData->image;

	for (int y = 0; y < image.getHeight(); ++y) {
		for (int x = 0; x < image.getWidth(); ++x) {

			int deltaX = x - centerX;
			int deltaY = y - centerY;

			int distSquared = deltaX * deltaX + deltaY * deltaY;
			int radiusSquared = radius *radius;

			if (distSquared <= radiusSquared) {
				image.setPixel(x, y, Pixel(0, 255, 0));
			}
		}
	}

	return true;
}

extern "C" __declspec(dllexport) bool drawRectangle(InstanceData instanceData, int centerX, int centerY, int halfExtentX, int halfExtentY)
{
	if (!instanceData.pCoreData) {
		std::cerr << "drawRectangle: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on drawRectangle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	if (!pCoreData->setupCalled) {
		std::cerr << "drawRectangle: an image must be created via setupImage() before drawing is allowed" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Must set up an image before trying to draw a rectangle");
		return false;
	}

	sf::RectangleShape* pRectShape = new sf::RectangleShape(sf::Vector2f(halfExtentX * 2, halfExtentY * 2));
	pRectShape->setFillColor(sf::Color::Magenta);
	pRectShape->setOrigin(sf::Vector2f(halfExtentX, halfExtentY));
	pRectShape->setPosition(sf::Vector2f(centerX, centerY));

	pCoreData->m_Shapes.addShape(pRectShape);

	Image& image = pCoreData->image;

	for (int y = 0; y < image.getHeight(); ++y) {
		for (int x = 0; x < image.getWidth(); ++x) {

			int deltaX = abs(x - centerX);
			int deltaY = abs(y - centerY);


			if (deltaX <= halfExtentX && deltaY <= halfExtentY) {
				image.setPixel(x, y, Pixel(250, 0, 250));
			}
		}
	}

	return true;
}

extern "C" __declspec(dllexport) bool exportImage(InstanceData instanceData, const char* pImageName)
{
	if (!instanceData.pCoreData) {
		std::cerr << "exportImage: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on exportImage()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	if (!pCoreData->setupCalled) {
		std::cerr << "exportImage: an image must be created via setupImage() before drawing is allowed" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Must set up an image before trying to export it");
		return false;
	}

	Image& image = pCoreData->image;

	std::string fullName = pImageName + std::string(".png");
	image.exportPNG(pImageName + std::string(".png"));
	system(fullName.c_str());

	return true;
}

extern "C" __declspec(dllexport) bool initRenderWindow(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "initRenderWindow: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on initRenderWindow()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	pCoreData->pWindow = new sf::RenderWindow(sf::VideoMode({ 1920, 1080 }), "SFML works!");

	sf::RenderWindow& window = *pCoreData->pWindow;
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(120);


	return true;
}
extern "C" __declspec(dllexport) bool updateRenderWindow(InstanceData instanceData, float deltaTime)
{
	if (!instanceData.pCoreData) {
		std::cerr << "updateRenderWindow: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on updateRenderWindow()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	sf::RenderWindow& window = *pCoreData->pWindow;

	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			window.close();

	}


	window.clear();
	pCoreData->m_Shapes.drawShapes(window);
	window.display();

	return true;
}
extern "C" __declspec(dllexport) bool disposeRenderWindow(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "disposeRenderWindow: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on disposeRenderWindow()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	delete pCoreData->pWindow;
	pCoreData->pWindow = nullptr;
	
	pCoreData->m_Shapes.destroyAllShapes();

	return true;
}

extern "C" __declspec(dllexport) bool temp_IsRenderWindowOpen(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "temp_IsRenderWindowOpen: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on temp_IsRenderWindowOpen()");
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