#include <ImageWriter/Entity.h>
#include <ImageWriter/Scene.h>
#include <ImageWriter/CoreCommon.h>
#include <iostream>

Entity::Entity(Type type, const Transform& transform, Color& color) :
	m_Type(type),
	m_Transform(transform),
	m_Color(color)
{
}

EllipseEntity::EllipseEntity(const Transform& transform, Color& color, Vec2 halfExtents) :
	Entity(EntityType::Ellipse, transform, color), m_HalfExtents(halfExtents)
{

}

RectangleEntity::RectangleEntity(const Transform& transform, Color& color, Vec2 halfExtents) :
	Entity(EntityType::Rectangle, transform, color), m_HalfExtents(halfExtents)
{
}

TriangleEntity::TriangleEntity(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3) :
	Entity(EntityType::Triangle, transform, color), m_Point1(point1), m_Point2(point2), m_Point3(point3)
{
}

void EllipseEntity::applyCustomDrawingProperties(sf::VertexArray& vertexArray, sf::Shader& vertexShader) const
{
	vertexShader.setUniform("scaleOffset", sf::fromCore(m_HalfExtents));
}

void RectangleEntity::applyCustomDrawingProperties(sf::VertexArray& vertexArray, sf::Shader& vertexShader) const
{
	vertexShader.setUniform("scaleOffset", sf::fromCore(m_HalfExtents));
}

void TriangleEntity::applyCustomDrawingProperties(sf::VertexArray& vertexArray, sf::Shader& vertexShader) const
{
	vertexArray[0].position = sf::fromCore(m_Point1);
	vertexArray[1].position = sf::fromCore(m_Point2);
	vertexArray[2].position = sf::fromCore(m_Point3);

	vertexShader.setUniform("scaleOffset", sf::Glsl::Vec2(1, 1));
}

EntityList::EntityList() :
	m_EllipseVerts(sf::PrimitiveType::TriangleFan, 100),
	m_RectangleVerts(sf::PrimitiveType::TriangleStrip, 4),
	m_TriangleVerts(sf::PrimitiveType::Triangles, 3)
{

	m_RectangleVerts[0].position = sf::Vector2f(1, 1);
	m_RectangleVerts[1].position = sf::Vector2f(-1, 1);
	m_RectangleVerts[2].position = sf::Vector2f(1, -1);
	m_RectangleVerts[3].position = sf::Vector2f(-1, -1);


	constexpr float TWO_PI = 3.14159265358979f * 2;

	int numSlices = m_EllipseVerts.getVertexCount() - 2;
	float sliceSize = (float)TWO_PI / numSlices;

	m_EllipseVerts[0].position = sf::Vector2f(0, 0);
	for (int i = 1; i < m_EllipseVerts.getVertexCount(); ++i) {
		float angle = (i - 1) * sliceSize;
		m_EllipseVerts[i].position = sf::Vector2f(cos(angle), sin(angle));
	}

	if (!m_VertexShader.loadFromFile("entityShader.vert", sf::Shader::Type::Vertex)) {
		std::cerr << "Vertex shader not found" << std::endl;
		exit(1);
	}
}

EntityList::~EntityList()
{
	destroyAllShapes();
}

void EntityList::addEllipse(const Transform& transform, Color& color, Vec2 halfExtents)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Entities.push_back(new EllipseEntity(transform, color, halfExtents));
}

void EntityList::addRectangle(const Transform& transform, Color& color, Vec2 halfExtents)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Entities.push_back(new RectangleEntity(transform, color, halfExtents));
}

void EntityList::addTriangle(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Entities.push_back(new TriangleEntity(transform, color, point1, point2, point3));
}
void EntityList::dummyUpdateShapes(float deltaTime) {
	std::unique_lock<std::shared_mutex> lock(mutex);

	for (int i = 0; i < m_Entities.size(); ++i) {
		m_Entities.at(i)->m_Transform.orientation += sf::radians(deltaTime).asDegrees();
	}
}
void EntityList::drawShapes(sf::RenderWindow& window, Scene* pScene) {

	std::shared_lock<std::shared_mutex> lock(mutex);

	m_VertexShader.setUniform("homogFromCamera", sf::Glsl::Mat4(pScene->m_HomogFromCamera));
	m_VertexShader.setUniform("cameraFromWorld", sf::Glsl::Mat4(pScene->m_CameraFromWorld));

	for (int i = 0; i < m_Entities.size(); ++i) {
		const Entity* pCurrEntity = m_Entities.at(i);

		m_VertexShader.setUniform("worldFromModel", sf::Glsl::Mat4(sf::fromCore(pCurrEntity->m_Transform)));
		m_VertexShader.setUniform("color", sf::Glsl::Vec4(sf::fromCore(pCurrEntity->m_Color)));

		sf::VertexArray* pVertexArray = getVertexArray(pCurrEntity->m_Type);
		pCurrEntity->applyCustomDrawingProperties(*pVertexArray, m_VertexShader);
		window.draw(*pVertexArray, &m_VertexShader);
	}
}

void EntityList::destroyAllShapes() {

	std::unique_lock<std::shared_mutex> lock(mutex);
	for (int i = 0; i < m_Entities.size(); ++i) {
		delete m_Entities.at(i);
		m_Entities.at(i) = nullptr;
	}
	m_Entities.clear();
}

sf::VertexArray* EntityList::getVertexArray(EntityType type)
{
	switch (type) {
	case EntityType::Ellipse:
		return &m_EllipseVerts;
	case EntityType::Rectangle:
		return &m_RectangleVerts;
	case EntityType::Triangle:
		return &m_TriangleVerts;
	}
	return nullptr;
}