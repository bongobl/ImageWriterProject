#pragma once
#include <ImageWriter/API.h>
#include <SFML/Graphics.hpp>
#include <shared_mutex>
#include <mutex>
#include <cmath>

// C++ constructs allowed here

class Scene;
struct CoreData 
{
	Scene* m_pScene = nullptr;
	
	// keeps reads/writes to the scene pointer atomic
	mutable std::shared_mutex m_SceneMutex;
};

using FrameworkColor = Color;
using FrameworkTransform = Transform;
namespace sf {
	inline sf::Color fromCore(FrameworkColor color) {
		return sf::Color(
			std::clamp(color.red, 0.0f, 1.0f) * 255,
			std::clamp(color.green, 0.0f, 1.0f) * 255,
			std::clamp(color.blue, 0.0f, 1.0f) * 255,
			std::clamp(color.alpha, 0.0f, 1.0f) * 255);
	}

	inline sf::Vector2f fromCore(Vec2 vector) {
		return sf::Vector2f(vector.x, vector.y);
	}

	inline sf::Transform fromCore(FrameworkTransform transform) {
		return sf::Transform()
			.translate(sf::fromCore(transform.position))
			.rotate(sf::degrees(transform.orientation))
			.scale(sf::Vector2f(transform.scale, transform.scale));
	}
}

inline const Vec2 operator*(const Transform& transform, const Vec2& orig)
{
	constexpr float deg_to_rad = 0.01745329251f;
	float cosine = cos(transform.orientation * deg_to_rad);
	float sine = sin(transform.orientation * deg_to_rad);

	return {
		.x = transform.position.x + (cosine * orig.x - sine * orig.y) * transform.scale,
		.y = transform.position.y + (sine * orig.x + cosine * orig.y) * transform.scale
	};
}
