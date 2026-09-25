#pragma once
#include <ImageWriter/API.h>
#include <ImageWriter/Entity.h>
#include <SFML/Graphics.hpp>
#include <shared_mutex>
#include <mutex>

// C++ constructs allowed here

class Camera {

	Vec2 m_Position;
	float m_Orientation;
	float m_ScaleY;
	mutable std::shared_mutex mutex;
	
public:

	float m_widthFromHeight;

	Camera() : m_Position(0, 0), m_Orientation(0), m_ScaleY(7) {
		
	}

	void setPosition(Vec2 position) {

		std::unique_lock<std::shared_mutex> lock(mutex);
		m_Position.x = position.x;
		m_Position.y = position.y;
	}

	void setOrientation(float angleAsDegrees) {
		std::unique_lock<std::shared_mutex> lock(mutex);
		m_Orientation = angleAsDegrees;
	}

	void setScaleY(float scale) {
		std::unique_lock<std::shared_mutex> lock(mutex);
		m_ScaleY = scale;
		m_ScaleY = m_ScaleY < 0 ? 0 : m_ScaleY;
		m_ScaleY = m_ScaleY > 100 ? 100 : m_ScaleY;
	}

	void move(Vec2 delta) {

		std::unique_lock<std::shared_mutex> lock(mutex);
		m_Position.x += delta.x;
		m_Position.y += delta.y;
	}

	void rotate(float deltaDegrees) {
		std::unique_lock<std::shared_mutex> lock(mutex);
		m_Orientation += deltaDegrees;
	}

	void incrementScaleY(float deltaScale) {
		std::unique_lock<std::shared_mutex> lock(mutex);
		m_ScaleY += deltaScale;
		m_ScaleY = m_ScaleY < 0 ? 0 : m_ScaleY;
		m_ScaleY = m_ScaleY > 100 ? 100 : m_ScaleY;
	}

	Vec2 getPosition() {
		std::shared_lock<std::shared_mutex> lock(mutex);
		return m_Position;
	}

	float getOrientation() {
		std::shared_lock<std::shared_mutex> lock(mutex);
		return m_Orientation;
	}

	float getScaleY() {
		std::shared_lock<std::shared_mutex> lock(mutex);
		return m_ScaleY;
	}

	Transform getTransform() const {
		std::shared_lock<std::shared_mutex> lock(mutex);
		return {
			.positionX = m_Position.x,
				.positionY = m_Position.y,
				.scaleX = m_widthFromHeight * m_ScaleY,
				.scaleY = m_ScaleY,
				.angle = m_Orientation
		};
	}
};

// TODO: move to a "Scene" class
struct CoreData {

	sf::RenderWindow* pWindow = nullptr;
	float initialWindowWidth = 0;
	float initialWindowHeight = 0;

	// screen space from world space conversion
	float pixelsPerWorldUnit = 1;
	sf::Transform screenFromCamera = sf::Transform::Identity;
	sf::Transform cameraFromWorld = sf::Transform::Identity;

	Camera m_Camera;
	EntityList m_Entities;
	sf::Vector2i mousePosition;

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