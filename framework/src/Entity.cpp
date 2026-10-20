#include <ImageWriter/Entity.h>
#include <ImageWriter/Scene.h>
#include <ImageWriter/CoreCommon.h>
#include <iostream>

Entity::Entity(Type type, uint64_t id, const Transform& transform, Color& color) :
	m_Type(type),
	m_Id(id),
	m_Transform(transform),
	m_Color(color)
{
}

EllipseEntity::EllipseEntity(uint64_t id, const Transform& transform, Color& color, Vec2 halfExtents) :
	Entity(Entity::Type::Ellipse, id, transform, color), m_HalfExtents(halfExtents)
{

}

RectangleEntity::RectangleEntity(uint64_t id, const Transform& transform, Color& color, Vec2 halfExtents) :
	Entity(Entity::Type::Rectangle, id, transform, color), m_HalfExtents(halfExtents)
{
}

TriangleEntity::TriangleEntity(uint64_t id, const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3) :
	Entity(Entity::Type::Triangle, id, transform, color), m_Point1(point1), m_Point2(point2), m_Point3(point3)
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
	m_TriangleVerts(sf::PrimitiveType::Triangles, 3),
	m_NextEntityId(0x0)
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
		m_Entities.push_back(new EllipseEntity(m_NextEntityId, params.transform, params.color, params.ellipse.halfExtents));
	}
	if (params.entityType == ENTITY_TYPE_RECTANGLE) {
		m_Entities.push_back(new RectangleEntity(m_NextEntityId, params.transform, params.color, params.rectangle.halfExtents));
	}
	if (params.entityType == ENTITY_TYPE_TRIANGLE) {
		m_Entities.push_back(new TriangleEntity(m_NextEntityId, params.transform, params.color, params.triangle.point1, params.triangle.point2, params.triangle.point3));
	}

	m_EntityIterFromId[m_NextEntityId] = --m_Entities.end();
	++m_NextEntityId;
}

bool EntityList::removeEntity(uint64_t id)
{
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);

	auto mapIter = m_EntityIterFromId.find(id);

	// If not found in map
	if (mapIter == m_EntityIterFromId.end()) {
		std::cerr << "Entity id " << id << " not found in m_EntityIterFromId map" << std::endl;
		return false;
	}

	IterT entityIter = mapIter->second;

	// delete entity
	delete *entityIter;
	*entityIter = nullptr;

	// remove from list and id map
	m_Entities.erase(entityIter);
	m_EntityIterFromId.erase(id);

	return true;
}
void EntityList::dummyUpdateShapes(float deltaTime) {
	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);

	for (auto iter = m_Entities.begin(); iter != m_Entities.end(); ++iter) {
		Entity* pCurrEntity = *iter;
		pCurrEntity->m_Transform.orientation += sf::radians(deltaTime).asDegrees();
	}
}
void EntityList::drawShapes(sf::RenderWindow& window, const Camera& camera) {

	std::shared_lock<std::shared_mutex> lock(m_EntitiesMutex);

	m_VertexShader.setUniform("homogFromWorld", camera.getComputedState().glsl_homogFromWorld);

	for (auto iter = m_Entities.begin(); iter != m_Entities.end(); ++iter) {
		const Entity* pCurrEntity = *iter;

		m_VertexShader.setUniform("worldFromModel", sf::Glsl::Mat4(sf::fromCore(pCurrEntity->m_Transform)));
		m_VertexShader.setUniform("color", sf::Glsl::Vec4(sf::fromCore(pCurrEntity->m_Color)));

		EntityGeometry* pEntityGeometry = getGeometryForEntity(pCurrEntity->m_Type);
		pCurrEntity->applyCustomDrawingProperties(*pEntityGeometry, m_VertexShader);
		pEntityGeometry->draw(window, m_VertexShader);
	}
}

void EntityList::destroyAllShapes() {

	std::unique_lock<std::shared_mutex> lock(m_EntitiesMutex);
	for (auto iter = m_Entities.begin(); iter != m_Entities.end(); ++iter) {
		delete *iter;
		*iter = nullptr;
	}
	m_Entities.clear();
	m_EntityIterFromId.clear();
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