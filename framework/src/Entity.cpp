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
	Entity(Entity::Type::Ellipse, transform, color), m_HalfExtents(halfExtents)
{

}

RectangleEntity::RectangleEntity(const Transform& transform, Color& color, Vec2 halfExtents) :
	Entity(Entity::Type::Rectangle, transform, color), m_HalfExtents(halfExtents)
{
}

TriangleEntity::TriangleEntity(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3) :
	Entity(Entity::Type::Triangle, transform, color), m_Point1(point1), m_Point2(point2), m_Point3(point3)
{
}

void EllipseEntity::applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const
{
	vertexShader.setUniform("scaleOffset", sf::fromCore(m_HalfExtents));
}

void RectangleEntity::applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const
{
	vertexShader.setUniform("scaleOffset", sf::fromCore(m_HalfExtents));
}

void TriangleEntity::applyCustomDrawingProperties(EntityGeometry& geometry, sf::Shader& vertexShader) const
{
	std::unique_lock<std::shared_mutex> lock(geometry.m_Mutex);
	geometry[0].position = sf::fromCore(m_Point1);
	geometry[1].position = sf::fromCore(m_Point2);
	geometry[2].position = sf::fromCore(m_Point3);

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

void EntityList::addEntity(AddEntityParams params)
{
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);

	if (params.entityType == ENTITY_TYPE_ELLIPSE) {
		m_Entities.push_back(new EllipseEntity(params.transform, params.color, params.ellipse.halfExtents));
	}
	if (params.entityType == ENTITY_TYPE_RECTANGLE) {
		m_Entities.push_back(new RectangleEntity(params.transform, params.color, params.rectangle.halfExtents));
	}
	if (params.entityType == ENTITY_TYPE_TRIANGLE) {
		m_Entities.push_back(new TriangleEntity(params.transform, params.color, params.triangle.point1, params.triangle.point2, params.triangle.point3));
	}
}
void EntityList::addEllipse(const Transform& transform, Color& color, Vec2 halfExtents)
{
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);
	m_Entities.push_back(new EllipseEntity(transform, color, halfExtents));
}

void EntityList::addRectangle(const Transform& transform, Color& color, Vec2 halfExtents)
{
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);
	m_Entities.push_back(new RectangleEntity(transform, color, halfExtents));
}

void EntityList::addTriangle(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3)
{
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);
	m_Entities.push_back(new TriangleEntity(transform, color, point1, point2, point3));
}
void EntityList::dummyUpdateShapes(float deltaTime) {
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);

	for (int i = 0; i < m_Entities.size(); ++i) {
		m_Entities.at(i)->m_Transform.orientation += sf::radians(deltaTime).asDegrees();
	}
}
void EntityList::drawShapes(sf::RenderWindow& window, const Camera& camera) {

	std::shared_lock<std::shared_mutex> lock(m_EntitiesMutex);

	m_VertexShader.setUniform("homogFromWorld", camera.getComputedState().glsl_homogFromWorld);

	for (int i = 0; i < m_Entities.size(); ++i) {
		const Entity* pCurrEntity = m_Entities.at(i);

		m_VertexShader.setUniform("worldFromModel", sf::Glsl::Mat4(sf::fromCore(pCurrEntity->m_Transform)));
		m_VertexShader.setUniform("color", sf::Glsl::Vec4(sf::fromCore(pCurrEntity->m_Color)));

		EntityGeometry* pEntityGeometry = getGeometryForEntity(pCurrEntity->m_Type);
		pCurrEntity->applyCustomDrawingProperties(*pEntityGeometry, m_VertexShader);
		pEntityGeometry->draw(window, m_VertexShader);
	}
}

void EntityList::destroyAllShapes() {

	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);
	for (int i = 0; i < m_Entities.size(); ++i) {
		delete m_Entities.at(i);
		m_Entities.at(i) = nullptr;
	}
	m_Entities.clear();
}

EntityGeometry* EntityList::getGeometryForEntity(Entity::Type type)
{
	switch (type) {
	case Entity::Type::Ellipse:
		return &m_EllipseVerts;
	case Entity::Type::Rectangle:
		return &m_RectangleVerts;
	case Entity::Type::Triangle:
		return &m_TriangleVerts;
	}
	return nullptr;
}