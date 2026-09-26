#pragma once
#include <ImageWriter/API.h>
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

	Transform getTransform() const;
};

//class Scene {
//
//};