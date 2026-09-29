#include <ImageWriter/Scene.h>
#include <ImageWriter/CoreCommon.h>
#include <windows.h>
#include <iostream>

Camera::Camera() : m_Position(0, 0), m_Orientation(0), m_ScaleY(7), m_widthFromHeight(0) {

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

Scene::Scene(int64_t windowHandle)
{
	if (windowHandle) {
		m_isWindowSelfManaged = false;
		m_pWindow = new sf::RenderWindow((HWND)windowHandle);
	}
	else {
		m_isWindowSelfManaged = true;
		m_pWindow = new sf::RenderWindow(sf::VideoMode({ 1920, 1080 }), "Image Writer App");
	}

	sf::RenderWindow& window = *m_pWindow;


	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(120);

	// read window initial size
	sf::Vector2u size = window.getSize();
	initialWindowWidth = size.x;
	initialWindowHeight = size.y;

	m_Camera.m_widthFromHeight = size.x / size.y;

	// Set screen from camera transform, it will never change
	screenFromCamera = sf::Transform()
		.translate(sf::Vector2f(initialWindowWidth / 2, initialWindowHeight / 2))
		.scale(sf::Vector2f(initialWindowWidth / 2, -initialWindowHeight / 2))
		.scale(sf::Vector2f((float)initialWindowHeight / initialWindowWidth, 1))
		;

	mousePosition = sf::Mouse::getPosition(window);

	// The thread that initializes this window may not be the one that renders to it
	// deactivate OpenGL context for this thread
	(void)window.setActive(false);
}
Scene::~Scene()
{
	m_Entities.destroyAllShapes();
	delete m_pWindow;
	m_pWindow = nullptr;
}

void Scene::updateState(float deltaTime)
{
	// activate OpenGL context for on thread for drawing
	(void)m_pWindow->setActive(true);

	// Update mouse tracking
	sf::Vector2i prevFrameMousePosition = mousePosition;
	mousePosition = sf::Mouse::getPosition(*m_pWindow);
	sf::Vector2f deltaMouseScreenSpace = (sf::Vector2f)(mousePosition - prevFrameMousePosition);


	// Camera controls (and event loop)
	while (const std::optional event = m_pWindow->pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			m_pWindow->close();

		if (const sf::Event::MouseWheelScrolled* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {

			float scrollDelta = mouseWheelScrolled->delta;
			m_Camera.incrementScaleY(-scrollDelta);
		}

		if (const sf::Event::MouseButtonPressed* MouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {

			if (MouseButtonPressed->button == sf::Mouse::Button::Middle) {
				m_Camera.setPosition(Vec2(0, 0));
				m_Camera.setOrientation(0);
				m_Camera.setScaleY(7);
			}
		}
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		sf::Vector2f cameraDeltaWorldSpaceYFlipped = deltaMouseScreenSpace.rotatedBy(-sf::degrees(m_Camera.getOrientation())) / pixelsPerWorldUnit;

		m_Camera.move(Vec2(-cameraDeltaWorldSpaceYFlipped.x, cameraDeltaWorldSpaceYFlipped.y));
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
		m_Camera.rotate(deltaMouseScreenSpace.x / 14);
	}

	// Test just to make sure window updates every frame
	//m_Entities.dummyUpdateShapes(deltaTime);

	
}
void Scene::render()
{
	// Update camera inverse transform for drawing
	pixelsPerWorldUnit = (initialWindowHeight / 2.0f) / m_Camera.getScaleY();
	cameraFromWorld = sf::Transform()
		.scale(sf::Vector2f(1 / m_Camera.getScaleY(), 1 / m_Camera.getScaleY())) // inverse camera scale
		.rotate(-sf::degrees(m_Camera.getOrientation())) // inverse camera orientation
		.translate(-sf::fromCore(m_Camera.getPosition())) // inverse camera position
		;

	// Draw
	m_pWindow->clear();
	m_Entities.drawShapes(*m_pWindow, this);
	m_pWindow->display();
}

bool Scene::isSelfManagedRenderWindowOpen() const
{
	if (!m_isWindowSelfManaged) {
		std::cerr << "\nError: Calling Scene::isSelfManagedRenderWindowOpen() on a non-self managed window" << std::endl;
		exit(1);
	}
	return m_pWindow->isOpen();
}