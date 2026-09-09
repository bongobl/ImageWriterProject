#pragma once
#include <ImageWriter/Image.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <shared_mutex>
#include <mutex>

// C++ constructs allowed here


// TODO: move to its own file
class ShapeList {

	// TODO: when implementing add/remove, need to store in pointer map
	std::vector<sf::Drawable*> m_SFMLDrawables;
	mutable std::shared_mutex mutex;

public:
	void addShape(sf::Drawable* pDrawable) {

		std::unique_lock<std::shared_mutex> lock(mutex);

		m_SFMLDrawables.push_back(pDrawable);
	}

	void drawShapes(sf::RenderWindow& window) {

		std::shared_lock<std::shared_mutex> lock(mutex);

		for (int i = 0; i < m_SFMLDrawables.size(); ++i) {
			window.draw(*m_SFMLDrawables.at(i));
		}
	}

	void destroyAllShapes() {
		for (int i = 0; i < m_SFMLDrawables.size(); ++i) {
			delete m_SFMLDrawables.at(i);
			m_SFMLDrawables.at(i) = nullptr;
		}
		m_SFMLDrawables.clear();
	}
};

struct CoreData {
	Image image;
	bool setupCalled = false; // needs to be called at least once before we can draw to image

	sf::RenderWindow* pWindow = nullptr;

	ShapeList m_Shapes;
};