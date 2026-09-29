#pragma once
#include <ImageWriter/API.h>
#include <SFML/Graphics.hpp>
#include <shared_mutex>
#include <mutex>

// C++ constructs allowed here

class Scene;
struct CoreData 
{
	Scene* m_pScene = nullptr;
	
	// keeps reads/writes to the scene pointer atomic
	mutable std::shared_mutex m_SceneMutex;
};

using FrameworkColor = Color;
namespace sf {
	static inline sf::Color fromCore(FrameworkColor color) {
		return sf::Color(
			std::clamp(color.red, 0.0f, 1.0f) * 255,
			std::clamp(color.green, 0.0f, 1.0f) * 255,
			std::clamp(color.blue, 0.0f, 1.0f) * 255,
			std::clamp(color.alpha, 0.0f, 1.0f) * 255);
	}

	static inline sf::Vector2f fromCore(Vec2 vector) {
		return sf::Vector2f(vector.x, vector.y);
	}
}