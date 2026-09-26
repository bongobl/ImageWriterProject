#include <ImageWriter/Scene.h>

Camera::Camera() : m_Position(0, 0), m_Orientation(0), m_ScaleY(7) {

}

void Camera::setPosition(Vec2 position) {

	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Position.x = position.x;
	m_Position.y = position.y;
}

void Camera::setOrientation(float angleAsDegrees) {
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Orientation = angleAsDegrees;
}

void Camera::setScaleY(float scale) {
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_ScaleY = scale;
	m_ScaleY = m_ScaleY < 0 ? 0 : m_ScaleY;
	m_ScaleY = m_ScaleY > 100 ? 100 : m_ScaleY;
}

void Camera::move(Vec2 delta) {

	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Position.x += delta.x;
	m_Position.y += delta.y;
}

void Camera::rotate(float deltaDegrees) {
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Orientation += deltaDegrees;
}

void Camera::incrementScaleY(float deltaScale) {
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_ScaleY += deltaScale;
	m_ScaleY = m_ScaleY < 0 ? 0 : m_ScaleY;
	m_ScaleY = m_ScaleY > 100 ? 100 : m_ScaleY;
}

Vec2 Camera::getPosition() const {
	std::shared_lock<std::shared_mutex> lock(mutex);
	return m_Position;
}

float Camera::getOrientation() const {
	std::shared_lock<std::shared_mutex> lock(mutex);
	return m_Orientation;
}

float Camera::getScaleY() const {
	std::shared_lock<std::shared_mutex> lock(mutex);
	return m_ScaleY;
}

Transform Camera::getTransform() const {
	std::shared_lock<std::shared_mutex> lock(mutex);
	return {
		.positionX = m_Position.x,
			.positionY = m_Position.y,
			.scaleX = m_widthFromHeight * m_ScaleY,
			.scaleY = m_ScaleY,
			.angle = m_Orientation
	};
}