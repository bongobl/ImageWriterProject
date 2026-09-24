#pragma once
#include <ImageWriter/API.h>
#include <ImageWriter/Entity.h>
#include <SFML/Graphics.hpp>

// C++ constructs allowed here

struct CoreData {

	sf::RenderWindow* pWindow = nullptr;
	float initialWindowWidth = 0;
	float initialWindowHeight = 0;

	// screen space from world space conversion
	const float worldZoomOutFactor = 0.15;
	float screenFromWorldScaleFactor = 1;
	sf::Transform screenFromWorld = sf::Transform::Identity;

	EntityList m_Entities;
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