#include <ImageWriter/Entity.h>
#include <ImageWriter/Scene.h>
#include <ImageWriter/CoreCommon.h>
#include <iostream>

Entity::Entity(Type type, const Transform& transform, Color& color, sf::Drawable& drawable) :
	m_Type(type),
	m_Transform(transform),
	m_Color(color),
	m_Drawing(drawable)
{
}

EllipseEntity::EllipseEntity(const Transform& transform, Color& color, Vec2 halfExtents, sf::Drawable& drawable) :
	Entity(EntityType::Ellipse, transform, color, drawable), m_HalfExtents(halfExtents)
{

}

RectangleEntity::RectangleEntity(const Transform& transform, Color& color, Vec2 halfExtents, sf::Drawable& drawable) :
	Entity(EntityType::Rectangle, transform, color, drawable), m_HalfExtents(halfExtents)
{
}

TriangleEntity::TriangleEntity(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3, sf::Drawable& drawable) :
	Entity(EntityType::Triangle, transform, color, drawable), m_Point1(point1), m_Point2(point2), m_Point3(point3)
{
}

sf::Drawable& EllipseEntity::getDrawable(Scene* pScene) const 
{
	return m_Drawing;
}

sf::Drawable& RectangleEntity::getDrawable(Scene* pScene) const 
{
	return m_Drawing;
}

sf::Drawable& TriangleEntity::getDrawable(Scene* pScene) const 
{
	sf::VertexArray& triangleDrawing = static_cast<sf::VertexArray&>(m_Drawing);
	// Points
	triangleDrawing[0].position = sf::fromCore(m_Point1);
	triangleDrawing[1].position = sf::fromCore(m_Point2);
	triangleDrawing[2].position = sf::fromCore(m_Point3);

	return m_Drawing;
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

	m_EllipseVerts[0].position = sf::Vector2f(0, 0);

	constexpr float TWO_PI = 3.14159265358979 * 2;

	int numSlices = m_EllipseVerts.getVertexCount() - 2;
	float sliceSize = (float)TWO_PI / numSlices;
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
	m_Entities.push_back(new EllipseEntity(transform, color, halfExtents, m_EllipseVerts));
}

void EntityList::addRectangle(const Transform& transform, Color& color, Vec2 halfExtents)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Entities.push_back(new RectangleEntity(transform, color, halfExtents, m_RectangleVerts));
}

void EntityList::addTriangle(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	m_Entities.push_back(new TriangleEntity(transform, color, point1, point2, point3, m_TriangleVerts));
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

		switch (pCurrEntity->m_Type) {
			case Entity::Type::Triangle:
			
				m_VertexShader.setUniform("scaleOffset", sf::Glsl::Vec2(1, 1));
			
				window.draw(pCurrEntity->getDrawable(pScene), &m_VertexShader);
				break;
			case Entity::Type::Rectangle:
			{
				const RectangleEntity* pRectEntity = static_cast<const RectangleEntity*>(pCurrEntity);
				m_VertexShader.setUniform("scaleOffset", sf::fromCore(pRectEntity->m_HalfExtents));
				window.draw(pCurrEntity->getDrawable(pScene), &m_VertexShader);
				break;
			}
			case Entity::Type::Ellipse:
			{
				const EllipseEntity* pRectEntity = static_cast<const EllipseEntity*>(pCurrEntity);
				m_VertexShader.setUniform("scaleOffset", sf::fromCore(pRectEntity->m_HalfExtents));
				window.draw(pCurrEntity->getDrawable(pScene), &m_VertexShader);
				break;
			}
		}
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