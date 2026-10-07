#pragma once
#include <ImageWriter/API.h>
#include <ImageWriter/Entity.h>
#include <SFML/Graphics.hpp>
#include <shared_mutex>
#include <mutex>

class Camera {

	Transform m_Transform;
	mutable std::shared_mutex mutex;

public:

	float m_widthFromHeight;
	static constexpr float MaxScale = 100.0f;
	static constexpr float MinScale = 1.0f;
	static constexpr float StartingScale = 13.0f;

	Camera();

	// setters
	void setPosition(Vec2 position);
	void setOrientation(float orientation);
	void setScale(float scale);

	// delta setters
	void move(Vec2 delta);
	void rotate(float deltaDegrees);
	void incrementScale(float deltaScale);

	void setTransform(const Transform& transform);
	Transform getTransform() const;

	void validate(std::stringstream* pStream);
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
	
	// Grid
	static constexpr int GridSpan = 100;
	static constexpr int NumLinesPerAxis = GridSpan * 2 + 1;
	static constexpr int NumTotalVertices = NumLinesPerAxis * 2 * 2;
	sf::Vertex m_GridVerts[NumTotalVertices];

public:
	Scene(int64_t windowHandle);
	~Scene();
	
	void updateState(float deltaTime);
	void render();

	bool isSelfManagedRenderWindowOpen() const;
};