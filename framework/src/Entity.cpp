#include <ImageWriter/Entity.h>
#include <ImageWriter/CoreCommon.h>

Entity::Entity(Type type, const Transform& transform, Color& color, sf::Shape& drawable) :
	m_Type(type),
	m_Transform(transform),
	m_Color(color),
	m_Drawing(drawable)
{
}

EllipseEntity::EllipseEntity(const Transform& transform, Color& color, sf::Shape& drawable) : 
	Entity(EntityType::Ellipse, transform, color, drawable)
{

}

RectangleEntity::RectangleEntity(const Transform& transform, Color& color, sf::Shape& drawable) :
	Entity(EntityType::Rectangle, transform, color, drawable) 
{
}

TriangleEntity::TriangleEntity(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3, sf::Shape& drawable) :
	Entity(EntityType::Triangle, transform, color, drawable), m_Point1(point1), m_Point2(point2), m_Point3(point3)
{
}

sf::Shape& EllipseEntity::getDrawable(CoreData* pCoreData) const {

	sf::Vector2f screenHalfExtents = pCoreData->pixelsPerWorldUnit * sf::Vector2f(m_Transform.scaleX, m_Transform.scaleY);

	sf::CircleShape& circleDrawing = static_cast<sf::CircleShape&>(m_Drawing);
	circleDrawing.setFillColor(sf::fromCore(m_Color));
	circleDrawing.setPosition(pCoreData->screenFromCamera * pCoreData->cameraFromWorld * sf::Vector2f(m_Transform.positionX, m_Transform.positionY));

	// Note on rotation: arithmetic looks ugly but cleanly shows screen <- view <- world <- model conversion
	circleDrawing.setRotation(-sf::degrees(-pCoreData->m_Camera.getOrientation() + m_Transform.angle));
	circleDrawing.setScale(screenHalfExtents);

	return circleDrawing;
}

sf::Shape& RectangleEntity::getDrawable(CoreData* pCoreData) const {

	sf::Vector2f screenHalfExtents = pCoreData->pixelsPerWorldUnit * sf::Vector2f(m_Transform.scaleX, m_Transform.scaleY);

	sf::RectangleShape& rectDrawing = static_cast<sf::RectangleShape&>(m_Drawing);
	rectDrawing.setFillColor(sf::fromCore(m_Color));
	rectDrawing.setPosition(pCoreData->screenFromCamera * pCoreData->cameraFromWorld * sf::Vector2f(m_Transform.positionX, m_Transform.positionY));
	
	// Note on rotation: arithmetic looks ugly but cleanly shows screen <- view <- world <- model conversion
	rectDrawing.setRotation(-sf::degrees(-pCoreData->m_Camera.getOrientation() + m_Transform.angle));

	rectDrawing.setScale(screenHalfExtents);

	return rectDrawing;
}

sf::Shape& TriangleEntity::getDrawable(CoreData* pCoreData) const {
	sf::ConvexShape& triangleDrawing = static_cast<sf::ConvexShape&>(m_Drawing);

	triangleDrawing.setFillColor(sf::fromCore(m_Color));

	sf::Transform screenFromWorld = pCoreData->screenFromCamera * pCoreData->cameraFromWorld;

	triangleDrawing.setOrigin(screenFromWorld * sf::Vector2f(0, 0));

	// Points
	triangleDrawing.setPoint(0, screenFromWorld * sf::fromCore(m_Point1));
	triangleDrawing.setPoint(1, screenFromWorld * sf::fromCore(m_Point2));
	triangleDrawing.setPoint(2, screenFromWorld * sf::fromCore(m_Point3));

	triangleDrawing.setPosition(screenFromWorld * sf::Vector2f(m_Transform.positionX, m_Transform.positionY));
	triangleDrawing.setRotation(-sf::degrees(m_Transform.angle));
	triangleDrawing.setScale(sf::Vector2f(m_Transform.scaleX, m_Transform.scaleY));	// For triangles, scale is specified in world space (not screen space)

	return triangleDrawing;
}

EntityList::EntityList() :
	m_CircleDrawing(1.0f), // <- radius
	m_RectangleDrawing(sf::Vector2f(2, 2)), // <- width and height
	m_TriangleDrawing(3) // <- num points
{
	m_CircleDrawing.setOrigin(sf::Vector2f(1, 1));
	m_RectangleDrawing.setOrigin(sf::Vector2f(1, 1));

	m_RectangleDrawing.setOrigin(sf::Vector2f(1, 1));
}

EntityList::~EntityList()
{
	destroyAllShapes();
}
void EntityList::addShape(Entity* pEntity) {

	std::unique_lock<std::shared_mutex> lock(mutex);

	m_Entities.push_back(pEntity);
}

void EntityList::addEllipse(const Transform& transform, Color& color) 
{
	m_Entities.push_back(new EllipseEntity(transform, color, m_CircleDrawing));
}

void EntityList::addRectangle(const Transform& transform, Color& color)
{
	m_Entities.push_back(new RectangleEntity(transform, color, m_RectangleDrawing));
}

void EntityList::addTriangle(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3)
{
	m_Entities.push_back(new TriangleEntity(transform, color, point1, point2, point3, m_TriangleDrawing));
}
void EntityList::dummyUpdateShapes(float deltaTime) {
	std::shared_lock<std::shared_mutex> lock(mutex);

	for (int i = 0; i < m_Entities.size(); ++i) {
		m_Entities.at(i)->m_Transform.angle += sf::radians(deltaTime).asDegrees();
	}
}
void EntityList::drawShapes(sf::RenderWindow& window, CoreData* pCoreData) {

	std::shared_lock<std::shared_mutex> lock(mutex);

	for (int i = 0; i < m_Entities.size(); ++i) {
		window.draw(m_Entities.at(i)->getDrawable(pCoreData));
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