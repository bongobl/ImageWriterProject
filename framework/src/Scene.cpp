#include <ImageWriter/Scene.h>
#include <ImageWriter/CoreCommon.h>
#include <windows.h>
#include <iostream>

Camera::Camera() : 
	m_Transform({ .position = { .x = 0, .y = 0 }, .orientation = 0, .scale = Camera::StartingScale }),
	m_widthFromHeight(0)
{

}

void Camera::setPosition(Vec2 position) 
{
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_Transform.position = position;
}

void Camera::updateComputedState(int initialWindowHeight)
{
	std::unique_lock<std::shared_mutex> lock(computedStatesMutex);

	const Transform& cameraTransform = getTransform();

	m_ComputedState.pixelsPerWorldUnit = (initialWindowHeight / 2.0f) / cameraTransform.scale;
	m_ComputedState.cameraFromWorld = sf::fromCore(cameraTransform).getInverse();
}

Camera::ComputedState Camera::getComputedState() const
{
	std::shared_lock<std::shared_mutex> lock(computedStatesMutex);
	return m_ComputedState;
}
void Camera::setOrientation(float orientation) {
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_Transform.orientation = orientation;
}

void Camera::setScale(float scale) {
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_Transform.scale = scale;
}

void Camera::move(Vec2 delta) {

	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_Transform.position.x += delta.x;
	m_Transform.position.y += delta.y;
}

void Camera::rotate(float deltaDegrees) {
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_Transform.orientation += deltaDegrees;
}

void Camera::incrementScale(float deltaScale) {
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_Transform.scale += deltaScale;
}

void Camera::setTransform(const Transform& transform) {

	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_Transform = transform;

}

Transform Camera::getTransform() const {
	std::shared_lock<std::shared_mutex> lock(m_TransformMutex);
	return m_Transform;
}

void Camera::validate(std::stringstream* pStream)
{
	// TODO: need to take in streamstream and append to it rather than writing to raw C string, 
	// this will allow additional validate functions to attach their message to the MCP string
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
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

///////////////////////////////////////////////////////////////////

GridView::GridView()
{
	if (!m_VertexShader.loadFromFile("gridShader.vert", sf::Shader::Type::Vertex)) {
		std::cerr << "Vertex shader not found" << std::endl;
		exit(1);
	}

	m_VertexBuffer.setPrimitiveType(sf::PrimitiveType::Lines);
	if (!m_VertexBuffer.create(1600)) {
		std::cerr << "Failed to create grid vertex buffer" << std::endl;
		exit(1);
	}
}

void GridView::draw(sf::RenderWindow& window, const Camera& camera)
{

	const Transform& cameraTransform = camera.getTransform();

	float minX = 0, maxX = 0, minY = 0, maxY = 0;
	Vec2 cameraCorners[4] = {
		{ .x = camera.m_widthFromHeight, .y = 1 },
		{ .x = camera.m_widthFromHeight, .y = -1 },
		{ .x = -camera.m_widthFromHeight, .y = 1 },
		{ .x = -camera.m_widthFromHeight, .y = -1 },
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

	m_VertexShader.setUniform("homogFromCamera", sf::Glsl::Mat4(camera.m_HomogFromCamera));
	m_VertexShader.setUniform("cameraFromWorld", sf::Glsl::Mat4(camera.getComputedState().cameraFromWorld));
	m_VertexShader.setUniform("cameraScale", cameraTransform.scale);
	m_VertexShader.setUniform("minX", minX);
	m_VertexShader.setUniform("maxX", maxX);
	m_VertexShader.setUniform("minY", minY);
	m_VertexShader.setUniform("maxY", maxY);
	m_VertexShader.setUniform("numXLines", numXLines);
	m_VertexShader.setUniform("numTotalVerts", numTotalVerts);

	sf::RenderStates states;
	states.shader = &m_VertexShader;
	window.draw(m_VertexBuffer, 0, numTotalVerts, states);
}

///////////////////////////////////////////////////////////////////
Scene::Scene(int64_t windowHandle, RenderConfig renderConfig) :
	m_RenderConfig(renderConfig)
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
	windowInitialExtents = (sf::Vector2f)window.getSize();

	// camera constants
	m_Camera.m_widthFromHeight = (float)windowInitialExtents.x / windowInitialExtents.y;
	m_Camera.m_HomogFromCamera = m_Camera.m_HomogFromCamera.scale(sf::Vector2f((float)windowInitialExtents.y / windowInitialExtents.x, 1));

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

				sf::Vector2f cameraDeltaWorldSpaceYFlipped = deltaMouseScreenSpace.rotatedBy(-sf::degrees(cameraTransform.orientation)) / m_Camera.getComputedState().pixelsPerWorldUnit;

				m_Camera.move(Vec2(-cameraDeltaWorldSpaceYFlipped.x, cameraDeltaWorldSpaceYFlipped.y));
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
				m_Camera.rotate(deltaMouseScreenSpace.x / 14);
			}
		}
	}

	m_Camera.updateComputedState(windowInitialExtents.y);
	// Test just to make sure window updates every frame
	//m_Entities.dummyUpdateShapes(deltaTime);
}
void Scene::render()
{
	// Draw
	m_pWindow->clear();

	// Grid background mode
	if (m_RenderConfig.gridViewMode == GRID_VIEW_MODE_BACKGROUND)
	{
		m_GridView.draw(*m_pWindow, m_Camera);
	}

	// draw rest of scene
	m_Entities.drawShapes(*m_pWindow, m_Camera);

	// Grid foreground mode
	if (m_RenderConfig.gridViewMode == GRID_VIEW_MODE_FOREGROUND)
	{
		m_GridView.draw(*m_pWindow, m_Camera);
	}
	m_pWindow->display();
}

void Scene::updateRenderConfig(RenderConfig renderConfig)
{
	m_RenderConfig = renderConfig;
}

bool Scene::isSelfManagedRenderWindowOpen() const
{
	if (!m_isWindowSelfManaged) {
		std::cerr << "\nError: Calling Scene::isSelfManagedRenderWindowOpen() on a non-self managed window" << std::endl;
		exit(1);
	}
	return m_pWindow->isOpen();
}