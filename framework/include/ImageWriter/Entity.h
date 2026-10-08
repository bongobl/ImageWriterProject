#pragma once
#include <ImageWriter/API.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <shared_mutex>
#include <mutex>

class Scene;
enum class EntityType
{
	None = 0,
	Ellipse = 1,
	Rectangle = 2,
	Triangle = 3
};
struct Entity {

	using Type = EntityType;
	Type m_Type;
	Transform m_Transform;
	Color m_Color;

	Entity(Type type, const Transform& transform, Color& color);
	virtual void applyCustomDrawingProperties(sf::VertexArray& vertexArray, sf::Shader& vertexShader) const {};
};

struct EllipseEntity : Entity {

	EllipseEntity(const Transform& transform, Color& color, Vec2 halfExtents);

	Vec2 m_HalfExtents;

	void applyCustomDrawingProperties(sf::VertexArray& vertexArray, sf::Shader& vertexShader) const override;
};

struct RectangleEntity : Entity {

	RectangleEntity(const Transform& transform, Color& color, Vec2 halfExtents);

	Vec2 m_HalfExtents;

	void applyCustomDrawingProperties(sf::VertexArray& vertexArray, sf::Shader& vertexShader) const override;
};

struct TriangleEntity : Entity {
	Vec2 m_Point1;
	Vec2 m_Point2;
	Vec2 m_Point3;

	TriangleEntity(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3);

	void applyCustomDrawingProperties(sf::VertexArray& vertexArray, sf::Shader& vertexShader) const override;
};

struct EntityList {

	sf::VertexArray m_EllipseVerts;
	sf::VertexArray m_RectangleVerts;
	sf::VertexArray m_TriangleVerts;

	std::vector<Entity*> m_Entities;
	sf::Shader m_VertexShader;
	mutable std::shared_mutex mutex;
public:

	EntityList();
	~EntityList();

	void addEllipse(const Transform& transform, Color& color, Vec2 halfExtents);
	void addRectangle(const Transform& transform, Color& color, Vec2 halfExtents);
	void addTriangle(const Transform& transform, Color& color, Vec2 point1, Vec2 point2, Vec2 point3);
	void dummyUpdateShapes(float deltaTime);
	void drawShapes(sf::RenderWindow& window, Scene* pScene);
	void destroyAllShapes();

	sf::VertexArray* getVertexArray(EntityType type);
};