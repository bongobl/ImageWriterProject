#pragma once
#include <ImageWriter/API.h>
#include <ImageWriter/Entity.h>
#include <SFML/Graphics.hpp>
#include <shared_mutex>
#include <mutex>

class Camera {

	Vec2 m_Position;
	float m_Orientation;
	float m_ScaleY;
	mutable std::shared_mutex mutex;

public:

	float m_widthFromHeight;

	Camera();

	// setters
	void setPosition(Vec2 position);
	void setOrientation(float angleAsDegrees);
	void setScaleY(float scale);

	// delta setters
	void move(Vec2 delta);
	void rotate(float deltaDegrees);
	void incrementScaleY(float deltaScale);

	// getters
	Vec2 getPosition() const;
	float getOrientation() const;
	float getScaleY() const;

	void setTransform(const Transform& transform);
	Transform getTransform() const;
};

class Scene {
public:
	bool m_isWindowSelfManaged = true;

	sf::RenderWindow* m_pWindow = nullptr;
	float initialWindowWidth = 0;
	float initialWindowHeight = 0;

	// screen space from world space conversion
	float pixelsPerWorldUnit = 1;
	sf::Transform screenFromCamera = sf::Transform::Identity;
	sf::Transform cameraFromWorld = sf::Transform::Identity;

	Camera m_Camera;
	EntityList m_Entities;
	sf::Vector2i mousePosition;

public:
	Scene(int64_t windowHandle);
	~Scene();
	
	void updateState(float deltaTime);
	void render();

	bool isSelfManagedRenderWindowOpen() const;
};