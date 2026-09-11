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

extern "C" __declspec(dllexport) bool drawCircle(InstanceData instanceData, int centerX, int centerY, int radius)
{
	if (!instanceData.pCoreData) {
		std::cerr << "drawCircle: instanceData.pCoreData was null" << std::endl;
		strcpy(instanceData.pPublicStatusMessage, "Internal failure in framework on drawCircle()");
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);


	sf::CircleShape* pCircleShape = new sf::CircleShape(radius);
	pCircleShape->setFillColor(sf::Color::Green);
	pCircleShape->setOrigin(sf::Vector2f(radius, radius));
	pCircleShape->setPosition(sf::Vector2f(centerX, centerY));

	pCoreData->m_Shapes.addShape(pCircleShape);

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

	sf::RectangleShape* pRectShape = new sf::RectangleShape(sf::Vector2f(halfExtentX * 2, halfExtentY * 2));
	pRectShape->setFillColor(sf::Color::Magenta);
	pRectShape->setOrigin(sf::Vector2f(halfExtentX, halfExtentY));
	pRectShape->setPosition(sf::Vector2f(centerX, centerY));

	pCoreData->m_Shapes.addShape(pRectShape);

	return true;
}

extern "C" __declspec(dllexport) bool initRenderWindow(InstanceData instanceData)
{
	if (!instanceData.pCoreData) {
		std::cerr << "initRenderWindow: instanceData.pCoreData was null" << std::endl;
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
		return false;
	}

	CoreData* pCoreData = static_cast<CoreData*>(instanceData.pCoreData);

	pCoreData->m_Shapes.destroyAllShapes();
	delete pCoreData->pWindow;
	pCoreData->pWindow = nullptr;

	return true;
}

extern "C" __declspec(dllexport) bool temp_IsRenderWindowOpen(InstanceData instanceData)
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