#pragma once
#include <ImageWriter/Image.h>
#include <SFML/Graphics.hpp>

// C++ constructs allowed here

struct CoreData {
	Image image;
	bool setupCalled = false; // needs to be called at least once before we can draw to image

	sf::RenderWindow* pWindow = nullptr;

	sf::RectangleShape* pRectangleShape = nullptr;
};