#pragma once
#include <ImageWriter/API.h>
#include <ImageWriter/Entity.h>
#include <SFML/Graphics.hpp>
#include <shared_mutex>
#include <mutex>

class Camera {
public:
	struct ComputedState {
		// computed states
		float pixelsPerWorldUnit = 1;
		Transform cameraFromWorld;
		sf::Glsl::Mat4 glsl_homogFromWorld = sf::Glsl::Mat4(sf::Transform::Identity);
	};

private:
	Transform m_WorldFromCamera;
	mutable std::shared_mutex m_TransformMutex;

	ComputedState m_ComputedState;
	mutable std::shared_mutex computedStatesMutex;
	
public:

	// constant over lifetime (no mutex needed)
	float m_widthFromHeight;
	sf::Transform m_sfHomogFromCamera = sf::Transform::Identity;

	
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

	// Computed state
	void updateComputedState(int initialWindowHeight);
	ComputedState getComputedState() const;
};

class GridView
{
	// Grid
	sf::Shader m_VertexShader;
	sf::VertexBuffer m_VertexBuffer;

public:
	GridView();

	void draw(sf::RenderWindow& window, const Camera& camera);
};

class Scene {

	RenderConfig m_RenderConfig;
	mutable std::shared_mutex m_RenderConfigMutex;

	bool m_isWindowSelfManaged = true;

	sf::RenderWindow* m_pWindow = nullptr;
	sf::Vector2f windowInitialExtents = sf::Vector2f(0, 0);

	sf::Vector2i mousePosition;

	GridView m_GridView;

	EntityList m_Entities;

	Camera m_Camera;

public:

	Scene(int64_t windowHandle, RenderConfig renderConfig);
	~Scene();
	bool isSelfManagedRenderWindowOpen() const;

	// Scene lifecycle
	void updateState(float deltaTime);
	void render();
	void updateRenderConfig(RenderConfig renderConfig);
	void updateScreenSpaceCameraControlRig(ScreenSpaceControls controls);

	// API

	void addEntity(AddEntityParams params);
	bool removeEntity(uint64_t id);
	void removeAllEntities();
	
	void setCameraTransform(PlainReply* pReply, Transform transform);
	void getCameraTransform(TransformReply* pReply);
	
};