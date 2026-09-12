#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <shared_mutex>
#include <mutex>

// C++ constructs allowed here


// TODO: move to its own file
class ShapeList {

	// TODO: when implementing add/remove, need to store in pointer map
	std::vector<sf::Shape*> m_Shapes;
	mutable std::shared_mutex mutex;

public:
	void addShape(sf::Shape* pDrawable) {

		std::unique_lock<std::shared_mutex> lock(mutex);

		m_Shapes.push_back(pDrawable);
	}

	void dummyUpdateShapes(float deltaTime) {
		std::shared_lock<std::shared_mutex> lock(mutex);

		for (int i = 0; i < m_Shapes.size(); ++i) {
			m_Shapes.at(i)->rotate(sf::radians(deltaTime));
		}
	}
	void drawShapes(sf::RenderWindow& window) {

		std::shared_lock<std::shared_mutex> lock(mutex);

		for (int i = 0; i < m_Shapes.size(); ++i) {
			window.draw(*m_Shapes.at(i));
		}
	}

	void destroyAllShapes() {
		for (int i = 0; i < m_Shapes.size(); ++i) {
			delete m_Shapes.at(i);
			m_Shapes.at(i) = nullptr;
		}
		m_Shapes.clear();
	}
};

struct CoreData {

	sf::RenderWindow* pWindow = nullptr;

	ShapeList m_Shapes;
};