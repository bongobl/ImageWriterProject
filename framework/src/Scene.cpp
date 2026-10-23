#include <ImageWriter/Scene.h>
#include <ImageWriter/CoreCommon.h>
#include <windows.h>
#include <iostream>

Camera::Camera() : 
	m_WorldFromCamera({ .position = { .x = 0, .y = 0 }, .orientation = 0, .scale = Camera::StartingScale }),
	m_widthFromHeight(0)
{
}

void Camera::setPosition(Vec2 position) 
{
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_WorldFromCamera.position = position;
}

void Camera::updateComputedState(int initialWindowHeight)
{
	std::unique_lock<std::shared_mutex> lock(computedStatesMutex);

	const Transform& cameraTransform = getTransform();

	m_ComputedState.pixelsPerWorldUnit = (initialWindowHeight / 2.0f) / cameraTransform.scale;
	m_ComputedState.cameraFromWorld = inverse(cameraTransform);
	m_ComputedState.glsl_homogFromWorld = sf::Glsl::Mat4(m_sfHomogFromCamera * sf::fromCore(m_ComputedState.cameraFromWorld));
}

Camera::ComputedState Camera::getComputedState() const
{
	std::shared_lock<std::shared_mutex> lock(computedStatesMutex);
	return m_ComputedState;
}
void Camera::setOrientation(float orientation) {
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_WorldFromCamera.orientation = orientation;
}

void Camera::setScale(float scale) {
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_WorldFromCamera.scale = scale;
	validateScale();
}

void Camera::move(Vec2 delta) {

	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_WorldFromCamera.position.x += delta.x;
	m_WorldFromCamera.position.y += delta.y;
}

void Camera::rotate(float deltaDegrees) {
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_WorldFromCamera.orientation += deltaDegrees;
}

void Camera::incrementScale(float deltaScale) {
	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_WorldFromCamera.scale += deltaScale;
	validateScale();
}

bool Camera::setTransform(const Transform& transform, std::string* pMessage) {

	std::unique_lock<std::shared_mutex> lock(m_TransformMutex);
	m_WorldFromCamera = transform;

	return validateScale(pMessage);
}

Transform Camera::getTransform() const {
	std::shared_lock<std::shared_mutex> lock(m_TransformMutex);
	return m_WorldFromCamera;
}

bool Camera::validateScale(std::string* pMessage)
{
	if (m_WorldFromCamera.scale > Camera::MaxScale) {

		if (pMessage) {
			*pMessage = "Warning: camera scale can not be greater than " + std::to_string(Camera::MaxScale) + ", setting camera scale to " + std::to_string(Camera::MaxScale);
		}
		
		m_WorldFromCamera.scale = Camera::MaxScale;
	}
	else if (m_WorldFromCamera.scale < Camera::MinScale) {

		if (pMessage) {
			*pMessage = "Warning: camera scale can not be less than " + std::to_string(Camera::MinScale) + ", setting camera scale to " + std::to_string(Camera::MinScale);
		}
		
		m_WorldFromCamera.scale = Camera::MinScale;
	}
	return true;
}

void Camera::API_setTransform(PlainReply* pReply, Transform transform)
{
	// tentatively set success string
	std::string message = "Camera transform set successfully";

	pReply->success = setTransform(transform, &message);
	strcpy(pReply->message, message.c_str());
}

void Camera::API_getTransform(TransformReply* pReply) const
{
	pReply->transform = getTransform();
	pReply->cameraWidthFromHeight = m_widthFromHeight;

	pReply->success = true;
	strcpy(pReply->message, "Camera transform obtained successfully");
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

	m_VertexShader.setUniform("homogFromWorld", camera.getComputedState().glsl_homogFromWorld);
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
	m_Camera.m_sfHomogFromCamera = m_Camera.m_sfHomogFromCamera.scale(sf::Vector2f((float)windowInitialExtents.y / windowInitialExtents.x, 1));

	mousePosition = sf::Mouse::getPosition(window);

	// The thread that initializes this window may not be the one that renders to it
	// deactivate OpenGL context for this thread
	(void)window.setActive(false);
}
Scene::~Scene()
{
	m_Entities.removeAllEntities();
	delete m_pWindow;
	m_pWindow = nullptr;
}

bool Scene::isSelfManagedRenderWindowOpen() const
{
	if (!m_isWindowSelfManaged) {
		std::cerr << "\nError: Calling Scene::isSelfManagedRenderWindowOpen() on a non-self managed window" << std::endl;
		exit(1);
	}
	return m_pWindow->isOpen();
}

void Scene::updateState(float deltaTime)
{
	// activate OpenGL context for on thread for drawing
	(void)m_pWindow->setActive(true);

	// Update mouse tracking
	sf::Vector2i prevFrameMousePosition = mousePosition;
	mousePosition = sf::Mouse::getPosition(*m_pWindow);
	sf::Vector2i deltaMouseScreenSpace = mousePosition - prevFrameMousePosition;

	
	// Scene controls for self managed windows
	if (m_isWindowSelfManaged) {

		ScreenSpaceControls controls;

		while (const std::optional event = m_pWindow->pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				m_pWindow->close();

			if (m_pWindow->hasFocus()) {
				if (const sf::Event::MouseWheelScrolled* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {

					float scrollDelta = mouseWheelScrolled->delta;
					controls.zoom = scrollDelta;
				}

				if (const sf::Event::MouseButtonPressed* MouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {

					if (MouseButtonPressed->button == sf::Mouse::Button::Middle) {
						m_Camera.setTransform({ .position = Vec2(0,0), .orientation = 0, .scale = 13 });
					}
				}
			}
		}

		if (m_pWindow->hasFocus()) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

				controls.moveX = deltaMouseScreenSpace.x;
				controls.moveY = deltaMouseScreenSpace.y;
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
				
				controls.rotate = deltaMouseScreenSpace.x;
			}
		}

		updateScreenSpaceCameraControlRig(controls);
	}

	m_Camera.updateComputedState(windowInitialExtents.y);
	// Test just to make sure window updates every frame
	//m_Entities.dummyUpdateShapes(deltaTime);
}
void Scene::render()
{
	std::shared_lock<std::shared_mutex> renderConfigLock(m_RenderConfigMutex);
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
	std::unique_lock<std::shared_mutex> renderConfigLock(m_RenderConfigMutex);
	m_RenderConfig = renderConfig;
}

void Scene::updateScreenSpaceCameraControlRig(ScreenSpaceControls controls)
{
	sf::Vector2f deltaMouseScreenSpace(controls.moveX, controls.moveY);

	const Transform& cameraTransform = m_Camera.getTransform();

	// Move
	sf::Vector2f cameraDeltaWorldSpaceYFlipped = deltaMouseScreenSpace.rotatedBy(-sf::degrees(cameraTransform.orientation)) / m_Camera.getComputedState().pixelsPerWorldUnit;
	m_Camera.move(Vec2(-cameraDeltaWorldSpaceYFlipped.x, cameraDeltaWorldSpaceYFlipped.y));

	// Rotate
	m_Camera.rotate(controls.rotate / 14.0f);

	// Scale
	m_Camera.incrementScale(-controls.zoom);

	// keep in bounds
	m_Camera.validateScale();
}

EntityList* Scene::getEntityList()
{
	return &m_Entities;
}

Camera* Scene::getCamera()
{
	return &m_Camera;
}