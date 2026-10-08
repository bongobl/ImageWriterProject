#include <ImageWriter/Scene.h>
#include <ImageWriter/CoreCommon.h>
#include <windows.h>
#include <iostream>

Camera::Camera() : 
	m_Transform({ .position = { .x = 0, .y = 0 }, .orientation = 0, .scale = Camera::StartingScale }),
	m_widthFromHeight(0)
{

}

void Camera::setPosition(Vec2 position) {

	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Transform.position = position;
}

void Camera::setOrientation(float orientation) {
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Transform.orientation = orientation;
}

void Camera::setScale(float scale) {
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Transform.scale = scale;
}

void Camera::move(Vec2 delta) {

	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Transform.position.x += delta.x;
	m_Transform.position.y += delta.y;
}

void Camera::rotate(float deltaDegrees) {
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Transform.orientation += deltaDegrees;
}

void Camera::incrementScale(float deltaScale) {
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Transform.scale += deltaScale;
}

void Camera::setTransform(const Transform& transform) {

	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Transform = transform;

}

Transform Camera::getTransform() const {
	std::shared_lock<std::shared_mutex> lock(mutex);
	return m_Transform;
}

void Camera::validate(std::stringstream* pStream)
{
	// TODO: need to take in streamstream and append to it rather than writing to raw C string, 
	// this will allow additional validate functions to attach their message to the MCP string
	std::unique_lock<std::shared_mutex> lock(mutex);
	if (m_Transform.scale > Camera::MaxScale) {

		if (pStream) {
			*pStream << "Warning: camera scale can not be greater than " << Camera::MaxScale << ", setting camera scale to " << Camera::MaxScale;
		}
		
		m_Transform.scale = Camera::MaxScale;
	}
	else if (m_Transform.scale < Camera::MinScale) {

		if (pStream) {
			*pStream << "Warning: camera scale can not be less than " << Camera::MinScale << ", setting camera scale to " << Camera::MinScale;
		}
		
		m_Transform.scale = Camera::MinScale;
	}
}
Scene::Scene(int64_t windowHandle)
{
	sf::ContextSettings settings;
	settings.antiAliasingLevel = 8;

	if (windowHandle) {
		m_isWindowSelfManaged = false;
		m_pWindow = new sf::RenderWindow((HWND)windowHandle, settings);
	}
	else {
		m_isWindowSelfManaged = true;
		m_pWindow = new sf::RenderWindow(sf::VideoMode({ 1920, 1480 }), "Image Writer App", sf::State::Windowed, settings);

		m_pWindow->setKeyRepeatEnabled(false);
	}

	sf::RenderWindow& window = *m_pWindow;

	// TODO: We also have a frame rate limiter in the app layer which gives us the delta time.
	// We might be able to get away with keeping frame rate limiting + delta time calculations here in the core layer,
	// and have the app call update inside a non-stalled loop. But we'll need to be mindful that
	// this takes away game-clock control from the app.
	window.setFramerateLimit(120);

	// read window initial size
	sf::Vector2u size = window.getSize();
	initialWindowWidth = size.x;
	initialWindowHeight = size.y;

	m_Camera.m_widthFromHeight = (float)size.x / size.y;

	m_HomogFromCamera = m_HomogFromCamera.scale(sf::Vector2f((float)initialWindowHeight / initialWindowWidth, 1));

	// Set screen from camera transform, it will never change
	m_ScreenFromCamera = sf::Transform()
		.translate(sf::Vector2f(initialWindowWidth / 2, initialWindowHeight / 2))
		.scale(sf::Vector2f(initialWindowWidth / 2, -initialWindowHeight / 2))
		* m_HomogFromCamera;

	mousePosition = sf::Mouse::getPosition(window);

	if (!m_VertexShader.loadFromFile("gridShader.vert", sf::Shader::Type::Vertex)) {
		std::cerr << "Vertex shader not found" << std::endl;
		exit(1);
	}

	m_GridVertexBuffer.setPrimitiveType(sf::PrimitiveType::Lines);
	if (!m_GridVertexBuffer.create(1600)) {
		std::cerr << "Failed to create grid vertex buffer" << std::endl;
		exit(1);
	}

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

	
	// Scene controls for self managed windows
	if (m_isWindowSelfManaged) {

		while (const std::optional event = m_pWindow->pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				m_pWindow->close();

			if (m_pWindow->hasFocus()) {
				if (const sf::Event::MouseWheelScrolled* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {


					float scrollDelta = mouseWheelScrolled->delta;

					m_Camera.incrementScale(-scrollDelta);
					m_Camera.validate(nullptr);
				}

				if (const sf::Event::MouseButtonPressed* MouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {

					if (MouseButtonPressed->button == sf::Mouse::Button::Middle) {
						m_Camera.setPosition(Vec2(0, 0));
						m_Camera.setOrientation(0);
						m_Camera.setScale(Camera::StartingScale);
					}
				}
			}
		}

		if (m_pWindow->hasFocus()) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

				const Transform& cameraTransform = m_Camera.getTransform();

				sf::Vector2f cameraDeltaWorldSpaceYFlipped = deltaMouseScreenSpace.rotatedBy(-sf::degrees(cameraTransform.orientation)) / pixelsPerWorldUnit;

				m_Camera.move(Vec2(-cameraDeltaWorldSpaceYFlipped.x, cameraDeltaWorldSpaceYFlipped.y));
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
				m_Camera.rotate(deltaMouseScreenSpace.x / 14);
			}
		}
		
	}
	// Test just to make sure window updates every frame
	//m_Entities.dummyUpdateShapes(deltaTime);
}
void Scene::render()
{
	const Transform& cameraTransform = m_Camera.getTransform();

	// Update camera inverse transform for drawing
	pixelsPerWorldUnit = (initialWindowHeight / 2.0f) / cameraTransform.scale;
	m_CameraFromWorld = sf::fromCore(cameraTransform).getInverse();


	sf::Transform screenFromWorld = m_ScreenFromCamera * m_CameraFromWorld;
	

	// Draw
	m_pWindow->clear();

	// Grid
	float minX = 0, maxX = 0, minY = 0, maxY = 0;
	Vec2 cameraCorners[4] = {
		{ .x = m_Camera.m_widthFromHeight, .y = 1},
		{ .x = m_Camera.m_widthFromHeight, .y = -1 },
		{ .x = -m_Camera.m_widthFromHeight, .y = 1 },
		{ .x = -m_Camera.m_widthFromHeight, .y = -1 },
	};

	for (int i = 0; i < 4; ++i) {

		Vec2 worldCorner = cameraTransform * cameraCorners[i];

		if (i == 0) {
			minX = maxX = worldCorner.x;
			minY = maxY = worldCorner.y;
		}
		else {
			maxX = worldCorner.x > maxX ? worldCorner.x : maxX;
			minX = worldCorner.x < minX ? worldCorner.x : minX;
			maxY = worldCorner.y > maxY ? worldCorner.y : maxY;
			minY = worldCorner.y < minY ? worldCorner.y : minY;
		}
	}

	int numXLines = (int)std::max(0, (int)floor(maxX) - (int)ceil(minX) + 1);
	int numYLines = (int)std::max(0, (int)floor(maxY) - (int)ceil(minY) + 1);

	int numTotalVerts = (numXLines + numYLines) * 2;
	// std::cout << "numXLines = " << numXLines << ", numYLines = " << numYLines << ", numTotalVerts = " << numTotalVerts << std::endl;
	
	m_VertexShader.setUniform("homogFromCamera", sf::Glsl::Mat4(m_HomogFromCamera));
	m_VertexShader.setUniform("cameraFromWorld", sf::Glsl::Mat4(m_CameraFromWorld));
	m_VertexShader.setUniform("cameraScale", cameraTransform.scale);
	m_VertexShader.setUniform("minX", minX);
	m_VertexShader.setUniform("maxX", maxX);
	m_VertexShader.setUniform("minY", minY);
	m_VertexShader.setUniform("maxY", maxY);
	m_VertexShader.setUniform("numXLines", numXLines);
	m_VertexShader.setUniform("numTotalVerts", numTotalVerts);

	sf::RenderStates states;
	states.shader = &m_VertexShader;
	m_pWindow->draw(m_GridVertexBuffer, 0, numTotalVerts, states);

	// draw rest of scene
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